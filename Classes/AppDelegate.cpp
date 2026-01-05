#include "AppDelegate.h"
#include "GameScene.h"
#include "GameConsts.h"

USING_NS_CC;

AppDelegate::AppDelegate() {}
AppDelegate::~AppDelegate() {}

void AppDelegate::initGLContextAttrs() {
    GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8, 0 };
    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();

    if (!glview) {
        // Windows 调试窗口大小 (缩小一半方便看)
        glview = GLViewImpl::createWithRect("CardGame",
            Rect(0, 0, GameConsts::kDesignWidth * 0.5f, GameConsts::kDesignHeight * 0.5f));
        director->setOpenGLView(glview);
    }

    // 设置设计分辨率 (适配手机)
    glview->setDesignResolutionSize(
        GameConsts::kDesignWidth,
        GameConsts::kDesignHeight,
        ResolutionPolicy::FIXED_WIDTH
    );

    // 显示 FPS
    director->setDisplayStats(true);
    director->setAnimationInterval(1.0f / 60);

    // 运行游戏场景
    auto scene = GameScene::createScene();
    director->runWithScene(scene);

    return true;
}

void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
}