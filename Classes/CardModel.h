/**
 * @file CardModel.h
 * @brief 卡牌基础数据模型
 * @details 纯数据类，存储卡牌的ID、点数、花色、位置以及逻辑状态（是否被锁）。
 *          不包含任何视图逻辑。
 */
#pragma once
#include "cocos2d.h"
#include "GameConsts.h"

class CardModel {
public:
    /**
     * @brief 构造函数
     * @param id 唯一标识符
     * @param face 点数 (A, 2...K)
     * @param suit 花色 (梅花, 方块...)
     */
    CardModel(int id, CardFaceType face, CardSuitType suit)
        : _id(id), _face(face), _suit(suit), _isBlocked(false), _zIndex(0) {
    }

    // Getters (访问器)
    int getId() const { return _id; }
    CardFaceType getFace() const { return _face; }
    CardSuitType getSuit() const { return _suit; }

    // 位置与层级
    cocos2d::Vec2 getPosition() const { return _position; }
    int getZIndex() const { return _zIndex; }

    // 逻辑状态
    bool isBlocked() const { return _isBlocked; }

    // Setters (修改器)
    void setPosition(const cocos2d::Vec2& pos) { _position = pos; }
    void setZIndex(int z) { _zIndex = z; }
    void setBlocked(bool val) { _isBlocked = val; }

private:
    int _id;                // 唯一ID
    CardFaceType _face;     // 点数
    CardSuitType _suit;     // 花色
    bool _isBlocked;        // 是否被遮挡（死局检测关键）

    cocos2d::Vec2 _position;// 逻辑坐标
    int _zIndex;            // 渲染层级
};