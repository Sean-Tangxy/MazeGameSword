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
    addScene(SceneType::BEFORE_BOSS, new BeforeBossScene(this));  // 新增：BOSS前剧情场景
    addScene(SceneType::BOSS, new BossFightScene(this));
    addScene(SceneType::END, new AfterBossScene(this));
    // SummaryScene需要特殊处理，通常从游戏结束后调用

    // 设置初始场景（调试时可以选择不同场景）
    switchTo(SceneType::MENU);                 // 正常流程->菜单开始
    // switchTo(SceneType::BEFORE_BOSS);       // 调试用，直接测试BOSS前剧情
    // switchTo(SceneType::BOSS);              // 调试用，直接测试BOSS战
	//switchTo(SceneType::END);           //调试用，直接测试结局场景
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