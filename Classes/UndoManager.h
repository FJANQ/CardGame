/**
 * @file UndoManager.h
 * @brief 回退功能管理器
 * @details 维护一个操作历史栈，用于实现“悔棋”功能。
 */
#pragma once
#include "CardModel.h"
#include <stack>
#include <memory>

 // 回退操作类型
enum class UndoActionType {
    PLAYFIELD_TO_STACK, // 从桌面消除了一张牌
    STOCK_TO_STACK      // 从备用堆翻了一张牌
};

/**
 * @brief 单步回退数据结构
 */
struct UndoStep {
    UndoActionType type;
    std::shared_ptr<CardModel> cardMoved;   // 移动的牌（当前显示的）
    std::shared_ptr<CardModel> oldStackTop; // 被覆盖的旧牌
    cocos2d::Vec2 originalPos;              // 移动牌的原始位置
};

class UndoManager {
public:
    UndoManager() {}

    /**
     * @brief 记录一步操作
     */
    void pushStep(const UndoStep& step) {
        _history.push(step);
    }

    bool hasUndo() const { return !_history.empty(); }

    /**
     * @brief 取出最近一步操作
     */
    UndoStep popStep() {
        if (_history.empty()) return {};
        UndoStep step = _history.top();
        _history.pop();
        return step;
    }

private:
    std::stack<UndoStep> _history;
};