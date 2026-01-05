#include "GameView.h"
#include "GameConsts.h"

USING_NS_CC;

GameView* GameView::create() {
    GameView* pRet = new(std::nothrow) GameView();
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    }
    delete pRet;
    return nullptr;
}

bool GameView::init() {
    if (!Layer::init()) return false;

    // 背景
    auto background = LayerColor::create(Color4B(20, 80, 30, 255));
    this->addChild(background, -1);

    // 层容器
    _playfieldLayer = Node::create();
    this->addChild(_playfieldLayer);
    _stackLayer = Node::create();
    this->addChild(_stackLayer);

    // 透明按钮 (备用牌点击区)
    _stockClickNode = LayerColor::create(Color4B(0, 0, 0, 0), 100, 140);
    _stockClickNode->ignoreAnchorPointForPosition(false);
    _stockClickNode->setAnchorPoint(Vec2(0.5f, 0.5f));
    _stockClickNode->setPosition(GameConsts::kStockPosX, GameConsts::kStockPosY);
    _stockClickNode->setVisible(false);
    _stackLayer->addChild(_stockClickNode, 9999);

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* t, Event* e) {
        auto target = e->getCurrentTarget();
        if (!target->isVisible()) return false;
        Vec2 p = target->convertTouchToNodeSpace(t);
        Rect rect(0, 0, 100, 140);
        return rect.containsPoint(p);
        };
    listener->onTouchEnded = [this](Touch* t, Event* e) {
        if (_onStockClickCallback) _onStockClickCallback();
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, _stockClickNode);

    return true;
}

void GameView::addCardView(CardView* cardView, const cocos2d::Vec2& pos, int zOrder) {
    if (!cardView) return;
    cardView->setPosition(pos);
    _playfieldLayer->addChild(cardView, zOrder);
    _cardViews[cardView->getCardId()] = cardView;
}

CardView* GameView::getCardViewById(int id) {
    auto it = _cardViews.find(id);
    if (it != _cardViews.end()) return it->second;
    return nullptr;
}

void GameView::playCardMoveAnim(int cardId, const cocos2d::Vec2& endPos, std::function<void()> onComplete) {
    auto cardView = getCardViewById(cardId);
    if (!cardView) return;

    cardView->setLocalZOrder(1000);

    auto move = MoveTo::create(GameConsts::kCardMoveDuration, endPos);
    auto scale = ScaleTo::create(0.15f, 1.2f);
    auto scaleBack = ScaleTo::create(0.15f, 1.0f);

    auto seq = Sequence::create(
        Spawn::create(move, Sequence::create(scale, scaleBack, nullptr), nullptr),
        CallFunc::create([onComplete]() { if (onComplete) onComplete(); }),
        nullptr
    );
    cardView->runAction(seq);
}

// =============================================================
// 备用牌堆绘制 (重构后)
// =============================================================

void GameView::refreshStockPile(const std::vector<std::shared_ptr<CardModel>>& stockCards,
    std::function<void()> onTopCardClick) {
    clearOldStockViews();

    _onStockClickCallback = onTopCardClick;

    if (stockCards.empty()) {
        if (_stockClickNode) _stockClickNode->setVisible(false);
        return;
    }

    if (_stockClickNode) {
        _stockClickNode->setVisible(true);
        int total = (int)stockCards.size();
        int drawCount = std::min(total, 5) - 1;
        float offsetX = drawCount * 3.0f;
        float offsetY = drawCount * 2.0f;
        _stockClickNode->setPosition(Vec2(GameConsts::kStockPosX + offsetX,
            GameConsts::kStockPosY + offsetY));
    }

    drawStaticStockCards(stockCards);
}

void GameView::clearOldStockViews() {
    for (auto node : _stockCardViews) {
        node->removeFromParent();
    }
    _stockCardViews.clear();
}

void GameView::drawStaticStockCards(const std::vector<std::shared_ptr<CardModel>>& stockCards) {
    int total = (int)stockCards.size();
    int drawCount = 0;
    int startIndex = std::max(0, total - 5);

    for (int i = startIndex; i < total; ++i) {
        auto cardData = stockCards[i];

        // 使用 CardView 的静态拼装方法
        auto cardNode = CardView::createVisualNode(cardData);

        float offsetX = drawCount * 3.0f;
        float offsetY = drawCount * 2.0f;
        cardNode->setPosition(Vec2(GameConsts::kStockPosX + offsetX,
            GameConsts::kStockPosY + offsetY));

        _stackLayer->addChild(cardNode, i);
        _stockCardViews.push_back(cardNode);

        drawCount++;
    }
}

// =============================================================
// UI 辅助
// =============================================================

void GameView::createUndoButton(const cocos2d::Vec2& pos, std::function<void()> onClick) {
    auto btnSprite = Sprite::create("ui/undo_button.png");
    if (!btnSprite) { // 容错：没图就用方块
        auto layer = LayerColor::create(Color4B(200, 200, 0, 255), 120, 60);
        layer->setPosition(pos.x - 60, pos.y - 30);
        _stackLayer->addChild(layer);
        auto label = Label::createWithSystemFont("UNDO", "Arial", 24);
        label->setColor(Color3B::BLACK);
        label->setPosition(pos);
        _stackLayer->addChild(label, 10);
    }
    else {
        btnSprite->setPosition(pos);
        _stackLayer->addChild(btnSprite);
    }

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [pos](Touch* t, Event* e) {
        Rect rect(pos.x - 60, pos.y - 30, 120, 60);
        return rect.containsPoint(t->getLocation());
        };
    listener->onTouchEnded = [onClick](Touch* t, Event* e) {
        if (onClick) onClick();
        };
    // 简单绑在层上，实际应绑在 Sprite 上
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, _stackLayer);
}

void GameView::createRestartButton(const cocos2d::Vec2& pos, std::function<void()> onClick) {
    auto layer = LayerColor::create(Color4B(0, 0, 200, 255), 140, 60);
    layer->setPosition(pos.x - 70, pos.y - 30);
    _stackLayer->addChild(layer);
    auto label = Label::createWithSystemFont("RESTART", "Arial", 24);
    label->setPosition(pos);
    _stackLayer->addChild(label, 10);

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [pos](Touch* t, Event* e) {
        Rect rect(pos.x - 70, pos.y - 30, 140, 60);
        return rect.containsPoint(t->getLocation());
        };
    listener->onTouchEnded = [onClick](Touch* t, Event* e) {
        if (onClick) onClick();
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, layer);
}

void GameView::showWinLabel() {
    auto label = Label::createWithSystemFont("YOU WIN!", "Arial", 80);
    label->setColor(Color3B::YELLOW);
    label->setPosition(Vec2(540, 1200));
    this->addChild(label, 999);
    label->setScale(0);
    label->runAction(EaseBackOut::create(ScaleTo::create(0.5f, 1.0f)));
}

void GameView::showGameOverLabel() {
    auto label = Label::createWithSystemFont("GAME OVER", "Arial", 80);
    label->setColor(Color3B::RED);
    label->setPosition(Vec2(540, 1200));
    this->addChild(label, 999);
    label->setScale(0);
    label->runAction(EaseBackOut::create(ScaleTo::create(0.5f, 1.0f)));
}