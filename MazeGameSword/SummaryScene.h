// SummaryScene.h
#pragma once
#include "Scene.h"
#include <graphics.h>
#include <string>
#include <vector>

class SceneManager;

class SummaryScene : public Scene {
private:
    SceneManager* sceneManager;

    // 游戏数据
    int totalScore;
    int swordFragmentsCollected;
    int stagesCleared;
    int enemiesDefeated;
    int itemsCollected;

    // 时间相关
    int playTimeSeconds;  // 游戏总时长（秒）

    // 评级
    enum class Rank {
        S, A, B, C, D
    };
    Rank playerRank;

    // 动画效果
    int animationTimer;
    int currentStatDisplay;  // 当前显示的统计项目
    bool statsRevealed;      // 所有统计是否已显示完毕
    bool showContinuePrompt; // 是否显示继续提示

    // 按钮
    enum class SummaryOption {
        RESTART_GAME,
        RETURN_TO_MENU
    };
    SummaryOption selectedOption;

public:
    explicit SummaryScene(SceneManager* manager);
    ~SummaryScene() override = default;

    // 实现Scene基类的纯虚函数
    void enter() override;
    void exit() override;
    void update() override;
    void render() override;
    void setSceneManager(SceneManager* manager) override { sceneManager = manager; }

    // 设置游戏数据
    void setGameData(int score, int fragments, int stages, int enemies, int items, int time);

private:
    // 计算评级
    void calculateRank();

    // 绘制方法
    void drawBackground();
    void drawTitle();
    void drawStats();
    void drawRank();
    void drawButtons();
    void drawAnimationEffects();

    // 绘制单个统计项
    void drawStatItem(int index, const std::string& label, const std::string& value,
        int y, bool highlight = false);

    // 绘制单个按钮
    void drawButton(int x, int y, int width, int height,
        const char* text, bool selected);

    // 工具函数
    std::string formatTime(int seconds);
    std::string getRankText(Rank rank);
    COLORREF getRankColor(Rank rank);
};