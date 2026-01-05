#include "GameController.h"
#include "GameModelFromLevelGenerator.h"
#include "GameConsts.h"
#include "GameScene.h" 

USING_NS_CC;

// =============================================================
// 生命周期与初始化
// =============================================================

GameController* GameController::create() {
    GameController* pRet = new(std::nothrow) GameController();
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    }
    delete pRet;
    return nullptr;
}

bool GameController::init() {
    _undoManager = std::unique_ptr<UndoManager>(new UndoManager());
    return true;
}

void GameController::startGame(int levelId) {
    // 1. 初始化数据和视图
    initLevelData(levelId);
    initViewLayers();

    // 2. 创建各个区域
    createTableCards();
    setupInitialStack();
    setupInitialStock();
    createHUDButtons();

    // 3. 初始状态检查
    refreshPlayfieldStatus();
    checkGameState();
}

void GameController::initLevelData(int levelId) {
    _gameModel = GameModelFromLevelGenerator::generate(levelId);
}

void GameController::initViewLayers() {
    _gameView = GameView::create();
    _gameView->retain();
}

void GameController::createTableCards() {
    for (auto& card : _gameModel->getPlayfieldCards()) {
        auto cardView = CardView::create(card);
        cardView->setClickCallback([this](int id) {
            this->handleCardClick(id);
            });
        _gameView->addCardView(cardView, card->getPosition(), card->getZIndex());
    }
}

void GameController::setupInitialStack() {
    auto stackCard = _gameModel->getStackTopCard();
    if (stackCard) {
        auto stackView = CardView::create(stackCard);
        Vec2 pos(GameConsts::kStackPosX, GameConsts::kStackPosY);
        stackView->setPosition(pos);
        stackCard->setPosition(pos); // 同步Model位置
        _gameView->addCardView(stackView, pos, 100);
    }
}

void GameController::setupInitialStock() {
    auto onStockClick = [this]() { this->handleStockClick(); };
    _gameView->refreshStockPile(_gameModel->getStockCards(), onStockClick);
}

void GameController::createHUDButtons() {
    // 回退按钮 (右下)
    _gameView->createUndoButton(Vec2(850, 200), [this]() {
        this->onUndoClicked();
        });

    // 重启按钮 (左下)
    _gameView->createRestartButton(Vec2(230, 200), [this]() {
        auto scene = GameScene::createScene();
        Director::getInstance()->replaceScene(scene);
        });
}

// =============================================================
// 翻牌逻辑 (Stock)
// =============================================================

void GameController::handleStockClick() {
    if (!_gameModel->hasStockCards()) {
        checkGameState();
        return;
    }

    // 1. 数据弹出
    auto newCard = _gameModel->popStockCard();

    // 2. 使用调度器延迟 UI 操作 (防崩溃)
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([this, newCard]() {
        this->performStockFlip(newCard);
        });
}

void GameController::performStockFlip(std::shared_ptr<CardModel> newCard) {
    // 1. 刷新备用堆 (视觉上少一张)
    _gameView->refreshStockPile(_gameModel->getStockCards(), [this]() {
        this->handleStockClick();
        });

    // 2. 创建飞行的卡牌
    auto cardView = CardView::create(newCard);
    Vec2 startPos(GameConsts::kStockPosX, GameConsts::kStockPosY);
    Vec2 endPos(GameConsts::kStackPosX, GameConsts::kStackPosY);

    cardView->setPosition(startPos);
    _gameView->addCardView(cardView, startPos, 200);

    // 3. 处理旧手牌隐藏
    auto oldStackTop = _gameModel->getStackTopCard();
    if (oldStackTop) {
        auto oldView = _gameView->getCardViewById(oldStackTop->getId());
        if (oldView) oldView->setVisible(false);
    }

    // 4. 更新 Model
    _gameModel->setStackTopCard(newCard);

    // 5. 记录 Undo
    UndoStep step;
    step.type = UndoActionType::STOCK_TO_STACK;
    step.cardMoved = newCard;
    step.oldStackTop = oldStackTop;
    step.originalPos = startPos;
    if (_undoManager) _undoManager->pushStep(step);

    // 6. 播放动画
    _gameView->playCardMoveAnim(newCard->getId(), endPos, [this]() {
        this->checkGameState();
        });
}

// =============================================================
// 桌面卡牌交互逻辑 (Playfield)
// =============================================================

void GameController::handleCardClick(int cardId) {
    auto clickedCard = _gameModel->getCardById(cardId);
    auto stackTop = _gameModel->getStackTopCard();

    if (!clickedCard || !stackTop) return;

    if (tryMatch(clickedCard, stackTop)) {
        executeMatch(clickedCard);
    }
}

bool GameController::tryMatch(std::shared_ptr<CardModel> clickCard, std::shared_ptr<CardModel> stackCard) {
    int face1 = (int)clickCard->getFace();
    int face2 = (int)stackCard->getFace();

    if (abs(face1 - face2) == 1) return true;
    if ((face1 == 1 && face2 == 13) || (face1 == 13 && face2 == 1)) return true;

    return false;
}

void GameController::executeMatch(std::shared_ptr<CardModel> clickCard) {
    auto oldStackTop = _gameModel->getStackTopCard();

    // 1. 记录 Undo
    UndoStep step;
    step.type = UndoActionType::PLAYFIELD_TO_STACK;
    step.cardMoved = clickCard;
    step.oldStackTop = oldStackTop;
    step.originalPos = clickCard->getPosition();
    if (_undoManager) _undoManager->pushStep(step);

    // 2. 隐藏旧底牌
    if (oldStackTop) {
        auto oldView = _gameView->getCardViewById(oldStackTop->getId());
        if (oldView) {
            oldView->setLocalZOrder(-1);
            oldView->setVisible(false);
        }
    }

    // 3. 更新 Model
    _gameModel->setStackTopCard(clickCard);
    _gameModel->removePlayfieldCard(clickCard);

    // 4. 动画
    Vec2 stackPos(GameConsts::kStackPosX, GameConsts::kStackPosY);
    _gameView->playCardMoveAnim(clickCard->getId(), stackPos, [this]() {
        this->refreshPlayfieldStatus();
        this->checkGameState();
        });
}

// =============================================================
// 回退逻辑 (Undo)
// =============================================================

void GameController::onUndoClicked() {
    if (!_undoManager || !_undoManager->hasUndo()) return;

    UndoStep step = _undoManager->popStep();

    if (step.type == UndoActionType::PLAYFIELD_TO_STACK) {
        auto currentStack = step.cardMoved;
        auto prevStack = step.oldStackTop;

        _gameModel->setStackTopCard(prevStack);
        _gameModel->addPlayfieldCard(currentStack);

        if (prevStack) {
            auto oldView = _gameView->getCardViewById(prevStack->getId());
            if (oldView) {
                oldView->setVisible(true);
                oldView->setLocalZOrder(10);
            }
        }

        _gameView->playCardMoveAnim(currentStack->getId(), step.originalPos, [this]() {
            this->refreshPlayfieldStatus();
            });
    }
    else if (step.type == UndoActionType::STOCK_TO_STACK) {
        auto currentStack = step.cardMoved;
        auto prevStack = step.oldStackTop;

        _gameModel->setStackTopCard(prevStack);
        _gameModel->addStockCard(currentStack);

        if (prevStack) {
            auto oldView = _gameView->getCardViewById(prevStack->getId());
            if (oldView) oldView->setVisible(true);
        }

        _gameView->playCardMoveAnim(currentStack->getId(), step.originalPos, [this, currentStack]() {
            auto view = _gameView->getCardViewById(currentStack->getId());
            if (view) view->removeFromParent();

            // 刷新备用牌堆显示
            auto onStockClick = [this]() { this->handleStockClick(); };
            _gameView->refreshStockPile(_gameModel->getStockCards(), onStockClick);

            this->checkGameState();
            });
    }
}

// =============================================================
// 状态检测逻辑
// =============================================================

void GameController::refreshPlayfieldStatus() {
    auto& cards = _gameModel->getPlayfieldCards();

    for (auto& cardA : cards) {
        bool isBlocked = false;
        Rect rectA(cardA->getPosition().x - 50, cardA->getPosition().y - 70, 100, 140);

        for (auto& cardB : cards) {
            if (cardA == cardB) continue;
            // 判定：Y坐标较小(屏幕下方)的牌 挡住 Y坐标较大(屏幕上方)的牌
            if (cardB->getPosition().y < cardA->getPosition().y) {
                Rect rectB(cardB->getPosition().x - 50, cardB->getPosition().y - 70, 100, 140);
                if (rectA.intersectsRect(rectB)) {
                    isBlocked = true;
                    break;
                }
            }
        }

        cardA->setBlocked(isBlocked);
        auto view = _gameView->getCardViewById(cardA->getId());
        if (view) view->setEnabled(!isBlocked);
    }
}

bool GameController::hasAvailableMoves() {
    auto stackTop = _gameModel->getStackTopCard();
    if (!stackTop) return false;

    for (auto& card : _gameModel->getPlayfieldCards()) {
        if (!card->isBlocked()) {
            if (tryMatch(card, stackTop)) return true;
        }
    }
    return false;
}

void GameController::checkGameState() {
    // 胜利
    if (_gameModel->getPlayfieldCards().empty()) {
        cocos2d::log("WIN");
        _gameView->showWinLabel();
        return;
    }

    // 失败
    bool isStockEmpty = !_gameModel->hasStockCards();
    bool canMove = hasAvailableMoves();

    if (isStockEmpty && !canMove) {
        cocos2d::log("GAME OVER");
        _gameView->showGameOverLabel();
    }
}