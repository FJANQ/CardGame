//#include "HelloWorldScene.h"
//
//USING_NS_CC;
//
//Scene* HelloWorld::createScene()
//{
//    return HelloWorld::create();
//}
//
//// --- 核心修改：替换 init() 的内容 ---
//bool HelloWorld::init()
//{
//    // 1. 父类初始化必须保留
//    if (!Scene::init())
//    {
//        return false;
//    }
//
//    // 2. 为了看清卡牌，我们加一个深灰色的背景层
//    // 因为默认背景是黑色的，如果以后卡牌素材也是深色就看不清了
//    auto bg = LayerColor::create(Color4B(50, 50, 50, 255));
//    this->addChild(bg);
//
//    // 3. 创建并持有 Controller
//    _gameController = GameController::create();
//    // 极其重要：必须 retain()，否则会被 Cocos 自动内存管理机制回收导致崩溃
//    _gameController->retain();
//
//    // 4. 把当前的 Layer (也就是 this) 传给 Controller，让它在上面画画
//    _gameController->startGame(this);
//
//    return true;
//}
//
//// --- 新增：实现析构函数 ---
//HelloWorld::~HelloWorld() {
//    // 极其重要：退出场景时释放 Controller，防止内存泄漏
//    if (_gameController) {
//        _gameController->release();
//        _gameController = nullptr;
//    }
//}