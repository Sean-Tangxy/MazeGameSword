// SceneManager.cpp - 更新版
#include "SceneManager.h"
#include "MenuScene.h"
#include "StoryScene.h"
#include "GameScene.h"
#include "BeforeBossScene.h"
#include "BossFightScene.h"
#include "AfterBossScene.h"
#include "SummaryScene.h"

SceneManager::SceneManager() : currentScene(nullptr), currentSceneType(SceneType::MENU) {
    // 注册所有场景
    addScene(SceneType::MENU, new MenuScene(this));
    addScene(SceneType::STORY, new StoryScene(this));
    addScene(SceneType::GAME, new GameScene(this));
    addScene(SceneType::BOSS, new BossFightScene(this));           // 新增
    addScene(SceneType::END, new AfterBossScene(this));            // 新增，使用END作为AfterBoss
    // 注意：SummaryScene需要特殊处理，通常从游戏结束后调用

    // 设置初始场景
    switchTo(SceneType::MENU);
}

SceneManager::~SceneManager() {
    // 清理所有场景
    for (auto& pair : scenes) {
        delete pair.second;
    }
    scenes.clear();
}

void SceneManager::addScene(SceneType type, Scene* scene) {
    scenes[type] = scene;
}

void SceneManager::switchTo(SceneType type) {
    if (scenes.find(type) != scenes.end()) {
        if (currentScene) {
            currentScene->exit();
        }
        currentScene = scenes[type];
        currentSceneType = type;
        currentScene->enter();
    }
}

void SceneManager::update() {
    if (currentScene) {
        currentScene->update();
    }
}

void SceneManager::render() {
    if (currentScene) {
        currentScene->render();
    }
}

// 新增：获取特定场景的指针（用于数据传递）
/*template<typename T>
T* SceneManager::getScene(SceneType type) {
    auto it = scenes.find(type);
    if (it != scenes.end()) {
        return dynamic_cast<T*>(it->second);
    }
    return nullptr;
}*/
//已经转移到头文件中了