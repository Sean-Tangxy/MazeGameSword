// MenuScene.h
#pragma once
#include "Scene.h"

class SceneManager;

class MenuScene : public Scene {
private:
    SceneManager* sceneManager;

    // 菜单选项
    enum class MenuOption {
        START_GAME,
        EXIT_GAME
    };

    MenuOption selectedOption;

    // 按钮尺寸
    const int buttonWidth = 200;
    const int buttonHeight = 50;
    const int startButtonY = 300;
    const int exitButtonY = 400;
    const int centerX = 400;

public:
    MenuScene(SceneManager* manager);
    ~MenuScene() override = default;

    void enter() override;
    void exit() override;
    void update() override;
    void render() override;
    void setSceneManager(SceneManager* manager) override { sceneManager = manager; }

private:
    // 绘制按钮
    void drawButton(int y, const char* text, bool selected);

    // 检查鼠标是否在按钮上
    bool isMouseOverButton(int buttonY);
};