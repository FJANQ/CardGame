/**
 * @file CardView.h
 * @brief 单张卡牌的视图组件
 * @details 负责卡牌的视觉拼装、触摸事件处理。
 *          不包含游戏业务逻辑，仅处理显示和输入反馈。
 */
#pragma once
#include "cocos2d.h"
#include "CardModel.h"
#include <functional>
#include <string>

class CardView : public cocos2d::Node {
public:
    /**
     * @brief 创建交互式卡牌实例
     * @param model 卡牌数据模型
     * @return CardView* 自动管理的实例
     */
    static CardView* create(std::shared_ptr<CardModel> model);

    /**
     * @brief 静态工厂：创建卡牌的纯视觉节点（拼图）
     * @details 用于生成不带逻辑的卡牌外观，如备用牌堆的静态图
     * @param model 卡牌数据
     * @return cocos2d::Node* 拼装好的节点
     */
    static cocos2d::Node* createVisualNode(std::shared_ptr<CardModel> model);

    // 初始化
    virtual bool init(std::shared_ptr<CardModel> model);

    // Getters
    int getCardId() const { return _model ? _model->getId() : -1; }

    /**
     * @brief 设置交互状态（遮挡处理）
     * @param enabled true=正常显示/可点, false=变暗/不可点
     */
    void setEnabled(bool enabled);

    /**
     * @brief 强制触摸开关
     * @param enabled false时会完全忽略触摸事件（穿透）
     */
    void setTouchEnabled(bool enabled) { _isTouchEnabled = enabled; }

    // 设置点击回调
    void setClickCallback(std::function<void(int)> callback) { _clickCallback = callback; }

private:
    /**
     * @brief 初始化触摸监听器
     */
    void initTouchListener();

    // 成员变量命名规范：_开头
    std::shared_ptr<CardModel> _model;
    bool _isTouchEnabled = true;
    std::function<void(int)> _clickCallback;
};