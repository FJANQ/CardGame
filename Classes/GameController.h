/**
 * @file GameController.h
 * @brief 游戏核心控制器
 * @details 负责协调 Model 和 View，处理业务逻辑（规则判定、流程控制）。
 *          遵循 MVC 模式，作为 User Input 的入口。
 */
#pragma once
#include "cocos2d.h"
#include "GameModel.h"
#include "GameView.h"
#include "UndoManager.h"

class GameController : public cocos2d::Ref {
public:
    static GameController* create();

    /**
     * @brief 开始一局新游戏
     * @param levelId 关卡ID
     */
    void startGame(int levelId);

    GameView* getGameView() { return _gameView; }

    // 输入事件处理
    void onUndoClicked();

private:
    bool init();

    // --- 内部逻辑拆分 (遵循50行限制) ---
    void initLevelData(int levelId);    // 初始化数据
    void initViewLayers();              // 初始化视图层
    void createTableCards();            // 创建桌面牌
    void createHUDButtons();            // 创建UI按钮
    void setupInitialStack();           // 设置初始手牌
    void setupInitialStock();           // 设置初始备用堆

    void handleCardClick(int cardId);
    void handleStockClick();            // 处理翻牌点击
    void performStockFlip(std::shared_ptr<CardModel> newCard); // 执行翻牌的具体动作

    bool tryMatch(std::shared_ptr<CardModel> a, std::shared_ptr<CardModel> b);
    void executeMatch(std::shared_ptr<CardModel> clickCard);

    void refreshPlayfieldStatus();      // 刷新遮挡关系
    bool hasAvailableMoves();           // 检查死局
    void checkGameState();              // 检查输赢

    // 成员变量
    std::shared_ptr<GameModel> _gameModel;
    GameView* _gameView;
    std::unique_ptr<UndoManager> _undoManager;
};