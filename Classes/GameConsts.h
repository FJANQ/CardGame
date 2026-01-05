/**
 * @file GameConsts.h
 * @brief 游戏全局静态配置
 * 定义了游戏中的基础枚举和物理常量
 */
#pragma once

namespace GameConsts {
    // 命名规范：常量以 k 开头
    static const float kCardMoveDuration = 0.3f; // 卡牌移动动画时长
    static const int kDesignWidth = 1080;        // 设计分辨率宽
    static const int kDesignHeight = 2080;       // 设计分辨率高

    // 布局坐标常量
    static const float kPlayfieldY_Top = 1500.0f;
    static const float kStackPosX = 780.0f;
    static const float kStackPosY = 450.0f;
    static const float kStockPosX = 300.0f;
    static const float kStockPosY = 450.0f;
}

// 花色类型
enum class CardSuitType {
    NONE = -1,
    CLUBS = 0,      // 梅花
    DIAMONDS,       // 方块
    HEARTS,         // 红桃
    SPADES,         // 黑桃
    COUNT
};

// 点数类型
enum class CardFaceType {
    NONE = -1,
    ACE = 1,
    TWO, THREE, FOUR, FIVE, SIX, SEVEN,
    EIGHT, NINE, TEN, JACK, QUEEN, KING,
    COUNT
};