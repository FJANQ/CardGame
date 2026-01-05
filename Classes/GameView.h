/**
 * @file GameView.h
 * @brief 游戏主场景视图层
 * @details 负责管理桌面牌、手牌、备用牌堆及UI按钮的渲染。
 *          不持有游戏核心逻辑，通过回调与 Controller 通信。
 */
#pragma once
#include "cocos2d.h"
#include "CardView.h"
#include <map>
#include <vector>

class GameView : public cocos2d::Layer {
public:
    static GameView* create();
    virtual bool init();

    /**
     * @brief 添加一张交互式卡牌到桌面
     */
    void addCardView(CardView* cardView, const cocos2d::Vec2& pos, int zOrder);

    /**
     * @brief 根据ID查找卡牌视图
     */
    CardView* getCardViewById(int id);

    /**
     * @brief 播放卡牌移动动画
     */
    void playCardMoveAnim(int cardId, const cocos2d::Vec2& endPos, std::function<void()> onComplete);

    /**
     * @brief 刷新备用牌堆显示（核心防崩溃逻辑）
     * @param stockCards 剩余的卡牌数据
     * @param onTopCardClick 点击透明区域时的回调
     */
    void refreshStockPile(const std::vector<std::shared_ptr<CardModel>>& stockCards,
        std::function<void()> onTopCardClick);

    // UI按钮创建方法
    void createUndoButton(const cocos2d::Vec2& pos, std::function<void()> onClick);
    void createRestartButton(const cocos2d::Vec2& pos, std::function<void()> onClick);
    void showWinLabel();
    void showGameOverLabel();

private:
    /**
     * @brief 内部辅助：清理旧的备用牌堆视图
     */
    void clearOldStockViews();

    /**
     * @brief 内部辅助：绘制新的静态备用牌
     */
    void drawStaticStockCards(const std::vector<std::shared_ptr<CardModel>>& stockCards);

    // 成员变量
    cocos2d::Node* _playfieldLayer;   // 桌面层
    cocos2d::Node* _stackLayer;       // 操作层
    cocos2d::Node* _stockClickNode;   // 透明点击区域

    std::map<int, CardView*> _cardViews;            // 桌面活牌缓存
    std::vector<cocos2d::Node*> _stockCardViews;    // 备用堆死牌缓存
    std::function<void()> _onStockClickCallback;    // 备用堆点击回调
};