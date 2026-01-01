// SceneManager.h - 更新版
#pragma once
#include <map>
#include <string>
#include "Scene.h"

class SceneManager {
private:
    std::map<SceneType, Scene*> scenes;
    Scene* currentScene;
    SceneType currentSceneType;

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

    // 获取特定场景的指针
    template<typename T>
    T* getScene(SceneType type) {
        auto it = scenes.find(type);
        if (it != scenes.end()) {
            return dynamic_cast<T*>(it->second);
        }
        return nullptr;
    }
};