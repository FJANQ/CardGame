/**
 * @file GameModelFromLevelGenerator.h
 * @brief 关卡数据生成服务
 * @details 负责将静态配置转换为动态的 GameModel 对象。
 *          包含卡牌的布局策略和生成逻辑。
 */
#pragma once
#include "GameModel.h"
#include "GameConsts.h"
#include "cocos2d.h"
#include <vector>

 // 内部辅助结构：卡牌配置
struct LevelCardConfig {
    int face;
    int suit;
    float x;
    float y;
};

class GameModelFromLevelGenerator {
public:
    /**
     * @brief 生成指定关卡的游戏数据
     */
    static std::shared_ptr<GameModel> generate(int levelId) {
        auto gameModel = std::make_shared<GameModel>();

        // 1. 生成主牌区 (金字塔布局)
        generatePlayfield(gameModel);

        // 2. 生成初始底牌 (一张4)
        generateInitialStack(gameModel);

        // 3. 生成备用牌堆 (5张)
        generateStockPile(gameModel);

        return gameModel;
    }

private:
    // 生成主牌区
    static void generatePlayfield(std::shared_ptr<GameModel> model) {
        // 使用优化后的三段式布局坐标 (Y 轴整体上移)
        std::vector<LevelCardConfig> layout = {
            // 第一层 (顶层)
            {12, 0, 250, 1500},
            {2,  0, 850, 1500},

            // 第二层
            {2,  0, 300, 1380},
            {2,  0, 800, 1380},

            // 第三层 (底层)
            {2,  1, 350, 1260},
            {1,  3, 750, 1260}
        };

        int idCounter = 1000;
        for (const auto& cfg : layout) {
            auto card = std::make_shared<CardModel>(
                idCounter++,
                (CardFaceType)cfg.face,
                (CardSuitType)cfg.suit
            );
            card->setPosition(cocos2d::Vec2(cfg.x, cfg.y));
            card->setZIndex(10);
            model->addPlayfieldCard(card);
        }
    }

    // 生成初始底牌
    static void generateInitialStack(std::shared_ptr<GameModel> model) {
        // 初始底牌设为 4 (方块)，配合桌面的 2 和 Q 测试逻辑
        auto stackCard = std::make_shared<CardModel>(
            2001, CardFaceType::THREE, CardSuitType::DIAMONDS
        );
        // 位置由 Controller 统一设置，这里只存数据
        model->setStackTopCard(stackCard);
    }

    // 生成备用牌堆
    static void generateStockPile(std::shared_ptr<GameModel> model) {
        // 生成 5 张随机备用牌
        for (int i = 0; i < 5; i++) {
            // 这里简单生成一些红桃牌
            auto stockCard = std::make_shared<CardModel>(
                3000 + i,
                (CardFaceType)(4 + i), // 5, 6, 7...
                CardSuitType::HEARTS
            );
            model->addStockCard(stockCard);
        }
    }
};