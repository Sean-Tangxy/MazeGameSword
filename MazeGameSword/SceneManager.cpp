// SceneManager.cpp
#include "SceneManager.h"
#include "MenuScene.h"
#include "StoryScene.h"
#include "GameScene.h"

SceneManager::SceneManager() : currentScene(nullptr), currentSceneType(SceneType::MENU) {
    // 创建并添加场景
    addScene(SceneType::MENU, new MenuScene(this));
    addScene(SceneType::STORY, new StoryScene(this));
    addScene(SceneType::GAME, new GameScene(this));  // 添加游戏场景

    // 默认从菜单开始
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