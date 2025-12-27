// GameScene.h
#pragma once
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
    FOREST_GUARDIAN  // 森林守护者
};

// NPC 对话结构
struct NPCDialogue {
    std::string speaker;
    std::string content;
};

// 地图坐标结构
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

class GameScene : public Scene {
private:

    SceneManager* sceneManager;

    // 地图相关
    static const int GAME_MAP_WIDTH = 20;     // 20列
    static const int GAME_MAP_HEIGHT = 15;    // 15行
    static const int GAME_TILE_SIZE = 40;     // 每个格子40像素

    TileType map[GAME_MAP_HEIGHT][GAME_MAP_WIDTH];
    std::vector<Position> wallPositions;

    // 物品相关
    std::vector<std::pair<Position, ItemType>> items;
    bool hasKey;
    bool swordFragments[3];  // 三个圣剑碎片

    // 游戏对象
    Player* player;
    std::vector<Enemy*> enemies;

    // NPC 相关
    Position npcPosition;
    NPCType npcType;
    bool inDialogue;
    std::vector<NPCDialogue> currentDialogue;
    size_t currentDialogueStep;

    // UI 状态
    int playerHealth;
    int playerMaxHealth;
    int score;

    // 游戏状态
    bool isGameOver;
    bool isGameWon;

    // 资源（图片）
    IMAGE tileWall;
    IMAGE tileFloor;
    IMAGE playerImg;
    IMAGE enemyImg;
    IMAGE healthPotionImg;
    IMAGE coinImg;
    IMAGE keyImg;
    IMAGE swordFragmentImg;
    IMAGE doorImg;
    IMAGE exitImg;
    IMAGE npcImg;

    // 加载状态
    bool resourcesLoaded;

public:
    explicit GameScene(SceneManager* manager);
    ~GameScene() override;

    // Scene 接口实现
    void enter() override;
    void exit() override;
    void update() override;
    void render() override;
    void setSceneManager(SceneManager* manager) override { sceneManager = manager; }

    // 工具函数，供 Enemy 类使用
    bool isPositionWalkable(const Position& pos) const;

    // 获取地图尺寸的公共方法
    int getMapWidth() const { return GAME_MAP_WIDTH; }
    int getMapHeight() const { return GAME_MAP_HEIGHT; }
    int getTileSize() const { return GAME_TILE_SIZE; }

private:
    // 地图生成
    void generateFirstLevelMap();
    void drawMap();

    // 调整地图大小以适应窗口
    static const int MAP_WIDTH = 20;    // 改为 20 列
    static const int MAP_HEIGHT = 15;   // 改为 15 行
    static const int TILE_SIZE = 40;    // 改为 40 像素，这样 20×40=800, 15×40=600

    // 物品管理
    void generateItems();
    void checkItemCollision();
    void removeItem(const Position& pos);

    // NPC 对话
    void initForestGuardianDialogue();
    void startDialogue();
    void updateDialogue();
    void renderDialogue();
    void endDialogue();

    // 游戏逻辑
    void checkGameStatus();
    void updateEnemies();
    void checkEnemyCollision();

    // 资源管理
    void loadResources();
    void unloadResources();
    void createDefaultResources();

    // 工具函数
    bool isPositionValid(const Position& pos) const;
   
    bool isPositionOccupiedByEnemy(const Position& pos) const;

    // UI 绘制
    void drawUI();
    void drawHealthBar(int x, int y, int width, int height, int current, int max);
    void drawInventory();

    // 对话工具
    void drawTextWithWrap(int x, int y, int width, const std::string& text,
        COLORREF color = WHITE, int fontSize = 20);
    void drawDialogueBox(int x, int y, int width, int height);
};