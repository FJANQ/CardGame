/**
 * @file GameScene.h
 * @brief 游戏主场景
 * @details Cocos2d-x 场景入口，负责持有 GameController。
 */
#pragma once
#include "cocos2d.h"
#include "GameController.h"

class GameScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameScene);

private:
    GameController* _controller;
};