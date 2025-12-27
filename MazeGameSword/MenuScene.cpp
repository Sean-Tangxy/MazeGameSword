// MenuScene.cpp
#include "MenuScene.h"
#include "SceneManager.h"  // 添加这个头文件
#include <graphics.h>
#include <conio.h>
#include <string>
#include <cstdlib>

MenuScene::MenuScene(SceneManager* manager)
    : sceneManager(manager), selectedOption(MenuOption::START_GAME) {
}

void MenuScene::enter() {
    // 初始化菜单状态
    selectedOption = MenuOption::START_GAME;
}

void MenuScene::exit() {
    // 清理资源（如果有）
}

void MenuScene::update() {
    // 键盘控制
    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        selectedOption = MenuOption::START_GAME;
    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        selectedOption = MenuOption::EXIT_GAME;
    }
    else if (GetAsyncKeyState(VK_RETURN) & 0x8000 ||
        GetAsyncKeyState(' ') & 0x8000) {
        // 确认选择
        if (selectedOption == MenuOption::START_GAME) {
            sceneManager->switchTo(SceneType::STORY);
        }
        else if (selectedOption == MenuOption::EXIT_GAME) {
            closegraph();
            std::exit(0);
        }
    }

    // 鼠标控制
    if (MouseHit()) {
        MOUSEMSG msg = GetMouseMsg();
        if (msg.uMsg == WM_LBUTTONDOWN) {
            if (isMouseOverButton(startButtonY)) {
                sceneManager->switchTo(SceneType::STORY);
            }
            else if (isMouseOverButton(exitButtonY)) {
                closegraph();
                std::exit(0);
            }
        }
    }
}

// render方法保持不变...

void MenuScene::render() {
    // 不需要清屏，双缓冲会自动处理

    // 设置背景色
    setbkcolor(RGB(30, 30, 60));
    cleardevice();  // 这个保留，但只会清后台缓冲区

    // 绘制标题
    settextcolor(RGB(255, 215, 0));  // 金色
    settextstyle(80, 0, _T("楷体"));
    outtextxy(150, 100, _T("勇者斗魔王"));

    // 绘制副标题
    settextcolor(RGB(200, 200, 200));
    settextstyle(24, 0, _T("宋体"));
    outtextxy(300, 200, _T("收集圣剑碎片，击败魔王"));

    // 绘制按钮
    drawButton(startButtonY, "开始新游戏",
        selectedOption == MenuOption::START_GAME);
    drawButton(exitButtonY, "退出游戏",
        selectedOption == MenuOption::EXIT_GAME);

    // 绘制操作提示
    settextcolor(RGB(150, 150, 150));
    settextstyle(18, 0, _T("宋体"));
    outtextxy(300, 500, _T("使用 ↑↓ 选择，回车确认"));
    outtextxy(300, 530, _T("或鼠标点击"));
}

void MenuScene::drawButton(int y, const char* text, bool selected) {
    // 按钮颜色
    COLORREF fillColor, borderColor, textColor;

    if (selected) {
        fillColor = RGB(70, 130, 180);   // 选中时亮蓝色
        borderColor = RGB(255, 255, 255); // 白色边框
        textColor = RGB(255, 255, 255);   // 白色文字
    }
    else {
        fillColor = RGB(50, 100, 150);   // 未选中时深蓝色
        borderColor = RGB(200, 200, 200); // 灰色边框
        textColor = RGB(220, 220, 220);   // 浅灰色文字
    }

    // 绘制按钮背景
    setfillcolor(fillColor);
    solidroundrect(centerX - buttonWidth / 2, y - buttonHeight / 2,
        centerX + buttonWidth / 2, y + buttonHeight / 2,
        10, 10);

    // 绘制按钮边框
    setlinecolor(borderColor);
    setlinestyle(PS_SOLID, 3);
    roundrect(centerX - buttonWidth / 2, y - buttonHeight / 2,
        centerX + buttonWidth / 2, y + buttonHeight / 2,
        10, 10);

    // 绘制按钮文字
    settextcolor(textColor);
    settextstyle(28, 0, _T("宋体"));
    setbkmode(TRANSPARENT);

    int textWidth = textwidth(_T(text));
    int textHeight = textheight(_T(text));
    outtextxy(centerX - textWidth / 2, y - textHeight / 2, _T(text));
}

bool MenuScene::isMouseOverButton(int buttonY) {
    MOUSEMSG msg = GetMouseMsg();
    int mouseX = msg.x;
    int mouseY = msg.y;

    return (mouseX >= centerX - buttonWidth / 2 &&
        mouseX <= centerX + buttonWidth / 2 &&
        mouseY >= buttonY - buttonHeight / 2 &&
        mouseY <= buttonY + buttonHeight / 2);
}