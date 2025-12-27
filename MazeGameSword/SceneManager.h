// SceneManager.h
#pragma once
#include <map>
#include <string>
#include "Scene.h"  // 包含基类定义

// 场景管理器
class SceneManager {
private:
    std::map<SceneType, Scene*> scenes;  // 存储场景
    Scene* currentScene;                  // 当前场景
    SceneType currentSceneType;           // 当前场景类型

public:
    SceneManager();
    ~SceneManager();

    // 场景管理
    void addScene(SceneType type, Scene* scene);
    void switchTo(SceneType type);

    // 游戏主循环方法
    void update();
    void render();

    // 获取当前场景类型
    SceneType getCurrentSceneType() const { return currentSceneType; }
};