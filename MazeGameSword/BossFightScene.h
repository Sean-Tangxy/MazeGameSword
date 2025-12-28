// BossFightScene.h
#pragma once
#include"CommonTypes.h"
#include "Scene.h"
#include <vector>
#include <string>
#include <map>
#include <graphics.h>

class SceneManager;
class Player;
class Boss;
class BossBullet;

// Boss子弹类型
enum class BossBulletType {
    BONE_SWORD_HORIZONTAL,  // 横向骨剑
    BONE_SWORD_VERTICAL,    // 纵向骨剑
    FIREBALL,              // 火球
    LASER                  // 激光
};

// Boss攻击类型
enum class BossAttackType {
    ATTACK1,  // 横向骨剑
    ATTACK2,  // 纵向骨剑
    ATTACK3,  // 火球
    ATTACK4   // 激光
};

// Boss阶段
enum class BossPhase {
    PHASE_1,  // 第一阶段
    PHASE_2,  // 第二阶段
    PHASE_3   // 第三阶段
};

/*// 位置结构
struct Position {
    int x, y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

// 游戏消息结构
struct GameMessage {
    std::string text;
    int duration;
    int timer;
    COLORREF color;
};*/

//前述部分结构体在CommonTypes.h中已定义

class BossFightScene : public Scene {
private:
    SceneManager* sceneManager;

    // 地图尺寸
    static const int MAP_WIDTH = 32;     // 32列
    static const int MAP_HEIGHT = 18;    // 18行
    static const int TILE_SIZE = 40;     // 每个格子40像素

    // 游戏对象
    Player* player;
    Boss* boss;

    // 子弹管理
    std::vector<BossBullet*> bossBullets;
    std::vector<BossBullet*> playerBullets;

    // Boss相关
    BossPhase currentPhase;
    int phaseTransitionTimer;
    bool inPhaseTransition;
    std::vector<std::string> phaseTransitionDialogue;
    size_t currentDialogueStep;
    bool inDialogue;

    // 攻击计时
    int attackCooldown;
    int attackTimer;

    // 玩家状态
    int playerHealth;
    int playerMaxHealth;
    bool hasHolySword;      // 是否获得圣剑
    int holySwordCooldown;  // 圣剑攻击冷却

    // Boss状态
    int bossHealth;
    int bossMaxHealth;

    // 游戏状态
    bool isBossDefeated;
    bool isGameOver;

    // 资源（图片）
    IMAGE playerImg;
    IMAGE bossImg;
    IMAGE boneSwordImg;
    IMAGE fireballImg;
    IMAGE laserImg;
    IMAGE holySwordBulletImg;

    // 消息系统
    std::vector<GameMessage> messages;

public:
    explicit BossFightScene(SceneManager* manager);
    ~BossFightScene() override;

    // Scene 接口实现
    void enter() override;
    void exit() override;
    void update() override;
    void render() override;
    void setSceneManager(SceneManager* manager) override { sceneManager = manager; }

    // 工具函数
    bool isPositionWalkable(const Position& pos) const;
    bool isPositionInPlayerArea(const Position& pos) const;
    bool isPositionInBossArea(const Position& pos) const;

    // 获取地图尺寸
    int getMapWidth() const { return MAP_WIDTH; }
    int getMapHeight() const { return MAP_HEIGHT; }
    int getTileSize() const { return TILE_SIZE; }

private:
    // 初始化
    void initPhaseTransitionDialogues();

    // 碰撞检测
    void checkBulletCollisions();
    void checkPlayerCollisionWithBossBullets();
    void checkBossCollisionWithPlayerBullets();

    // 攻击生成
    void generateAttack(BossAttackType attackType);
    void generateAttack1();  // 横向骨剑
    void generateAttack2();  // 纵向骨剑
    void generateAttack3();  // 火球
    void generateAttack4();  // 激光

    // 子弹管理
    void updateBullets();
    void renderBullets();
    void clearAllBullets();

    // 阶段转换
    void checkPhaseTransition();
    void startPhaseTransition();
    void updatePhaseTransition();
    void endPhaseTransition();

    // 消息系统
    void showMessage(const std::string& text, int duration = 180, COLORREF color = WHITE);
    void updateMessages();
    void renderMessages();

    // UI 绘制
    void drawUI();
    void drawHealthBar(int x, int y, int width, int height, int current, int max, COLORREF color);

    // 工具函数
    bool isPositionValid(const Position& pos) const;
    Position getRandomPositionNearPlayer(int radius);
};