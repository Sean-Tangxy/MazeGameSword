// Scene.h - 更新版
#pragma once
#include <string>

// 场景类型枚举
enum class SceneType {
    MENU,       // 主菜单
    STORY,      // 剧情
    GAME,       // 游戏关卡
    BOSS,       // BOSS战（新增）
    END,        // 结局（更新）
    DEATH,      // 死亡场景
    SUMMARY     // 结算界面（新增，可选）
};

// 前向声明 SceneManager
class SceneManager;

// 场景基类
class Scene {
public:
    virtual ~Scene() = default;

    // 进入场景
    virtual void enter() = 0;

    // 退出场景
    virtual void exit() = 0;

    // 更新场景逻辑
    virtual void update() = 0;

    // 渲染场景
    virtual void render() = 0;

    // 设置场景管理器
    virtual void setSceneManager(SceneManager* manager) = 0;
};