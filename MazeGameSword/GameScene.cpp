// GameScene.cpp
#include"CommonTypes.h"
#include "GameScene.h"
#include "SceneManager.h"
#include <graphics.h>
#include <conio.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>

// ========== Player 类 ==========
class Player {
private:
    Position position;
    IMAGE* sprite;

public:
    Player() : sprite(nullptr) {}
    Player(Position startPos) : position(startPos), sprite(nullptr) {}

    Position getPosition() const { return position; }
    void setPosition(const Position& pos) { position = pos; }

    void setSprite(IMAGE* img) { sprite = img; }

    void move(int dx, int dy) {
        position.x += dx;
        position.y += dy;
    }

    void render(int tileSize) const {
        if (sprite) {
            putimage(position.x * tileSize, position.y * tileSize, sprite);
        }
        else {
            // 绘制默认玩家方块
            setfillcolor(RGB(0, 128, 255));  // 蓝色
            solidrectangle(position.x * tileSize,
                position.y * tileSize,
                (position.x + 1) * tileSize - 2,
                (position.y + 1) * tileSize - 2);
        }
    }
};

// ========== Enemy 类 ==========
class Enemy {
private:
    Position position;
    IMAGE* sprite;
    int justAttackTimer;
    int moveCooldown;
    static const int MOVE_DELAY = 30;  // 30帧 = 0.5秒（假设60FPS）

public:
    Enemy(Position startPos) : position(startPos), sprite(nullptr),
        justAttackTimer(0), moveCooldown(0) {
    }

    // 访问器/修改器
    int getJustAttackTimer() const { return justAttackTimer; }
    void decreaseJustAttackTimer() { if (justAttackTimer > 0) --justAttackTimer; }
    void setJustAttackTimer(int v) { justAttackTimer = v; }

    Position getPosition() const { return position; }

    void setSprite(IMAGE* img) { sprite = img; }

    // 简单AI：朝玩家移动
    void update(const Position& playerPos, const GameScene& scene) {
        // 减少攻击冷却
        if (justAttackTimer > 0) {
            --justAttackTimer;
        }

        // 减少移动冷却
        if (moveCooldown > 0) {
            --moveCooldown;
            return;  // 冷却期间不移动
        }

        // 重置移动冷却
        moveCooldown = MOVE_DELAY;
        int dx = 0, dy = 0;

        // 简单追踪算法
        if (position.x < playerPos.x &&
            scene.isPositionWalkable({ position.x + 1, position.y })) {
            dx = 1;
        }
        else if (position.x > playerPos.x &&
            scene.isPositionWalkable({ position.x - 1, position.y })) {
            dx = -1;
        }
        else if (position.y < playerPos.y &&
            scene.isPositionWalkable({ position.x, position.y + 1 })) {
            dy = 1;
        }
        else if (position.y > playerPos.y &&
            scene.isPositionWalkable({ position.x, position.y - 1 })) {
            dy = -1;
        }

        // 随机移动（如果追踪失败）
        if (dx == 0 && dy == 0) {
            int dir = rand() % 4;
            switch (dir) {
            case 0: if (scene.isPositionWalkable({ position.x + 1, position.y })) dx = 1; break;
            case 1: if (scene.isPositionWalkable({ position.x - 1, position.y })) dx = -1; break;
            case 2: if (scene.isPositionWalkable({ position.x, position.y + 1 })) dy = 1; break;
            case 3: if (scene.isPositionWalkable({ position.x, position.y - 1 })) dy = -1; break;
            }
        }

        // 更新位置
        position.x += dx;
        position.y += dy;
    }

    void render(int tileSize) const {
        if (sprite) {
            putimage(position.x * tileSize, position.y * tileSize, sprite);
        }
        else {
            // 绘制默认敌人方块
            setfillcolor(RGB(255, 50, 50));  // 红色
            solidrectangle(position.x * tileSize,
                position.y * tileSize,
                (position.x + 1) * tileSize - 2,
                (position.y + 1) * tileSize - 2);
        }
    }
};

// ========== GameScene 实现 ==========

GameScene::GameScene(SceneManager* manager)
    : sceneManager(manager),
    stage(1),
    player(nullptr),
    hasKey(false),
    playerHealth(6),
    playerMaxHealth(6),
    score(0),
    inDialogue(false),
    currentDialogueStep(0),
    isGameOver(false),
    isGameWon(false),
    resourcesLoaded(false),
    npcType(NPCType::FOREST_GUARDIAN) {

    // 初始化圣剑碎片
    for (int i = 0; i < 3; i++) {
        swordFragments[i] = false;
    }

    // 初始化地图
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            map[y][x] = TileType::EMPTY;
        }
    }

    srand(static_cast<unsigned>(time(nullptr)));
}

GameScene::~GameScene() {
    exit();
}

void GameScene::enter() {
    // 重置游戏状态
    hasKey = false;
    playerHealth = 6;
    playerMaxHealth = 6;
    score = 0;
    inDialogue = false;
    currentDialogueStep = 0;
    isGameOver = false;
    isGameWon = false;
    messages.clear();

    // 初始化圣剑碎片
    for (int i = 0; i < 3; i++) {
        swordFragments[i] = false;
    }

    // 清理旧的游戏对象
    delete player;
    player = nullptr;

    for (auto enemy : enemies) {
        delete enemy;
    }
    enemies.clear();

    items.clear();

    // 加载资源
    loadResources();

    // 生成地图
    generateFirstLevelMap();

    // 生成物品
    generateFirstStageItems();

    // 初始化玩家
    player = new Player(Position(1, 1));

    // 初始化敌人
    enemies.push_back(new Enemy(Position(15, 10)));
    enemies.push_back(new Enemy(Position(10, 12)));

    // 设置 NPC
    npcPosition = findValidNPCPosition();
    npcType = NPCType::FOREST_GUARDIAN;
    initForestGuardianDialogue();

    // 显示欢迎消息
    showMessage("第一关：森林迷宫", 180, RGB(100, 255, 100));
}

void GameScene::initNextStage() {
    // 清理旧的游戏对象
//    delete player;
//    player = nullptr;
    for (auto enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
    items.clear();
    // 重置游戏状态
    hasKey = false;
//    playerHealth = 6;
//    playerMaxHealth = 6;
    inDialogue = false;
    currentDialogueStep = 0;
    isGameOver = false;
    isGameWon = false;
    messages.clear();
    // 生成新地图
    if (stage == 2) {
        generateSecondLevelMap();
        showMessage("第二关：天空之城", 180, RGB(100, 100, 255));
        npcType = NPCType::SKY_GUARDIAN;
        initSkyGuardianDialogue();
        // 生成物品
        generateSecondStageItems();
    }
    else if (stage == 3) {
        generateThirdLevelMap();
        showMessage("第三关：岩石洞穴", 180, RGB(255, 100, 100));
        npcType = NPCType::ROCK_GUARDIAN;
        initRockGuardianDialogue();
        //生成物品
        generateThirdStageItems();
    }
    // 初始化玩家
    player = new Player(Position(1, 1));
    // 初始化敌人
    enemies.push_back(new Enemy(Position(15, 10)));
    enemies.push_back(new Enemy(Position(10, 12)));
    // 设置 NPC
    npcPosition = findValidNPCPosition();
}

void GameScene::exit() {
    // 清理游戏对象-------------------?
//    delete player;
//    player = nullptr;

    for (auto enemy : enemies) {
        delete enemy;
    }
    enemies.clear();

    // 清理资源
    unloadResources();
}

void GameScene::update() {
    if (isGameOver || isGameWon) {
        return;
    }

    if (inDialogue) {
        updateDialogue();
        updateMessages();
        return;
    }

    // 玩家移动输入
    static bool keysProcessed[4] = { false, false, false, false };
    int dx = 0, dy = 0;

    if (GetAsyncKeyState('W') & 0x8000) {
        if (!keysProcessed[0]) {
            dy = -1;
            keysProcessed[0] = true;
        }
    }
    else {
        keysProcessed[0] = false;
    }

    if (GetAsyncKeyState('S') & 0x8000) {
        if (!keysProcessed[1]) {
            dy = 1;
            keysProcessed[1] = true;
        }
    }
    else {
        keysProcessed[1] = false;
    }

    if (GetAsyncKeyState('A') & 0x8000) {
        if (!keysProcessed[2]) {
            dx = -1;
            keysProcessed[2] = true;
        }
    }
    else {
        keysProcessed[2] = false;
    }

    if (GetAsyncKeyState('D') & 0x8000) {
        if (!keysProcessed[3]) {
            dx = 1;
            keysProcessed[3] = true;
        }
    }
    else {
        keysProcessed[3] = false;
    }

    // 检查 NPC 对话（按 E 键）
    static bool eProcessed = false;
    if (GetAsyncKeyState('E') & 0x8000) {
        if (!eProcessed) {
            if (isPlayerNearNPC()) {
                startDialogue();
                if(stage == 1) showMessage("与森林守护者对话中...", 90);
                else if(stage == 2) showMessage("与天空守护者对话中...", 90);
				else if(stage == 3) showMessage("与熔岩守护者对话中...", 90);
            }
            eProcessed = true;
        }
    }
    else {
        eProcessed = false;
    }

    // 移动玩家
    if (dx != 0 || dy != 0) {
        Position newPos = player->getPosition();
        newPos.x += dx;
        newPos.y += dy;

        // 检查是否可以移动
        if (isPositionValid(newPos) && isPositionWalkable(newPos) &&
            !isPositionOccupiedByEnemy(newPos)) {
            player->setPosition(newPos);

            // 检查物品碰撞
            checkItemCollision();

            // 检查出口
            if (map[newPos.y][newPos.x] == TileType::EXIT && hasKey) {
                isGameWon = true;
                showMessage("通关成功！找到出口了！", 300, RGB(255, 215, 0));
                return;
            }
        }
    }

    // 更新敌人
    updateEnemies();

    // 检查敌人碰撞
    checkEnemyCollision();

    // 检查游戏状态
    checkGameStatus();

    // 更新消息系统
    updateMessages();
}

void GameScene::render() {
    // 清屏（黑色背景）
    setbkcolor(BLACK);
    cleardevice();

    if (isGameOver) {
        // 显示游戏结束画面
        settextcolor(RED);
        settextstyle(48, 0, _T("楷体"));
        outtextxy(300, 250, _T("游戏结束"));

        settextcolor(WHITE);
        settextstyle(24, 0, _T("宋体"));
        outtextxy(320, 320, _T("按R重新开始"));
        outtextxy(320, 360, _T("按ESC返回菜单")); 

        static bool rProcessed = false;
        if (GetAsyncKeyState('R') & 0x8000) {
            if (!rProcessed) {
                sceneManager->switchTo(SceneType::GAME);
                rProcessed = true;
            }
        }
        else {
            rProcessed = false;
        }

        static bool escProcessed = false;
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            if (!escProcessed) {
                sceneManager->switchTo(SceneType::MENU);
                // 可以在这里添加返回菜单的逻辑
                escProcessed = true;
            }
        }
        else {
            escProcessed = false;
        }
        return;
    }

    if (isGameWon) {
        // 显示通关画面
        settextcolor(RGB(255, 215, 0));
        settextstyle(48, 0, _T("楷体"));
        outtextxy(300, 250, _T("通关成功！"));

        settextcolor(WHITE);
        settextstyle(24, 0, _T("宋体"));
        char scoreText[50];
        sprintf_s(scoreText, "得分: %d", score);
        outtextxy(350, 320, scoreText);
        outtextxy(300, 360, _T("按空格键进入下一关"));   
		static bool spaceProcessed = false;
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            if (!spaceProcessed) {
                if(stage < 3){
                    stage++;
                    initNextStage();
                }
                else {
                    sceneManager->switchTo(SceneType::BOSS);
                }
                spaceProcessed = true;
            }
        }
        else {
            spaceProcessed = false;
        }
        return;
    }

    // 绘制地图背景（深灰色）
    setfillcolor(RGB(40, 40, 40));
    solidrectangle(0, 0, 800, 600);

    // 绘制地图
    drawMap();

    // 绘制物品
    for (const auto& item : items) {
        int screenX = item.first.x * TILE_SIZE;
        int screenY = item.first.y * TILE_SIZE;

        // 确保物品可见：先绘制背景
        setfillcolor(RGB(40, 40, 40));
        solidrectangle(screenX, screenY, screenX + TILE_SIZE, screenY + TILE_SIZE);

        // 绘制物品
        if (item.second == ItemType::HEALTH_POTION) {
            if (resourcesLoaded) {
                putimage(screenX, screenY, &healthPotionImg);
            }
            else {
                setfillcolor(RGB(0, 255, 0));
                solidrectangle(screenX + 10, screenY + 10, screenX + 30, screenY + 30);
            }
        }
        else if (item.second == ItemType::COIN) {
            if (resourcesLoaded) {
                putimage(screenX, screenY, &coinImg);
            }
            else {
                setfillcolor(RGB(255, 215, 0));
                solidcircle(screenX + 20, screenY + 20, 15);
            }
        }
        else if (item.second == ItemType::KEY) {
            if (resourcesLoaded) {
                putimage(screenX, screenY, &keyImg);
            }
            else {
                setfillcolor(RGB(255, 165, 0));
                solidrectangle(screenX + 10, screenY + 15, screenX + 30, screenY + 25);
                solidcircle(screenX + 35, screenY + 20, 10);
            }
        }
        else if (item.second == ItemType::SWORD_FRAGMENT) {
            if (resourcesLoaded) {
                putimage(screenX, screenY, &swordFragmentImg);
            }
            else {
                setfillcolor(RGB(255, 200, 50));
                POINT triangle[3] = {
                    {screenX + 20, screenY + 5},
                    {screenX + 5, screenY + 35},
                    {screenX + 35, screenY + 35}
                };
                solidpolygon(triangle, 3);
            }
        }
    }

    // 绘制 NPC
    if (!inDialogue) {
        if (resourcesLoaded) {
            putimage(npcPosition.x * TILE_SIZE, npcPosition.y * TILE_SIZE, &npcImg);
        }
        else {
            drawDefaultNPC(npcPosition.x * TILE_SIZE, npcPosition.y * TILE_SIZE);
        }

        // 当玩家靠近时显示互动提示
        if (isPlayerNearNPC()) {
            drawInteractionPrompt(npcPosition.x * TILE_SIZE, npcPosition.y * TILE_SIZE);
        }
    }

    // 绘制敌人
    for (const auto& enemy : enemies) {
        enemy->render(TILE_SIZE);
    }

    // 绘制玩家
    if (player) {
        player->render(TILE_SIZE);
    }

    // 绘制 UI
    drawUI();

    // 绘制消息
    renderMessages();

    // 绘制对话（如果正在进行）
    if (inDialogue) {
        renderDialogue();
    }
}

// ========== 地图生成 ==========

void GameScene::generateFirstLevelMap() {
    // 清空地图
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            map[y][x] = TileType::EMPTY;
        }
    }

    // 创建基本迷宫
    // 外边界墙
    for (int x = 0; x < MAP_WIDTH; x++) {
        map[0][x] = TileType::WALL;
        map[MAP_HEIGHT - 1][x] = TileType::WALL;
    }
    for (int y = 0; y < MAP_HEIGHT; y++) {
        map[y][0] = TileType::WALL;
        map[y][MAP_WIDTH - 1] = TileType::WALL;
    }

    // 内墙（创建简单的迷宫结构）
    // 纵向墙
    for (int y = 2; y < MAP_HEIGHT - 2; y++) {
        map[y][5] = TileType::WALL;
        map[y][10] = TileType::WALL;
        map[y][15] = TileType::WALL;
    }

    // 横向墙
    for (int x = 5; x < MAP_WIDTH - 5; x++) {
        map[3][x] = TileType::WALL;
        map[7][x] = TileType::WALL;
        map[11][x] = TileType::WALL;
    }

    // 创建一些通道
    map[3][8] = TileType::EMPTY;
    map[7][12] = TileType::EMPTY;
    map[11][6] = TileType::EMPTY;
    map[7][5] = TileType::EMPTY;
    map[10][15] = TileType::EMPTY;

    // 设置出生点
    map[1][1] = TileType::PLAYER_SPAWN;

    // 设置出口
    map[MAP_HEIGHT - 2][MAP_WIDTH - 2] = TileType::EXIT;

    // 设置门（在出口前）
    map[MAP_HEIGHT - 2][MAP_WIDTH - 3] = TileType::DOOR;

    // 收集墙的位置用于绘制
    wallPositions.clear();
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == TileType::WALL) {
                wallPositions.push_back(Position(x, y));
            }
        }
    }
}

void GameScene::generateSecondLevelMap() {
    // 清空地图
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            map[y][x] = TileType::EMPTY;
        }
    }

    // 创建基本迷宫
    // 外边界墙
    for (int x = 0; x < MAP_WIDTH; x++) {
        map[0][x] = TileType::WALL;
        map[MAP_HEIGHT - 1][x] = TileType::WALL;
    }
    for (int y = 0; y < MAP_HEIGHT; y++) {
        map[y][0] = TileType::WALL;
        map[y][MAP_WIDTH - 1] = TileType::WALL;
    }

    // 内墙（创建简单的迷宫结构）
    // 纵向墙
    for (int y = 2; y < MAP_HEIGHT - 2; y++) {
        map[y][3] = TileType::WALL;
        map[y][6] = TileType::WALL;
        map[y][10] = TileType::WALL;
        map[y][14] = TileType::WALL;
        map[y][17] = TileType::WALL;
    }

    // 横向墙
    for (int x = 3; x < MAP_WIDTH - 3; x++) {
        map[3][x] = TileType::WALL;
        map[7][x] = TileType::WALL;
        map[11][x] = TileType::WALL;
    }

    // 创建一些通道
    map[3][8] = TileType::EMPTY;
    map[7][12] = TileType::EMPTY;
    map[10][6] = TileType::EMPTY;
    map[7][4] = TileType::EMPTY;
    map[5][6] = TileType::EMPTY;
	map[9][10] = TileType::EMPTY;
	map[4][14] = TileType::EMPTY;
    map[10][17] = TileType::EMPTY;

    // 设置出生点
    map[1][1] = TileType::PLAYER_SPAWN;

    // 设置出口
    map[MAP_HEIGHT - 2][MAP_WIDTH - 2] = TileType::EXIT;

    // 设置门（在出口前）
    map[MAP_HEIGHT - 2][MAP_WIDTH - 3] = TileType::DOOR;

    // 收集墙的位置用于绘制
    wallPositions.clear();
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == TileType::WALL) {
                wallPositions.push_back(Position(x, y));
            }
        }
    }
}

void GameScene::generateThirdLevelMap() {
    // 清空地图
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            map[y][x] = TileType::EMPTY;
        }
    }

    // 创建基本迷宫
    // 外边界墙
    for (int x = 0; x < MAP_WIDTH; x++) {
        map[0][x] = TileType::WALL;
        map[MAP_HEIGHT - 1][x] = TileType::WALL;
    }
    for (int y = 0; y < MAP_HEIGHT; y++) {
        map[y][0] = TileType::WALL;
        map[y][MAP_WIDTH - 1] = TileType::WALL;
    }

    // 内墙（创建简单的迷宫结构）
    // 纵向墙
    for (int y = 2; y < MAP_HEIGHT - 2; y++) {
        map[y][5] = TileType::WALL;
        map[y][10] = TileType::WALL;
        map[y][15] = TileType::WALL;
    }

    // 横向墙
    for (int x = 5; x < MAP_WIDTH - 5; x++) {
        map[3][x] = TileType::WALL;
        map[7][x] = TileType::WALL;
        map[11][x] = TileType::WALL;
    }

    // 创建一些通道
    map[3][8] = TileType::EMPTY;
    map[7][12] = TileType::EMPTY;
    map[11][6] = TileType::EMPTY;
    map[7][5] = TileType::EMPTY;
    map[10][15] = TileType::EMPTY;

    // 设置出生点
    map[1][1] = TileType::PLAYER_SPAWN;

    // 设置出口
    map[MAP_HEIGHT - 2][MAP_WIDTH - 2] = TileType::EXIT;

    // 设置门（在出口前）
    map[MAP_HEIGHT - 2][MAP_WIDTH - 3] = TileType::DOOR;

    // 收集墙的位置用于绘制
    wallPositions.clear();
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == TileType::WALL) {
                wallPositions.push_back(Position(x, y));
            }
        }
    }
}

void GameScene::drawMap() {
    // 绘制地板
    setfillcolor(RGB(50, 50, 50));  // 深灰色地板
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] != TileType::WALL) {
                solidrectangle(x * TILE_SIZE, y * TILE_SIZE,
                    (x + 1) * TILE_SIZE, (y + 1) * TILE_SIZE);
            }
        }
    }

    // 绘制墙
    if (resourcesLoaded) {
        for (const auto& pos : wallPositions) {
            putimage(pos.x * TILE_SIZE, pos.y * TILE_SIZE, &tileWall);
        }
    }
    else {
        setfillcolor(RGB(100, 70, 50));  // 棕色墙
        for (const auto& pos : wallPositions) {
            solidrectangle(pos.x * TILE_SIZE, pos.y * TILE_SIZE,
                (pos.x + 1) * TILE_SIZE, (pos.y + 1) * TILE_SIZE);
        }
    }

    // 绘制门
    setfillcolor(RGB(139, 69, 19));  // 棕色门
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == TileType::DOOR) {
                solidrectangle(x * TILE_SIZE, y * TILE_SIZE,
                    (x + 1) * TILE_SIZE, (y + 1) * TILE_SIZE);
            }
        }
    }

    // 绘制出口
    setfillcolor(RGB(0, 100, 0));  // 深绿色出口
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == TileType::EXIT) {
                solidrectangle(x * TILE_SIZE, y * TILE_SIZE,
                    (x + 1) * TILE_SIZE, (y + 1) * TILE_SIZE);
            }
        }
    }
}

// ========== 物品管理 ==========

void GameScene::generateFirstStageItems() {
    items.clear();

    // 生成药水
    items.push_back({ Position(3, 3), ItemType::HEALTH_POTION });
    items.push_back({ Position(15, 10), ItemType::HEALTH_POTION });

    // 生成金币
    items.push_back({ Position(5, 7), ItemType::COIN });
    items.push_back({ Position(12, 12), ItemType::COIN });
    items.push_back({ Position(18, 8), ItemType::COIN });

    // 生成钥匙（确保不在墙上）
    Position keyPos(2, 2);
    if (isPositionWalkable(keyPos)) {
        items.push_back({ keyPos, ItemType::KEY });
    }
    else {
        // 尝试其他位置
        Position altPositions[] = { Position(3, 2), Position(2, 3), Position(4, 4) };
        for (const auto& pos : altPositions) {
            if (isPositionWalkable(pos)) {
                items.push_back({ pos, ItemType::KEY });
                break;
            }
        }
    }

    // 生成圣剑碎片
    items.push_back({ Position(14, 4), ItemType::SWORD_FRAGMENT });
}

void GameScene::generateSecondStageItems() {
    items.clear();

    // 生成药水
    items.push_back({ Position(15, 2), ItemType::HEALTH_POTION });
    items.push_back({ Position(4, 10), ItemType::HEALTH_POTION });

    // 生成金币
    items.push_back({ Position(4, 7), ItemType::COIN });
    items.push_back({ Position(12, 12), ItemType::COIN });
    items.push_back({ Position(18, 8), ItemType::COIN });

    // 生成钥匙（确保不在墙上）
    Position keyPos(1, 13);
    if (isPositionWalkable(keyPos)) {
        items.push_back({ keyPos, ItemType::KEY });
    }
    else {
        // 尝试其他位置
        Position altPositions[] = { Position(3, 2), Position(2, 3), Position(4, 4) };
        for (const auto& pos : altPositions) {
            if (isPositionWalkable(pos)) {
                items.push_back({ pos, ItemType::KEY });
                break;
            }
        }
    }

    // 生成圣剑碎片
    items.push_back({ Position(16, 6), ItemType::SWORD_FRAGMENT });
}

void GameScene::generateThirdStageItems() {
    items.clear();

    // 生成药水
    items.push_back({ Position(3, 3), ItemType::HEALTH_POTION });
    items.push_back({ Position(15, 10), ItemType::HEALTH_POTION });

    // 生成金币
    items.push_back({ Position(5, 7), ItemType::COIN });
    items.push_back({ Position(12, 12), ItemType::COIN });
    items.push_back({ Position(18, 8), ItemType::COIN });

    // 生成钥匙（确保不在墙上）
    Position keyPos(2, 2);
    if (isPositionWalkable(keyPos)) {
        items.push_back({ keyPos, ItemType::KEY });
    }
    else {
        // 尝试其他位置
        Position altPositions[] = { Position(3, 2), Position(2, 3), Position(4, 4) };
        for (const auto& pos : altPositions) {
            if (isPositionWalkable(pos)) {
                items.push_back({ pos, ItemType::KEY });
                break;
            }
        }
    }

    // 生成圣剑碎片
    items.push_back({ Position(14, 4), ItemType::SWORD_FRAGMENT });
}

void GameScene::checkItemCollision() {
    Position playerPos = player->getPosition();

    auto it = items.begin();
    while (it != items.end()) {
        if (playerPos == it->first) {
            // 碰撞到物品
            switch (it->second) {
            case ItemType::HEALTH_POTION:
                playerHealth = min(playerHealth + 1, playerMaxHealth);
                showMessage("获得生命药水！生命值+1", 120, RGB(0, 255, 0));
                break;

            case ItemType::COIN:
                score += 100;
                showMessage("获得金币！分数+100", 120, RGB(255, 215, 0));
                break;

            case ItemType::KEY:
                hasKey = true;
                // 移除门
                for (int y = 0; y < MAP_HEIGHT; y++) {
                    for (int x = 0; x < MAP_WIDTH; x++) {
                        if (map[y][x] == TileType::DOOR) {
                            map[y][x] = TileType::EMPTY;
                        }
                    }
                }
                showMessage("获得钥匙！门已打开", 180, RGB(255, 165, 0));
                break;

            case ItemType::SWORD_FRAGMENT:
                if (stage == 1) swordFragments[0] = true;  // 第一关获得第一个碎片
                if (stage == 2) swordFragments[1] = true;  // 第二关获得第一个碎片
                if (stage == 3) swordFragments[2] = true;  // 第三关获得第一个碎片
                showMessage("获得圣剑碎片！", 180, RGB(255, 200, 50));
                break;
            }

            // 移除物品
            it = items.erase(it);
        }
        else {
            ++it;
        }
    }
}

void GameScene::removeItem(const Position& pos) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->first == pos) {
            items.erase(it);
            break;
        }
    }
}

// ========== NPC 对话 ==========

void GameScene::initForestGuardianDialogue() {
    currentDialogue.clear();

    currentDialogue.push_back({ "", "一位身形半透明、身着古老树皮长袍的森林守护者从圣坛后显现。" });
    currentDialogue.push_back({ "", "她面容慈祥，眼中闪烁着星辰般的光芒。" });
    currentDialogue.push_back({ "森林守护者莉拉娜", "年轻的人之子，我等待你的到来已经很久了。" });
    currentDialogue.push_back({ "艾登", "您是...森林的守护者？我是来寻找圣剑碎片的。" });
    currentDialogue.push_back({ "森林守护者莉拉娜", "我知道。我能感受到你血脉中的圣光，也看到了你穿越迷宫时的坚定。" });
    currentDialogue.push_back({ "森林守护者莉拉娜", "告诉我，勇者——你为何踏上这危险的道路？" });
    currentDialogue.push_back({ "艾登", "为了驱散这片黑暗" });
    currentDialogue.push_back({ "森林守护者莉拉娜", "三个答案都是真实的...却又不完整。让我看看你的心吧。" });
    currentDialogue.push_back({ "", "*莉拉娜轻轻挥手，周围浮现三个光球，每个光球都映出一段回忆：" });
    currentDialogue.push_back({ "", "艾登在城堡中训练的场景" });
    currentDialogue.push_back({ "", "村民们被黑暗侵扰的恐惧面容" });
    currentDialogue.push_back({ "", "年幼的艾登听祖母讲述圣骑士传说的夜晚*" });
    currentDialogue.push_back({ "森林守护者莉拉娜", "啊...我看到了。" });
    currentDialogue.push_back({ "森林守护者莉拉娜", "你的勇气不仅来自责任，更来自那些温暖记忆给予你的力量。" });
    currentDialogue.push_back({ "森林守护者莉拉娜", "你没有被黑暗动摇，也没有被恐惧吞噬...这正是圣剑需要的光芒。" });
    currentDialogue.push_back({ "", "莉拉娜走向圣坛，碎片开始发出更强烈的光芒" });
    currentDialogue.push_back({ "森林守护者莉拉娜", "千百年来，我守护着这片'自然之翠'。" });
    currentDialogue.push_back({ "森林守护者莉拉娜", "它蕴含着生命的坚韧与循环之力。" });
    currentDialogue.push_back({ "森林守护者莉拉娜", "但最近，我感到森林中的黑暗在蔓延..." });
    currentDialogue.push_back({ "森林守护者莉拉娜", "这碎片需要新的主人，需要为更伟大的使命而战。" });
    currentDialogue.push_back({ "", "碎片缓缓飘向艾登" });
    currentDialogue.push_back({ "森林守护者莉拉娜", "接受它吧。" });
    currentDialogue.push_back({ "森林守护者莉拉娜", "记住——真正的力量不在于征服，而在于守护生命的意志。" });
    currentDialogue.push_back({ "森林守护者莉拉娜", "愿森林的坚韧与你同在。" });
}

void GameScene::initSkyGuardianDialogue(){
    currentDialogue.clear();
}

void GameScene::initRockGuardianDialogue(){
    currentDialogue.clear();
}

void GameScene::startDialogue() {
    inDialogue = true;
    currentDialogueStep = 0;
}

void GameScene::updateDialogue() {
    static bool spaceProcessed = false;

    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        if (!spaceProcessed) {
            spaceProcessed = true;
            currentDialogueStep++;

            if (currentDialogueStep >= currentDialogue.size()) {
                endDialogue();
                // 对话结束后给予圣剑碎片
                if (!swordFragments[0]) {
                    items.push_back({ npcPosition, ItemType::SWORD_FRAGMENT });
                    showMessage("森林守护者给予你圣剑碎片！", 180, RGB(255, 200, 50));
                }
            }
        }
    }
    else {
        spaceProcessed = false;
    }

    // ESC 跳过对话
    static bool escProcessed = false;
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        if (!escProcessed) {
            endDialogue();
            escProcessed = true;
        }
    }
    else {
        escProcessed = false;
    }
}

void GameScene::renderDialogue() {
    if (currentDialogueStep >= currentDialogue.size()) {
        return;
    }

    // 绘制半透明覆盖层
    setfillcolor(RGB(0, 0, 0, 180));
    solidrectangle(0, 0, 800, 600);

    // 绘制对话框
    drawDialogueBox(50, 300, 700, 250);

    // 绘制对话内容
    const NPCDialogue& dialogue = currentDialogue[currentDialogueStep];

    if (!dialogue.speaker.empty()) {
        // 绘制说话者名字
        settextcolor(RGB(255, 215, 0));  // 金色
        settextstyle(24, 0, _T("楷体"));
        outtextxy(80, 320, dialogue.speaker.c_str());
    }

    // 绘制对话内容
    drawTextWithWrap(80, 360, 650, dialogue.content, RGB(255, 255, 255), 20);

    // 绘制提示
    settextcolor(RGB(200, 200, 200));
    settextstyle(18, 0, _T("宋体"));
    outtextxy(550, 520, _T("按空格继续"));
    outtextxy(550, 540, _T("按ESC跳过"));
}

void GameScene::endDialogue() {
    inDialogue = false;
    currentDialogueStep = 0;
}

// ========== NPC 绘制辅助 ==========

void GameScene::drawDefaultNPC(int screenX, int screenY) {
    // NPC主体（绿色圆形）
    setfillcolor(RGB(100, 180, 100));
    solidcircle(screenX + TILE_SIZE / 2, screenY + TILE_SIZE / 2, TILE_SIZE / 2 - 4);

    // NPC面部特征
    setfillcolor(WHITE);
    solidcircle(screenX + TILE_SIZE / 2 - 8, screenY + TILE_SIZE / 2 - 4, 3);  // 左眼
    solidcircle(screenX + TILE_SIZE / 2 + 8, screenY + TILE_SIZE / 2 - 4, 3);  // 右眼

    // 嘴巴（微笑）
    setlinecolor(WHITE);
    setlinestyle(PS_SOLID, 2);
    arc(screenX + TILE_SIZE / 2 - 6, screenY + TILE_SIZE / 2 + 4,
        screenX + TILE_SIZE / 2 + 6, screenY + TILE_SIZE / 2 + 10,
        0, 180);

    // 精灵光环（森林守护者的特效）
    static int glowAngle = 0;
    glowAngle = (glowAngle + 2) % 360;

    setlinecolor(RGB(100, 255, 100));
    setlinestyle(PS_SOLID, 1);

    for (int i = 0; i < 3; i++) {
        int angle = glowAngle + i * 120;
        int glowX = screenX + TILE_SIZE / 2 + (int)(cos(angle * 3.14159 / 180.0) * 20);
        int glowY = screenY + TILE_SIZE / 2 + (int)(sin(angle * 3.14159 / 180.0) * 20);
        circle(glowX, glowY, 6);
    }
}

bool GameScene::isPlayerNearNPC() const {
    if (!player || inDialogue) return false;

    Position playerPos = player->getPosition();
    int dx = abs(playerPos.x - npcPosition.x);
    int dy = abs(playerPos.y - npcPosition.y);

    return (dx <= 1 && dy <= 1 && (dx + dy) > 0);  // 相邻且不重合
}

void GameScene::drawInteractionPrompt(int npcScreenX, int npcScreenY) {
    // 绘制提示框
    int promptX = npcScreenX - 10;
    int promptY = npcScreenY - 40;

    setfillcolor(RGB(0, 0, 0, 200));
    solidrectangle(promptX, promptY, promptX + 100, promptY + 30);

    setlinecolor(RGB(218, 165, 32));
    rectangle(promptX, promptY, promptX + 100, promptY + 30);

    // 绘制提示文字
    settextcolor(RGB(255, 255, 200));
    settextstyle(16, 0, _T("宋体"));
    setbkmode(TRANSPARENT);

    // 显示"按E互动"
    outtextxy(promptX + 20, promptY + 7, _T("按 E 互动"));

    // 绘制E键图标
    setfillcolor(RGB(255, 200, 0));
    solidrectangle(promptX + 5, promptY + 5, promptX + 18, promptY + 18);

    settextcolor(BLACK);
    settextstyle(12, 0, _T("Arial"));
    outtextxy(promptX + 7, promptY + 3, _T("E"));
}

Position GameScene::findValidNPCPosition() {
    // 预定义几个可能的位置
    std::vector<Position> possiblePositions = {
        Position(10, 5),   // 原位置
        Position(8, 6),    // 备选1
        Position(12, 7),   // 备选2
        Position(9, 8)     // 备选3
    };

    // 检查每个位置是否可通行
    for (const auto& pos : possiblePositions) {
        if (isPositionValid(pos) && isPositionWalkable(pos)) {
            return pos;
        }
    }

    // 如果所有预定义位置都不行，随机找一个空地
    for (int attempts = 0; attempts < 100; attempts++) {
        int x = rand() % MAP_WIDTH;
        int y = rand() % MAP_HEIGHT;
        Position pos(x, y);

        if (isPositionValid(pos) && isPositionWalkable(pos) &&
            !isPositionOccupiedByEnemy(pos)) {
            return pos;
        }
    }

    // 实在找不到，返回默认位置
    return Position(10, 5);
}

// ========== 游戏逻辑 ==========

void GameScene::checkGameStatus() {
    if (playerHealth <= 0) {
        isGameOver = true;
        showMessage("游戏结束！", 300, RED);
    }
}

void GameScene::updateEnemies() {
    for (auto enemy : enemies) {
        if (enemy->getJustAttackTimer() > 0) continue;
        enemy->update(player->getPosition(), *this);
    }
}

void GameScene::checkEnemyCollision() {
    Position playerPos = player->getPosition();

    for (const auto& enemy : enemies) {
        if (enemy->getJustAttackTimer() > 0) {
            enemy->decreaseJustAttackTimer();
            continue;
        }
        if (playerPos == enemy->getPosition()) {
            playerHealth--;

            // 击退效果
            Position enemyPos = enemy->getPosition();
            int dx = playerPos.x - enemyPos.x;
            int dy = playerPos.y - enemyPos.y;

            // 尝试击退玩家
            Position newPos = playerPos;
            if (dx > 0 && isPositionWalkable({ playerPos.x + 1, playerPos.y })) {
                newPos.x += 1;
            }
            else if (dx < 0 && isPositionWalkable({ playerPos.x - 1, playerPos.y })) {
                newPos.x -= 1;
            }
            else if (dy > 0 && isPositionWalkable({ playerPos.x, playerPos.y + 1 })) {
                newPos.y += 1;
            }
            else if (dy < 0 && isPositionWalkable({ playerPos.x, playerPos.y - 1 })) {
                newPos.y -= 1;
            }

            player->setPosition(newPos);

            // 限制最低生命值
            if (playerHealth < 0) playerHealth = 0;

            // 显示受伤消息
            if (playerHealth > 0) {
                showMessage("被敌人攻击！生命值-1", 120, RED);
            }

            // 设置攻击冷却
            enemy->setJustAttackTimer(100);

            break;  // 一次只处理一个碰撞
        }
    }
}

// ========== 资源管理 ==========

void GameScene::loadResources() {
    // 尝试加载资源文件
    // 这里可以添加实际的图片加载代码
    resourcesLoaded = false;  // 暂时设置为false，使用默认图形

    // 如果没有资源，创建默认的
    if (!resourcesLoaded) {
        createDefaultResources();
    }
}

void GameScene::unloadResources() {
    // EasyX会自动管理资源
    // 可以在这里添加自定义的资源清理
}

void GameScene::createDefaultResources() {
    // 设置文字背景透明
    setbkmode(TRANSPARENT);

    // 创建默认墙砖
    SetWorkingImage(&tileWall);
    Resize(&tileWall, TILE_SIZE, TILE_SIZE);
    setfillcolor(RGB(80, 60, 40));
    solidrectangle(0, 0, TILE_SIZE, TILE_SIZE);

    // 设置线条颜色
    setlinecolor(RGB(60, 40, 20));
    setlinestyle(PS_SOLID, 2);

    // 绘制砖块纹理
    for (int i = 0; i < TILE_SIZE; i += 4) {
        line(0, i, TILE_SIZE, i);
    }

    // 创建默认药水
    SetWorkingImage(&healthPotionImg);
    Resize(&healthPotionImg, TILE_SIZE, TILE_SIZE);
    setfillcolor(BLACK);
    solidrectangle(0, 0, TILE_SIZE, TILE_SIZE);
    setfillcolor(RGB(0, 200, 0));
    solidellipse(4, 4, TILE_SIZE - 4, TILE_SIZE - 4);

    // 创建默认金币
    SetWorkingImage(&coinImg);
    Resize(&coinImg, TILE_SIZE, TILE_SIZE);
    setfillcolor(BLACK);
    solidrectangle(0, 0, TILE_SIZE, TILE_SIZE);
    setfillcolor(RGB(255, 200, 0));
    solidcircle(TILE_SIZE / 2, TILE_SIZE / 2, TILE_SIZE / 2 - 4);

    // 创建默认钥匙（确保明显可见）
    SetWorkingImage(&keyImg);
    Resize(&keyImg, TILE_SIZE, TILE_SIZE);
    setfillcolor(RGB(60, 60, 60));  // 浅灰色背景，不是黑色
    solidrectangle(0, 0, TILE_SIZE, TILE_SIZE);
    setfillcolor(RGB(255, 150, 0));

    // 钥匙柄（更粗）
    solidrectangle(TILE_SIZE / 4 - 2, TILE_SIZE / 2 - 4,
        TILE_SIZE / 2 + 2, TILE_SIZE / 2 + 4);
    // 钥匙环（更大）
    solidellipse(TILE_SIZE / 2 - 2, TILE_SIZE / 2 - 8,
        TILE_SIZE * 3 / 4 + 2, TILE_SIZE / 2 + 8);

    // 创建默认圣剑碎片
    SetWorkingImage(&swordFragmentImg);
    Resize(&swordFragmentImg, TILE_SIZE, TILE_SIZE);
    setfillcolor(BLACK);
    solidrectangle(0, 0, TILE_SIZE, TILE_SIZE);
    setfillcolor(RGB(255, 200, 50));

    POINT triangle[3] = {
        {TILE_SIZE / 2, 4},
        {6, TILE_SIZE - 6},
        {TILE_SIZE - 6, TILE_SIZE - 6}
    };
    solidpolygon(triangle, 3);

    // 创建默认 NPC（森林守护者）
    SetWorkingImage(&npcImg);
    Resize(&npcImg, TILE_SIZE, TILE_SIZE);
    setfillcolor(BLACK);
    solidrectangle(0, 0, TILE_SIZE, TILE_SIZE);
    setfillcolor(RGB(100, 180, 100));
    solidellipse(4, 4, TILE_SIZE - 4, TILE_SIZE - 4);

    // 添加面部特征
    setfillcolor(WHITE);
    solidcircle(TILE_SIZE / 2 - 8, TILE_SIZE / 2 - 4, 3);  // 左眼
    solidcircle(TILE_SIZE / 2 + 8, TILE_SIZE / 2 - 4, 3);  // 右眼

    // 恢复工作图像
    SetWorkingImage();
    Resize(NULL, 800, 600);

    resourcesLoaded = true;
}

// ========== 工具函数 ==========

bool GameScene::isPositionValid(const Position& pos) const {
    return pos.x >= 0 && pos.x < MAP_WIDTH &&
        pos.y >= 0 && pos.y < MAP_HEIGHT;
}

bool GameScene::isPositionWalkable(const Position& pos) const {
    if (!isPositionValid(pos)) return false;

    TileType tile = map[pos.y][pos.x];
    return tile == TileType::EMPTY ||
        tile == TileType::PLAYER_SPAWN ||
        tile == TileType::NPC_SPAWN ||
        tile == TileType::ENEMY_SPAWN ||
        (tile == TileType::EXIT && hasKey) ||
        (tile == TileType::DOOR && hasKey);
}

bool GameScene::isPositionOccupiedByEnemy(const Position& pos) const {
    for (const auto& enemy : enemies) {
        if (enemy->getPosition() == pos) {
            return true;
        }
    }
    return false;
}

// ========== 消息系统 ==========

void GameScene::showMessage(const std::string& text, int duration, COLORREF color) {
    GameMessage msg;
    msg.text = text;
    msg.duration = duration;
    msg.timer = duration;
    msg.color = color;
    messages.push_back(msg);
}

void GameScene::updateMessages() {
    for (auto it = messages.begin(); it != messages.end();) {
        it->timer--;
        if (it->timer <= 0) {
            it = messages.erase(it);
        }
        else {
            ++it;
        }
    }
}

void GameScene::renderMessages() {
    if (messages.empty()) return;

    int startY = 450;
    int maxWidth = 600;

    for (const auto& msg : messages) {
        // 计算透明度（淡入淡出效果）
        int alpha = 255;
        if (msg.timer < 30) {
            alpha = (msg.timer * 255) / 30;  // 淡出
        }
        else if (msg.duration - msg.timer < 15) {
            alpha = ((msg.duration - msg.timer) * 255) / 15;  // 淡入
        }

        // 计算文字宽度
        settextstyle(20, 0, _T("宋体"));
        int textWidth = textwidth(msg.text.c_str());

        // 绘制背景
        setfillcolor(RGB(0, 0, 0, alpha * 0.7));
        int padding = 20;
        solidrectangle(400 - textWidth / 2 - padding, startY - 10,
            400 + textWidth / 2 + padding, startY + 30);

        // 绘制边框
        setlinecolor(RGB(218, 165, 32, alpha));
        setlinestyle(PS_SOLID, 2);
        rectangle(400 - textWidth / 2 - padding, startY - 10,
            400 + textWidth / 2 + padding, startY + 30);

        // 绘制文字
        settextcolor(RGB(
            GetRValue(msg.color),
            GetGValue(msg.color),
            GetBValue(msg.color),
            alpha
        ));
        setbkmode(TRANSPARENT);
        outtextxy(400 - textWidth / 2, startY, msg.text.c_str());

        startY += 50;
    }
}

// ========== UI 绘制 ==========

void GameScene::drawUI() {
    // 绘制血条
    drawHealthBar(20, 20, 200, 20, playerHealth, playerMaxHealth);

    // 绘制分数
    settextcolor(WHITE);
    settextstyle(20, 0, _T("宋体"));
    setbkmode(TRANSPARENT);

    char scoreText[50];
    sprintf_s(scoreText, "分数: %d", score);
    outtextxy(600, 20, scoreText);

    // 绘制钥匙状态
    if (hasKey) {
        settextcolor(RGB(255, 165, 0));
        outtextxy(600, 50, _T("钥匙: 已获得"));
    }
    else {
        settextcolor(RGB(150, 150, 150));
        outtextxy(600, 50, _T("钥匙: 未获得"));
    }

    // 绘制圣剑碎片状态
    char fragmentText[50];
	int numSwordFragments = swordFragments[0] ? 1 : 0;
	numSwordFragments += swordFragments[1] ? 1 : 0;
	numSwordFragments += swordFragments[2] ? 1 : 0;
    sprintf_s(fragmentText, "圣剑碎片: %d/3", numSwordFragments);
    outtextxy(600, 80, fragmentText);

    // 绘制操作提示
    settextcolor(RGB(200, 200, 200));
    settextstyle(16, 0, _T("宋体"));
    outtextxy(20, 560, _T("WASD: 移动  E: 对话  ESC: 菜单"));
}

void GameScene::drawHealthBar(int x, int y, int width, int height, int current, int max) {
    // 绘制背景
    setfillcolor(RGB(100, 0, 0));
    solidrectangle(x, y, x + width, y + height);

    // 绘制当前血量
    if (current > 0) {
        int fillWidth = width * current / max;
        setfillcolor(RGB(255, 0, 0));
        solidrectangle(x, y, x + fillWidth, y + height);
    }

    // 绘制边框
    setlinecolor(WHITE);
    rectangle(x, y, x + width, y + height);

    // 绘制文字
    settextcolor(WHITE);
    settextstyle(16, 0, _T("宋体"));
    setbkmode(TRANSPARENT);

    char healthText[50];
    sprintf_s(healthText, "生命: %d/%d", current, max);
    outtextxy(x + 5, y + 2, healthText);
}

// ========== 对话工具 ==========

void GameScene::drawTextWithWrap(int x, int y, int width, const std::string& text,
    COLORREF color, int fontSize) {
    settextcolor(color);
    settextstyle(fontSize, 0, _T("宋体"));
    setbkmode(TRANSPARENT);

    std::string currentLine;
    int lineHeight = fontSize + 4;
    int currentY = y;

    std::istringstream iss(text);
    std::string word;

    while (std::getline(iss, word, '\n')) {
        std::istringstream words(word);
        currentLine.clear();

        while (words >> word) {
            std::string testLine = currentLine + (currentLine.empty() ? "" : " ") + word;
            int textWidth = textwidth(testLine.c_str());

            if (textWidth > width) {
                // 输出当前行
                outtextxy(x, currentY, currentLine.c_str());
                currentY += lineHeight;
                currentLine = word;
            }
            else {
                currentLine = testLine;
            }
        }

        // 输出最后一行
        if (!currentLine.empty()) {
            outtextxy(x, currentY, currentLine.c_str());
            currentY += lineHeight;
        }
    }
}

void GameScene::drawDialogueBox(int x, int y, int width, int height) {
    // 绘制对话框背景
    setfillcolor(RGB(0, 0, 0, 220));
    solidrectangle(x, y, x + width, y + height);

    // 绘制边框
    setlinecolor(RGB(218, 165, 32));  // 金色
    setlinestyle(PS_SOLID, 4);
    rectangle(x, y, x + width, y + height);

    // 内边框
    setlinecolor(RGB(139, 69, 19));  // 棕色
    setlinestyle(PS_SOLID, 2);
    rectangle(x + 5, y + 5, x + width - 5, y + height - 5);
}