#include "GameScene.h"

USING_NS_CC;

Scene* GameScene::createScene() {
    return GameScene::create();
}

bool GameScene::init() {
    if (!Scene::init()) return false;

    // 初始化控制器
    _controller = GameController::create();
    if (_controller) {
        _controller->retain(); // 保持引用

        // 启动第一关
        _controller->startGame(1);

        // 将游戏视图添加到场景
        auto view = _controller->getGameView();
        if (view) {
            this->addChild(view);
        }
    }

    return true;
}