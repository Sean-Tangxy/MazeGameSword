// SummaryScene.cpp
#define NOMINMAX// 防止windows.h定义min/max宏
#include "SummaryScene.h"
#include "SceneManager.h"
#include <graphics.h>
#include <conio.h>
#include <string>
#include <sstream>
#include <iomanip>

SummaryScene::SummaryScene(SceneManager* manager)
    : sceneManager(manager),
    totalScore(0),
    swordFragmentsCollected(0),
    stagesCleared(0),
    enemiesDefeated(0),
    itemsCollected(0),
    playTimeSeconds(0),
    playerRank(Rank::C),
    animationTimer(0),
    currentStatDisplay(0),
    statsRevealed(false),
    showContinuePrompt(false),
    selectedOption(SummaryOption::RESTART_GAME) {
}

void SummaryScene::enter() {
    // 重置动画状态
    animationTimer = 0;
    currentStatDisplay = 0;
    statsRevealed = false;
    showContinuePrompt = false;
    selectedOption = SummaryOption::RESTART_GAME;

    // 计算评级
    calculateRank();
}

void SummaryScene::exit() {
    // 清理资源（如果有）
}

void SummaryScene::update() {
    // 更新动画
    animationTimer++;

    // 每60帧（1秒）显示一个统计项目
    if (!statsRevealed && animationTimer > 60) {
        currentStatDisplay++;
        animationTimer = 0;

        if (currentStatDisplay >= 5) { // 5个统计项目
            statsRevealed = true;
            showContinuePrompt = true;
        }
    }

    // 处理输入（只有在所有统计显示完毕后）
    if (statsRevealed) {
        // 键盘控制
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
            selectedOption = SummaryOption::RESTART_GAME;
        }
        else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
            selectedOption = SummaryOption::RETURN_TO_MENU;
        }
        else if (GetAsyncKeyState(VK_RETURN) & 0x8000 ||
            GetAsyncKeyState(' ') & 0x8000) {
            // 确认选择
            if (selectedOption == SummaryOption::RESTART_GAME) {
                sceneManager->switchTo(SceneType::GAME);
            }
            else if (selectedOption == SummaryOption::RETURN_TO_MENU) {
                sceneManager->switchTo(SceneType::MENU);
            }
        }
    }
}

void SummaryScene::render() {
    // 绘制背景
    drawBackground();

    // 绘制标题
    drawTitle();

    // 绘制统计信息
    drawStats();

    // 绘制评级
    drawRank();

    // 绘制按钮（如果统计已显示完毕）
    if (statsRevealed) {
        drawButtons();
    }

    // 绘制动画效果
    drawAnimationEffects();
}

void SummaryScene::setGameData(int score, int fragments, int stages,
    int enemies, int items, int time) {
    totalScore = score;
    swordFragmentsCollected = fragments;
    stagesCleared = stages;
    enemiesDefeated = enemies;
    itemsCollected = items;
    playTimeSeconds = time;
}

// 计算评级
void SummaryScene::calculateRank() {
    int rankScore = 0;

    // 分数评级（最高5000分）
    rankScore += (totalScore * 20) / 5000;

    // 圣剑碎片（3个）
    rankScore += (swordFragmentsCollected * 10);

    // 通关关卡（3关）
    rankScore += (stagesCleared * 15);

    // 击败敌人
    rankScore += std::min(enemiesDefeated * 5, 15);

    // 收集物品
    rankScore += std::min(itemsCollected * 2, 10);

    // 时间评分（越快越好，基准时间1800秒=30分钟）
    if (playTimeSeconds > 0) {
        int timeScore = (1800 * 10) / playTimeSeconds;
        rankScore += std::min(timeScore, 10);
    }

    // 转换为评级
    if (rankScore >= 90) {
        playerRank = Rank::S;
    }
    else if (rankScore >= 75) {
        playerRank = Rank::A;
    }
    else if (rankScore >= 60) {
        playerRank = Rank::B;
    }
    else if (rankScore >= 40) {
        playerRank = Rank::C;
    }
    else {
        playerRank = Rank::D;
    }
}

// 绘制背景
void SummaryScene::drawBackground() {
    // 渐变背景
    for (int i = 0; i < 600; i++) {
        int r = 30 + i * 20 / 600;
        int g = 40 + i * 30 / 600;
        int b = 50 + i * 40 / 600;
        setlinecolor(RGB(r, g, b));
        line(0, i, 800, i);
    }

    // 绘制装饰性图案
    setfillcolor(RGB(255, 215, 0, 30));  // 半透明金色
    for (int i = 0; i < 5; i++) {
        int x = 100 + i * 150;
        int y = 150 + (i % 2) * 100;
        solidcircle(x, y, 50);
    }
}

// 绘制标题
void SummaryScene::drawTitle() {
    // 大标题
    settextcolor(RGB(255, 215, 0));
    settextstyle(64, 0, _T("楷体"));
    setbkmode(TRANSPARENT);
    outtextxy(250, 50, _T("游戏结算"));

    // 副标题
    settextcolor(RGB(200, 200, 255));
    settextstyle(24, 0, _T("宋体"));
    outtextxy(350, 130, _T("Journey Summary"));
}

// 绘制统计信息
void SummaryScene::drawStats() {
    // 统计项目列表
    const char* statLabels[] = {
        "总分",
        "圣剑碎片",
        "通关关卡",
        "击败敌人",
        "收集物品",
        "游戏时间"
    };

    std::string statValues[6];
    statValues[0] = std::to_string(totalScore);
    statValues[1] = std::to_string(swordFragmentsCollected) + "/3";
    statValues[2] = std::to_string(stagesCleared) + "/3";
    statValues[3] = std::to_string(enemiesDefeated);
    statValues[4] = std::to_string(itemsCollected);
    statValues[5] = formatTime(playTimeSeconds);

    // 绘制所有统计项目
    int startY = 200;
    int spacing = 60;

    for (int i = 0; i < 6; i++) {
        bool highlight = (i < currentStatDisplay);
        drawStatItem(i, statLabels[i], statValues[i], startY + i * spacing, highlight);
    }
}

// 绘制单个统计项
void SummaryScene::drawStatItem(int index, const std::string& label,
    const std::string& value, int y, bool highlight) {
    int labelX = 200;
    int valueX = 500;
    int boxWidth = 400;
    int boxHeight = 40;

    // 绘制背景框
    if (highlight) {
        setfillcolor(RGB(50, 50, 80, 200));
        solidrectangle(labelX - 20, y - 10, valueX + 120, y + boxHeight - 10);

        // 绘制边框
        setlinecolor(RGB(218, 165, 32));
        setlinestyle(PS_SOLID, 3);
        rectangle(labelX - 20, y - 10, valueX + 120, y + boxHeight - 10);
    }

    // 绘制标签
    COLORREF labelColor = highlight ? RGB(255, 255, 200) : RGB(150, 150, 150);
    settextcolor(labelColor);
    settextstyle(28, 0, _T("楷体"));

    outtextxy(labelX, y, label.c_str());

    // 绘制数值
    COLORREF valueColor = highlight ? RGB(255, 255, 100) : RGB(200, 200, 200);
    settextcolor(valueColor);
    settextstyle(32, 0, _T("Arial"));

    int valueWidth = textwidth(value.c_str());
    outtextxy(valueX + 100 - valueWidth, y - 2, value.c_str());
}

// 绘制评级
void SummaryScene::drawRank() {
    if (!statsRevealed) return;

    int centerX = 400;
    int rankY = 520;

    // 绘制评级背景
    setfillcolor(RGB(0, 0, 0, 180));
    solidrectangle(centerX - 150, rankY - 20, centerX + 150, rankY + 60);

    // 绘制评级标题
    settextcolor(RGB(200, 200, 255));
    settextstyle(24, 0, _T("宋体"));
    outtextxy(centerX - 40, rankY - 15, _T("最终评级"));

    // 绘制评级字母
    std::string rankText = getRankText(playerRank);
    COLORREF rankColor = getRankColor(playerRank);

    settextcolor(rankColor);
    settextstyle(72, 0, _T("Arial Black"));

    int rankWidth = textwidth(rankText.c_str());
    outtextxy(centerX - rankWidth / 2, rankY + 5, rankText.c_str());

    // 添加光晕效果
    setlinecolor(rankColor);
    setlinestyle(PS_SOLID, 3);

    for (int i = 1; i <= 3; i++) {
        int offset = i * 2;
        rectangle(centerX - 100 - offset, rankY - 20 - offset,
            centerX + 100 + offset, rankY + 60 + offset);
    }
}

// 绘制按钮
void SummaryScene::drawButtons() {
    int buttonWidth = 180;
    int buttonHeight = 50;
    int buttonY = 580;
    int buttonSpacing = 40;

    int leftButtonX = 200;
    int rightButtonX = 800 - 200 - buttonWidth;

    // 绘制重新开始按钮
    bool leftSelected = (selectedOption == SummaryOption::RESTART_GAME);
    drawButton(leftButtonX, buttonY, buttonWidth, buttonHeight,
        "重新开始", leftSelected);

    // 绘制返回菜单按钮
    bool rightSelected = (selectedOption == SummaryOption::RETURN_TO_MENU);
    drawButton(rightButtonX, buttonY, buttonWidth, buttonHeight,
        "返回菜单", rightSelected);

    // 绘制选择提示
    if (showContinuePrompt) {
        settextcolor(RGB(200, 200, 200));
        settextstyle(18, 0, _T("宋体"));
        outtextxy(320, 540, _T("使用←→选择，回车确认"));
    }
}

// 绘制单个按钮
void SummaryScene::drawButton(int x, int y, int width, int height,
    const char* text, bool selected) {
    // 按钮颜色
    COLORREF fillColor, borderColor, textColor;

    if (selected) {
        fillColor = RGB(70, 130, 180);
        borderColor = RGB(255, 255, 255);
        textColor = RGB(255, 255, 255);
    }
    else {
        fillColor = RGB(50, 100, 150);
        borderColor = RGB(200, 200, 200);
        textColor = RGB(220, 220, 220);
    }

    // 绘制按钮背景
    setfillcolor(fillColor);
    solidroundrect(x, y, x + width, y + height, 10, 10);

    // 绘制按钮边框
    setlinecolor(borderColor);
    setlinestyle(PS_SOLID, 3);
    roundrect(x, y, x + width, y + height, 10, 10);

    // 绘制按钮文字
    settextcolor(textColor);
    settextstyle(24, 0, _T("宋体"));
    setbkmode(TRANSPARENT);

    int textWidth = textwidth(_T(text));
    int textHeight = textheight(_T(text));
    outtextxy(x + (width - textWidth) / 2,
        y + (height - textHeight) / 2,
        _T(text));
}

// 绘制动画效果
void SummaryScene::drawAnimationEffects() {
    // 绘制当前正在显示的统计项目的动画
    if (currentStatDisplay > 0 && !statsRevealed) {
        int y = 200 + (currentStatDisplay - 1) * 60;

        // 绘制闪光效果
        int flashAlpha = (animationTimer * 255) / 60;
        if (flashAlpha > 255) flashAlpha = 255;

        setfillcolor(RGB(255, 255, 255, flashAlpha));
        solidrectangle(180, y - 5, 620, y + 35);
    }

    // 绘制评级出现动画
    if (statsRevealed && animationTimer < 60) {
        float scale = animationTimer / 60.0f;
        int centerX = 400;
        int rankY = 520;

        // 绘制缩放效果
        setlinecolor(RGB(255, 255, 255, 100));
        setlinestyle(PS_SOLID, 2);

        int rectWidth = static_cast<int>(300 * scale);
        int rectHeight = static_cast<int>(80 * scale);

        rectangle(centerX - rectWidth / 2, rankY - rectHeight / 2 + 20,
            centerX + rectWidth / 2, rankY + rectHeight / 2 + 20);
    }
}

// 工具函数：格式化时间
std::string SummaryScene::formatTime(int seconds) {
    int minutes = seconds / 60;
    int secs = seconds % 60;

    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << secs;
    return ss.str();
}

// 工具函数：获取评级文本
std::string SummaryScene::getRankText(Rank rank) {
    switch (rank) {
    case Rank::S: return "S";
    case Rank::A: return "A";
    case Rank::B: return "B";
    case Rank::C: return "C";
    case Rank::D: return "D";
    default: return "?";
    }
}

// 工具函数：获取评级颜色
COLORREF SummaryScene::getRankColor(Rank rank) {
    switch (rank) {
    case Rank::S: return RGB(255, 215, 0);   // 金色
    case Rank::A: return RGB(255, 50, 50);   // 红色
    case Rank::B: return RGB(50, 150, 255);  // 蓝色
    case Rank::C: return RGB(50, 255, 50);   // 绿色
    case Rank::D: return RGB(150, 150, 150); // 灰色
    default: return RGB(255, 255, 255);
    }
}