/**
 * @file GameModel.h
 * @brief 游戏运行时全局数据
 * @details 管理桌面上所有卡牌的集合、手牌堆、备用牌堆的状态。
 *          负责数据的增删改查。
 */
#pragma once
#include "CardModel.h"
#include <vector>
#include <memory>
#include <algorithm> // for std::remove_if

class GameModel {
public:
    using CardPtr = std::shared_ptr<CardModel>;

    // --- 主牌区 (Playfield) 操作 ---

    void addPlayfieldCard(CardPtr card) {
        _playfieldCards.push_back(card);
    }

    /**
     * @brief 从桌面移除指定卡牌
     * @param card 要移除的卡牌指针
     */
    void removePlayfieldCard(CardPtr card) {
        _playfieldCards.erase(
            std::remove(_playfieldCards.begin(), _playfieldCards.end(), card),
            _playfieldCards.end()
        );
    }

    const std::vector<CardPtr>& getPlayfieldCards() const { return _playfieldCards; }

    /**
     * @brief 根据ID查找卡牌 (用于点击检测)
     */
    CardPtr getCardById(int id) {
        for (auto& card : _playfieldCards) {
            if (card->getId() == id) return card;
        }
        // 如果需要，也可以查一下 StackTop，但这通常由 Controller 处理
        return nullptr;
    }

    // --- 手牌区 (Stack / 底牌) 操作 ---

    CardPtr getStackTopCard() const { return _stackTopCard; }
    void setStackTopCard(CardPtr card) { _stackTopCard = card; }

    // --- 备用牌堆 (Stock) 操作 ---

    void addStockCard(CardPtr card) { _stockCards.push_back(card); }

    CardPtr popStockCard() {
        if (_stockCards.empty()) return nullptr;
        CardPtr card = _stockCards.back();
        _stockCards.pop_back();
        return card;
    }

    const std::vector<CardPtr>& getStockCards() const { return _stockCards; }

    bool hasStockCards() const { return !_stockCards.empty(); }
    int getStockCount() const { return (int)_stockCards.size(); }

private:
    std::vector<CardPtr> _playfieldCards; // 桌面待消除的牌
    std::vector<CardPtr> _stockCards;     // 左下角的备用牌
    CardPtr _stackTopCard;                // 右下角的当前底牌
};