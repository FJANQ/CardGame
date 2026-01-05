#include "CardView.h"
#include "GameConsts.h"

USING_NS_CC;

// =================================================================
// 1. 静态拼装逻辑 (拼图外观)
// =================================================================
Node* CardView::createVisualNode(std::shared_ptr<CardModel> model) {
    // 创建底图
    auto bgSprite = Sprite::create("card_general.png");
    if (!bgSprite) {
        // 容错：没图就画白块
        auto node = LayerColor::create(Color4B::WHITE, 100, 140);
        node->ignoreAnchorPointForPosition(false);
        return node;
    }

    Size size = bgSprite->getContentSize();

    // 解析颜色
    std::string colorStr = "black";
    int suit = (int)model->getSuit();
    if (suit == (int)CardSuitType::DIAMONDS || suit == (int)CardSuitType::HEARTS) {
        colorStr = "red";
    }

    // 解析点数
    std::string faceStr;
    int face = (int)model->getFace();
    switch (face) {
    case 1: faceStr = "A"; break;
    case 11: faceStr = "J"; break;
    case 12: faceStr = "Q"; break;
    case 13: faceStr = "K"; break;
    default: faceStr = StringUtils::format("%d", face); break;
    }

    // 拼装文件名
    std::string smallName = StringUtils::format("small_%s_%s.png", colorStr.c_str(), faceStr.c_str());
    std::string bigName = StringUtils::format("big_%s_%s.png", colorStr.c_str(), faceStr.c_str());
    std::string suitName;
    switch (model->getSuit()) {
    case CardSuitType::CLUBS:    suitName = "club.png"; break;
    case CardSuitType::DIAMONDS: suitName = "diamond.png"; break;
    case CardSuitType::HEARTS:   suitName = "heart.png"; break;
    case CardSuitType::SPADES:   suitName = "spade.png"; break;
    default: suitName = ""; break;
    }

    // 添加左上角小数字
    auto smallSprite = Sprite::create(smallName);
    if (smallSprite) {
        smallSprite->setAnchorPoint(Vec2(0, 1));
        smallSprite->setPosition(size.width * 0.08f, size.height * 0.92f);
        bgSprite->addChild(smallSprite);
    }

    // 添加右上角花色
    auto suitSprite = Sprite::create(suitName);
    if (suitSprite) {
        suitSprite->setAnchorPoint(Vec2(1, 1));
        suitSprite->setPosition(size.width * 0.92f, size.height * 0.92f);
        suitSprite->setScale(0.5f);
        bgSprite->addChild(suitSprite);
    }

    // 添加中间大数字
    auto bigSprite = Sprite::create(bigName);
    if (bigSprite) {
        bigSprite->setPosition(size.width / 2, size.height / 2);
        bgSprite->addChild(bigSprite);
    }

    return bgSprite;
}

// =================================================================
// 2. 核心生命周期 (create, init) - 修复“无法解析 create”
// =================================================================
CardView* CardView::create(std::shared_ptr<CardModel> model) {
    CardView* pRet = new(std::nothrow) CardView();
    if (pRet && pRet->init(model)) {
        pRet->autorelease();
        return pRet;
    }
    delete pRet;
    return nullptr;
}

bool CardView::init(std::shared_ptr<CardModel> model) {
    if (!Node::init()) return false;
    _model = model;

    this->setCascadeOpacityEnabled(true);

    // 调用拼装逻辑
    auto visualNode = CardView::createVisualNode(model);

    // 设置尺寸
    this->setContentSize(visualNode->getContentSize());
    this->setAnchorPoint(Vec2(0.5f, 0.5f));

    // 居中显示
    visualNode->setPosition(this->getContentSize() / 2);
    this->addChild(visualNode);

    // 初始化触摸
    initTouchListener();

    return true;
}

// =================================================================
// 3. 状态控制 (setEnabled) - 修复“无法解析 setEnabled”
// =================================================================
void CardView::setEnabled(bool enabled) {
    // 启用: 不透明(255)
    // 禁用/被挡住: 半透明(150)
    this->setOpacity(enabled ? 255 : 150);
}

// =================================================================
// 4. 触摸事件处理
// =================================================================
void CardView::initTouchListener() {
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        // 1. 检查强制开关
        if (!_isTouchEnabled) return false;

        // 2. 检查逻辑开关 (是否被遮挡/变暗)
        if (this->getOpacity() < 255) return false;

        // 3. 检查是否有回调 (无回调则穿透，给透明按钮)
        if (!_clickCallback) return false;

        Vec2 p = this->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0, 0, this->getContentSize().width, this->getContentSize().height);
        return rect.containsPoint(p);
        };

    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        // 【防崩溃保命符】
        this->retain();

        Vec2 p = this->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0, 0, this->getContentSize().width, this->getContentSize().height);

        if (rect.containsPoint(p)) {
            if (_clickCallback) {
                _clickCallback(_model->getId());
            }
        }

        // 【防崩溃保命符】
        this->autorelease();
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}