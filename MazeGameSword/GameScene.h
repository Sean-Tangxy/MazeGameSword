// GameScene.h
#pragma once
#include "CommonTypes.h"
#include "Scene.h"
#include <vector>
#include <string>
#include <map>
#include <graphics.h>

class SceneManager;
class Player;
class Enemy;

// 地图格子类型
enum class TileType {
    EMPTY,      // 空地（可通行）
    WALL,       // 墙（不可通行）
    DOOR,       // 门（需要钥匙）
    EXIT,       // 出口（需要钥匙）
    PLAYER_SPAWN, // 玩家出生点
    ENEMY_SPAWN,  // 敌人生成点
    NPC_SPAWN     // NPC生成点
};

// 物品类型
enum class ItemType {
    HEALTH_POTION,  // 药水
    COIN,           // 金币
    KEY,            // 钥匙
    SWORD_FRAGMENT  // 圣剑碎片
};

// NPC 类型
enum class NPCType {
    FOREST_GUARDIAN,  // 森林守护者
    SKY_GUARDIAN,     // 天空守护者
    ROCK_GUARDIAN     // 熔岩守护者
};

// NPC 对话结构
struct NPCDialogue {
    std::string speaker;
    std::string content;
};

// 玩家数据（需要跨关卡保存）
struct PlayerData {
    Position position;
    int health;
    int maxHealth;
    int score;
    bool hasKey;
    bool swordFragments[3];

    PlayerData() : position(1, 1), health(6), maxHealth(6), score(0), hasKey(false) {
        for (int i = 0; i < 3; i++) {
            swordFragments[i] = false;
        }
    }
};

class GameScene : public Scene {
private:
    SceneManager* sceneManager;

    // 游戏阶段
    int stage;

    // 地图相关
    static const int MAP_WIDTH = 20;     // 20列
    static const int MAP_HEIGHT = 15;    // 15行
    static const int TILE_SIZE = 40;     // 每个格子40像素

    TileType map[MAP_HEIGHT][MAP_WIDTH];
    std::vector<Position> wallPositions;

    // 不同关卡的墙砖图片
    IMAGE tileWallStage1;  // 第一关墙砖
    IMAGE tileWallStage2;  // 第二关墙砖
    IMAGE tileWallStage3;  // 第三关墙砖
    IMAGE* currentWallTile; // 当前使用的墙砖指针

    // 物品相关
    std::vector<std::pair<Position, ItemType>> items;

    // 游戏对象
    Player* player;
    std::vector<Enemy*> enemies;

    // NPC 相关
    Position npcPosition;
    NPCType npcType;
    bool inDialogue;
    std::vector<NPCDialogue> currentDialogue;
    size_t currentDialogueStep;

    // UI 状态（从 PlayerData 获取，用于显示）
    int playerHealth;
    int playerMaxHealth;
    int score;
    bool hasKey;
    bool swordFragments[3];

    // 游戏状态
    bool isGameOver;
    bool isGameWon;

    // 资源（图片）
    IMAGE healthPotionImg;
    IMAGE coinImg;
    IMAGE keyImg;
    IMAGE swordFragmentImg;
    IMAGE npcImg;

    // 加载状态
    bool resourcesLoaded;

    // 消息系统
    std::vector<GameMessage> messages;

    // 玩家数据（跨关卡保存）
    PlayerData playerData;

public:
    explicit GameScene(SceneManager* manager);
    ~GameScene() override;

    // Scene 接口实现
    void enter() override;
    void exit() override;
    void update() override;
    void render() override;
    void setSceneManager(SceneManager* manager) override { sceneManager = manager; }
    void initNextStage();

    // 重置整个游戏
    void resetGame();

    // 工具函数，供 Enemy 类使用
    bool isPositionWalkable(const Position& pos) const;

    // 获取地图尺寸的公共方法
    int getMapWidth() const { return MAP_WIDTH; }
    int getMapHeight() const { return MAP_HEIGHT; }
    int getTileSize() const { return TILE_SIZE; }

    // 获取玩家数据
    const PlayerData& getPlayerData() const { return playerData; }
    void setPlayerData(const PlayerData& data) { playerData = data; updatePlayerUI(); }

private:
    // 内存管理
    void clearGameObjects();

    // 更新UI显示数据
    void updatePlayerUI();

    // 重置关卡状态（不重置玩家数据）
    void resetStageState();

    // 地图生成
    void generateFirstLevelMap();
    void generateSecondLevelMap();
    void generateThirdLevelMap();
    void drawMap();

    // 物品管理
    void generateFirstStageItems();
    void generateSecondStageItems();
    void generateThirdStageItems();
    void checkItemCollision();
    void removeItem(const Position& pos);

    // NPC 对话
    void initForestGuardianDialogue();
    void initSkyGuardianDialogue();
    void initRockGuardianDialogue();
    void startDialogue();
    void updateDialogue();
    void renderDialogue();
    void endDialogue();

    // NPC 绘制辅助
    void drawDefaultNPC(int screenX, int screenY);
    bool isPlayerNearNPC() const;
    void drawInteractionPrompt(int npcScreenX, int npcScreenY);
    Position findValidNPCPosition();

    // 游戏逻辑
    void checkGameStatus();
    void updateEnemies();
    void checkEnemyCollision();

    // 资源管理
    void loadResources();
    void unloadResources();
    void createDefaultResources();
    bool fileExists(const TCHAR* filename);

    // 工具函数
    bool isPositionValid(const Position& pos) const;
    bool isPositionOccupiedByEnemy(const Position& pos) const;

    // 消息系统
    void showMessage(const std::string& text, int duration = 180, COLORREF color = WHITE);
    void updateMessages();
    void renderMessages();

    // UI 绘制
    void drawUI();
    void drawHealthBar(int x, int y, int width, int height, int current, int max);

    // 对话工具
    void drawTextWithWrap(int x, int y, int width, const std::string& text,
        COLORREF color = WHITE, int fontSize = 20);
    void drawDialogueBox(int x, int y, int width, int height);
};