// BossFightScene.cpp
#define NOMINMAX// 防止windows.h定义min/max宏
#include"CommonTypes.h"
#include "BossFightScene.h"
#include "SceneManager.h"
#include "GameScene.h"  // 为了获取玩家状态
#include <graphics.h>
#include <conio.h>
#include <vector>
#include <string>
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
    int health;
    int maxHealth;
    bool hasHolySword;

public:
    Player() : sprite(nullptr), health(6), maxHealth(6), hasHolySword(false) {}
    Player(Position startPos) : position(startPos), sprite(nullptr),
        health(6), maxHealth(6), hasHolySword(false) {
    }

    // 获取/设置位置
    Position getPosition() const { return position; }
    void setPosition(const Position& pos) { position = pos; }

    // 获取/设置精灵
    void setSprite(IMAGE* img) { sprite = img; }

    // 移动
    void move(int dx, int dy) {
        position.x += dx;
        position.y += dy;
    }

    // 血量管理
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    void setHealth(int h) { health = h; if (health > maxHealth) health = maxHealth; }
    void takeDamage(int damage) { health -= damage; if (health < 0) health = 0; }
    void heal(int amount) { health += amount; if (health > maxHealth) health = maxHealth; }

    // 圣剑状态
    bool hasHolySwordAbility() const { return hasHolySword; }
    void setHolySword(bool has) { hasHolySword = has; }

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

// ========== Boss 类 ==========
class Boss {
private:
    Position position;
    IMAGE* sprite;
    int health;
    int maxHealth;
    int phase;

public:
    Boss() : sprite(nullptr), health(3000), maxHealth(3000), phase(1) {}
    Boss(Position startPos) : position(startPos), sprite(nullptr),
        health(3000), maxHealth(3000), phase(1) {
    }

    Position getPosition() const { return position; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getPhase() const { return phase; }

    void setSprite(IMAGE* img) { sprite = img; }
    void takeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0;
    }

    void setPhase(int p) { phase = p; }

    void render(int tileSize) const {
        if (sprite) {
            putimage(position.x * tileSize, position.y * tileSize, sprite);
        }
        else {
            // 绘制默认Boss方块
            setfillcolor(RGB(128, 0, 128));  // 紫色
            solidrectangle(position.x * tileSize,
                position.y * tileSize,
                (position.x + 1) * tileSize - 2,
                (position.y + 1) * tileSize - 2);
        }
    }
};

// ========== BossBullet 基类 ==========
class BossBullet {
protected:
    Position position;
    Position velocity;
    IMAGE* sprite;
    BossBulletType type;
    int damage;
    int lifeTimer;
    int maxLifeTime;
    bool active;

public:
    BossBullet(Position pos, Position vel, BossBulletType t, int dmg, int lifeTime = -1)
        : position(pos), velocity(vel), sprite(nullptr), type(t),
        damage(dmg), lifeTimer(0), maxLifeTime(lifeTime), active(true) {
    }

    virtual ~BossBullet() = default;

    virtual void update() {
        position.x += velocity.x;
        position.y += velocity.y;
        lifeTimer++;

        if (maxLifeTime > 0 && lifeTimer >= maxLifeTime) {
            active = false;
        }
    }

    virtual void render(int tileSize) const {
        if (sprite) {
            putimage(position.x * tileSize, position.y * tileSize, sprite);
        }
        else {
            // 默认绘制
            setfillcolor(RGB(255, 100, 100));
            solidrectangle(position.x * tileSize,
                position.y * tileSize,
                (position.x + 1) * tileSize - 2,
                (position.y + 1) * tileSize - 2);
        }
    }

    bool isActive() const { return active; }
    void deactivate() { active = false; }
    Position getPosition() const { return position; }
    int getDamage() const { return damage; }
    BossBulletType getType() const { return type; }

    void setSprite(IMAGE* img) { sprite = img; }

    // 检查碰撞
    virtual bool checkCollision(const Position& otherPos) const {
        return (position.x == otherPos.x && position.y == otherPos.y);
    }
};

// ========== 骨剑子弹类 ==========
class BoneSwordBullet : public BossBullet {
private:
    bool horizontal;  // true为横向，false为纵向
    int sizeX, sizeY; // 尺寸

public:
    BoneSwordBullet(Position pos, Position vel, bool isHorizontal)
        : BossBullet(pos, vel, BossBulletType::BONE_SWORD_HORIZONTAL, 1, -1),
        horizontal(isHorizontal) {
        if (isHorizontal) {
            type = BossBulletType::BONE_SWORD_HORIZONTAL;
            sizeX = 2; sizeY = 1;  // 0.5*2格
        }
        else {
            type = BossBulletType::BONE_SWORD_VERTICAL;
            sizeX = 1; sizeY = 2;  // 1*2格
        }
    }

    void render(int tileSize) const override {
        if (sprite) {
            putimage(position.x * tileSize, position.y * tileSize, sprite);
        }
        else {
            // 绘制骨剑
            setfillcolor(RGB(200, 200, 200));  // 灰色
            if (horizontal) {
                // 横向骨剑
                solidrectangle(position.x * tileSize,
                    position.y * tileSize + tileSize / 4,
                    (position.x + sizeX) * tileSize,
                    (position.y + sizeY) * tileSize - tileSize / 4);
            }
            else {
                // 纵向骨剑
                solidrectangle(position.x * tileSize + tileSize / 4,
                    position.y * tileSize,
                    (position.x + sizeX) * tileSize - tileSize / 4,
                    (position.y + sizeY) * tileSize);
            }
        }
    }

    bool checkCollision(const Position& otherPos) const override {
        // 检查是否在骨剑的矩形范围内
        return (otherPos.x >= position.x && otherPos.x < position.x + sizeX &&
            otherPos.y >= position.y && otherPos.y < position.y + sizeY);
    }
};

// ========== 火球子弹类 ==========
class FireballBullet : public BossBullet {
private:
    Position targetPos;  // 追踪目标

public:
    FireballBullet(Position pos, Position target)
        : BossBullet(pos, Position(0, 0), BossBulletType::FIREBALL, 1, 300),  // 5秒*60帧
        targetPos(target) {
    }

    void update() override {
        // 追踪玩家
        if (lifeTimer < 150) {  // 2.5秒内追踪
            int dx = targetPos.x - position.x;
            int dy = targetPos.y - position.y;

            // 归一化方向
            float length = sqrt(dx * dx + dy * dy);
            if (length > 0) {
                velocity.x = static_cast<int>((dx / length) * 2);  // 速度2格/秒
                velocity.y = static_cast<int>((dy / length) * 2);
            }
        }

        BossBullet::update();
    }

    void render(int tileSize) const override {
        if (sprite) {
            putimage(position.x * tileSize, position.y * tileSize, sprite);
        }
        else {
            // 绘制火球
            setfillcolor(RGB(255, 100, 0));  // 橙色
            solidcircle(position.x * tileSize + tileSize / 2,
                position.y * tileSize + tileSize / 2,
                tileSize / 2);

            // 火焰效果
            setfillcolor(RGB(255, 200, 0));  // 黄色
            solidcircle(position.x * tileSize + tileSize / 2,
                position.y * tileSize + tileSize / 2,
                tileSize / 4);
        }
    }

    void setTarget(const Position& target) { targetPos = target; }
};

// ========== 激光子弹类 ==========
class LaserBullet : public BossBullet {
private:
    int startX, endX, startY, endY;
    bool horizontal;  // true为横向激光
    int delayTimer;
    bool firing;

public:
    LaserBullet(int x1, int y1, int x2, int y2, bool isHorizontal, int delay)
        : BossBullet(Position(x1, y1), Position(0, 0), BossBulletType::LASER, 1, 30),  // 持续0.5秒
        startX(x1), endX(x2), startY(y1), endY(y2),
        horizontal(isHorizontal), delayTimer(delay), firing(false) {
        if (isHorizontal) {
            position.x = x1;
            position.y = y1;
        }
        else {
            position.x = x1;
            position.y = y1;
        }
    }

    void update() override {
        if (delayTimer > 0) {
            delayTimer--;
            return;
        }

        if (!firing) {
            firing = true;
            lifeTimer = 0;
        }

        BossBullet::update();
    }

    void render(int tileSize) const override {
        if (!firing) return;

        if (sprite) {
            // 绘制激光图片
        }
        else {
            // 绘制激光
            setlinecolor(RGB(255, 0, 0));  // 红色
            setlinestyle(PS_SOLID, 10);    // 粗线

            if (horizontal) {
                // 横向激光
                line(startX * tileSize + tileSize / 2,
                    startY * tileSize + tileSize / 2,
                    endX * tileSize + tileSize / 2,
                    endY * tileSize + tileSize / 2);
            }
            else {
                // 纵向激光
                line(startX * tileSize + tileSize / 2,
                    startY * tileSize + tileSize / 2,
                    endX * tileSize + tileSize / 2,
                    endY * tileSize + tileSize / 2);
            }

            // 激光核心
            setlinecolor(RGB(255, 255, 0));  // 黄色
            setlinestyle(PS_SOLID, 4);

            if (horizontal) {
                line(startX * tileSize + tileSize / 2,
                    startY * tileSize + tileSize / 2,
                    endX * tileSize + tileSize / 2,
                    endY * tileSize + tileSize / 2);
            }
            else {
                line(startX * tileSize + tileSize / 2,
                    startY * tileSize + tileSize / 2,
                    endX * tileSize + tileSize / 2,
                    endY * tileSize + tileSize / 2);
            }
        }
    }

    bool checkCollision(const Position& otherPos) const override {
        if (!firing) return false;

        if (horizontal) {
            // 横向激光：检查Y坐标相同且X在范围内
            return (otherPos.y == startY &&
                otherPos.x >= std::min(startX, endX) &&
                otherPos.x <= std::max(startX, endX));
        }
        else {
            // 纵向激光：检查X坐标相同且Y在范围内
            return (otherPos.x == startX &&
                otherPos.y >= std::min(startY, endY) &&
                otherPos.y <= std::max(startY, endY));
        }
    }
};

// ========== 玩家圣剑子弹类 ==========
class HolySwordBullet : public BossBullet {
public:
    HolySwordBullet(Position pos, Position target)
        : BossBullet(pos, Position(0, 0), BossBulletType::BONE_SWORD_HORIZONTAL, 20, -1) {
        // 计算向目标移动的方向
        int dx = target.x - pos.x;
        int dy = target.y - pos.y;

        // 归一化方向（速度10格/秒）
        float length = sqrt(dx * dx + dy * dy);
        if (length > 0) {
            velocity.x = static_cast<int>((dx / length) * 10);
            velocity.y = static_cast<int>((dy / length) * 10);
        }
    }

    void render(int tileSize) const override {
        if (sprite) {
            putimage(position.x * tileSize, position.y * tileSize, sprite);
        }
        else {
            // 绘制圣剑子弹
            setfillcolor(RGB(255, 255, 100));  // 亮黄色
            solidcircle(position.x * tileSize + tileSize / 2,
                position.y * tileSize + tileSize / 2,
                tileSize / 3);

            // 光晕效果
            setlinecolor(RGB(255, 255, 200));
            setlinestyle(PS_SOLID, 2);
            circle(position.x * tileSize + tileSize / 2,
                position.y * tileSize + tileSize / 2,
                tileSize / 2);
        }
    }
};

// ========== BossFightScene 实现 ==========

BossFightScene::BossFightScene(SceneManager* manager)
    : sceneManager(manager),
    player(nullptr),
    boss(nullptr),
    currentPhase(BossPhase::PHASE_1),
    phaseTransitionTimer(0),
    inPhaseTransition(false),
    currentDialogueStep(0),
    inDialogue(false),
    attackCooldown(480),  // 8秒 * 60帧
    attackTimer(0),
    playerHealth(6),
    playerMaxHealth(6),
    hasHolySword(false),
    holySwordCooldown(0),
    bossHealth(3000),
    bossMaxHealth(3000),
    isBossDefeated(false),
    isGameOver(false) {

    srand(static_cast<unsigned>(time(nullptr)));
    initPhaseTransitionDialogues();
}

BossFightScene::~BossFightScene() {
    exit();
}

void BossFightScene::enter() {
    // 从GameScene获取玩家状态
    // 这里需要sceneManager->getGameScene()来获取碎片状态，暂时假设已获得

    // 初始化玩家
    player = new Player(Position(16, 12));  // 地图中央下方
    player->setHealth(playerHealth);
    player->setHolySword(true);  // Boss战默认获得圣剑

    // 初始化Boss
    boss = new Boss(Position(16, 3));  // 地图上方中央
    boss->setPhase(1);

    // 重置状态
    currentPhase = BossPhase::PHASE_1;
    inPhaseTransition = false;
    inDialogue = false;
    attackTimer = 0;
    isBossDefeated = false;
    isGameOver = false;

    // 清空子弹
    clearAllBullets();

    // 显示消息
    showMessage("最终决战！魔王卡奥斯！", 180, RGB(255, 50, 50));
    showMessage("按J键发射圣剑光弹攻击魔王", 180, RGB(255, 255, 100));
}

void BossFightScene::exit() {
    // 清理游戏对象
    delete player;
    player = nullptr;

    delete boss;
    boss = nullptr;

    // 清理子弹
    clearAllBullets();

    // 清理消息
    messages.clear();
}

void BossFightScene::update() {
    if (isGameOver || isBossDefeated) {
        // 检查下一步操作
        static bool spaceProcessed = false;
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            if (!spaceProcessed) {
                if (isBossDefeated) {
                    sceneManager->switchTo(SceneType::END);
                }
                else {
                    // 重新开始Boss战
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

    if (inPhaseTransition) {
        updatePhaseTransition();
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

    // 玩家圣剑攻击（J键）
    static bool jProcessed = false;
    if (GetAsyncKeyState('J') & 0x8000 && player->hasHolySwordAbility()) {
        if (!jProcessed && holySwordCooldown <= 0) {
            // 发射圣剑子弹
            Position playerPos = player->getPosition();
            Position bossPos = boss->getPosition();
            playerBullets.push_back(new HolySwordBullet(playerPos, bossPos));

            holySwordCooldown = 6;  // 0.1秒 * 60帧
            jProcessed = true;
        }
    }
    else {
        jProcessed = false;
    }

    // 更新圣剑冷却
    if (holySwordCooldown > 0) {
        holySwordCooldown--;
    }

    // 移动玩家（限制在玩家区域）
    if (dx != 0 || dy != 0) {
        Position newPos = player->getPosition();
        newPos.x += dx;
        newPos.y += dy;

        // 检查是否在玩家区域内（y >= 6）
        if (isPositionValid(newPos) && newPos.y >= 6 && newPos.y <= 17) {
            player->setPosition(newPos);
        }
    }

    // 更新攻击计时
    attackTimer++;
    if (attackTimer >= attackCooldown) {
        attackTimer = 0;

        // 根据阶段生成攻击
        if (currentPhase == BossPhase::PHASE_1) {
            // 第一阶段：随机选择Attack1或Attack2
            if (rand() % 2 == 0) {
                generateAttack(BossAttackType::ATTACK1);
            }
            else {
                generateAttack(BossAttackType::ATTACK2);
            }
        }
        else if (currentPhase == BossPhase::PHASE_2) {
            // 第二阶段：随机选择Attack3或Attack4
            if (rand() % 2 == 0) {
                generateAttack(BossAttackType::ATTACK3);
            }
            else {
                generateAttack(BossAttackType::ATTACK4);
            }
        }
        else if (currentPhase == BossPhase::PHASE_3) {
            // 第三阶段：同时出两招
            generateAttack(BossAttackType::ATTACK1);
            generateAttack(BossAttackType::ATTACK3);
        }
    }

    // 更新子弹
    updateBullets();

    // 检查碰撞
    checkBulletCollisions();

    // 检查阶段转换
    checkPhaseTransition();

    // 检查游戏状态
    if (player->getHealth() <= 0) {
        isGameOver = true;
        showMessage("游戏结束！", 300, RED);
    }
    else if (boss->getHealth() <= 0) {
        isBossDefeated = true;
        showMessage("魔王被击败了！", 300, RGB(255, 215, 0));
    }

    // 更新消息
    updateMessages();
}

void BossFightScene::render() {
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
        outtextxy(320, 320, _T("按空格键重新挑战"));
        outtextxy(320, 360, _T("按ESC返回菜单"));
        return;
    }

    if (isBossDefeated) {
        // 显示胜利画面
        settextcolor(RGB(255, 215, 0));
        settextstyle(48, 0, _T("楷体"));
        outtextxy(280, 200, _T("胜利！"));

        settextcolor(WHITE);
        settextstyle(24, 0, _T("宋体"));
        outtextxy(300, 280, _T("魔王卡奥斯被击败了"));
        outtextxy(300, 320, _T("黑暗正在退散..."));
        outtextxy(300, 360, _T("按空格键继续"));
        return;
    }

    // 绘制地图背景（深色）
    setfillcolor(RGB(20, 20, 40));  // 深蓝色背景
    solidrectangle(0, 0, MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE);

    // 绘制区域分界线
    setlinecolor(RGB(100, 100, 150));
    setlinestyle(PS_SOLID, 3);
    line(0, 6 * TILE_SIZE, MAP_WIDTH * TILE_SIZE, 6 * TILE_SIZE);

    // 绘制玩家区域（下半部分）
    setfillcolor(RGB(40, 40, 60));  // 稍亮的玩家区域
    solidrectangle(0, 6 * TILE_SIZE,
        MAP_WIDTH * TILE_SIZE,
        MAP_HEIGHT * TILE_SIZE);

    // 绘制Boss区域（上半部分）
    setfillcolor(RGB(60, 20, 20));  // 红色调的Boss区域
    solidrectangle(0, 0,
        MAP_WIDTH * TILE_SIZE,
        6 * TILE_SIZE);

    // 绘制玩家
    if (player) {
        player->render(TILE_SIZE);
    }

    // 绘制Boss
    if (boss) {
        boss->render(TILE_SIZE);
    }

    // 绘制子弹
    renderBullets();

    // 绘制UI
    drawUI();

    // 绘制消息
    renderMessages();

    // 绘制对话（如果正在进行）
    if (inDialogue) {
        // 绘制半透明覆盖层
        setfillcolor(RGB(0, 0, 0, 180));
        solidrectangle(0, 0, 800, 600);

        // 绘制对话内容
        if (currentDialogueStep < phaseTransitionDialogue.size()) {
            settextcolor(RGB(255, 50, 50));
            settextstyle(32, 0, _T("楷体"));
            outtextxy(200, 250, phaseTransitionDialogue[currentDialogueStep].c_str());

            settextcolor(RGB(200, 200, 200));
            settextstyle(20, 0, _T("宋体"));
            outtextxy(300, 350, _T("按空格继续"));
        }
    }
}

bool BossFightScene::isPositionWalkable(const Position& pos) const {
    return isPositionValid(pos);
}

bool BossFightScene::isPositionInPlayerArea(const Position& pos) const {
    return isPositionValid(pos) && pos.y >= 6 && pos.y <= 17;
}

bool BossFightScene::isPositionInBossArea(const Position& pos) const {
    return isPositionValid(pos) && pos.y >= 0 && pos.y < 6;
}

// 碰撞检测
void BossFightScene::checkBulletCollisions() {
    checkPlayerCollisionWithBossBullets();
    checkBossCollisionWithPlayerBullets();
}

void BossFightScene::checkPlayerCollisionWithBossBullets() {
    if (!player) return;

    Position playerPos = player->getPosition();

    // 检查Boss子弹
    for (auto it = bossBullets.begin(); it != bossBullets.end();) {
        if ((*it)->checkCollision(playerPos)) {
            player->takeDamage((*it)->getDamage());
            delete* it;
            it = bossBullets.erase(it);

            // 显示受伤消息
            if (player->getHealth() > 0) {
                showMessage("被击中！", 60, RED);
            }
        }
        else {
            ++it;
        }
    }
}

void BossFightScene::checkBossCollisionWithPlayerBullets() {
    if (!boss) return;

    Position bossPos = boss->getPosition();

    // 检查玩家子弹
    for (auto it = playerBullets.begin(); it != playerBullets.end();) {
        if ((*it)->checkCollision(bossPos)) {
            boss->takeDamage((*it)->getDamage());
            delete* it;
            it = playerBullets.erase(it);

            // 显示伤害数字
            char damageText[50];
            sprintf_s(damageText, "-20");
            showMessage(damageText, 30, RGB(255, 255, 100));
        }
        else {
            ++it;
        }
    }
}

// 攻击生成
void BossFightScene::generateAttack(BossAttackType attackType) {
    switch (attackType) {
    case BossAttackType::ATTACK1:
        generateAttack1();
        break;
    case BossAttackType::ATTACK2:
        generateAttack2();
        break;
    case BossAttackType::ATTACK3:
        generateAttack3();
        break;
    case BossAttackType::ATTACK4:
        generateAttack4();
        break;
    }
}

void BossFightScene::generateAttack1() {
    if (!player) return;

    Position playerPos = player->getPosition();
    bool fromLeft = (rand() % 2 == 0);

    for (int i = 0; i < 3; i++) {
        int offset = rand() % 5 - 2;  // -2到2的随机偏移
        int y = playerPos.y + offset;

        if (y < 6) y = 6;  // 限制在玩家区域
        if (y > 17) y = 17;

        if (fromLeft) {
            // 从左侧生成，飞向右侧
            Position pos(1, y);
            Position vel(10, 0);  // 10格/秒
            bossBullets.push_back(new BoneSwordBullet(pos, vel, true));
        }
        else {
            // 从右侧生成，飞向左侧
            Position pos(31, y);
            Position vel(-10, 0);
            bossBullets.push_back(new BoneSwordBullet(pos, vel, true));
        }
    }
}

void BossFightScene::generateAttack2() {
    if (!player) return;

    Position playerPos = player->getPosition();
    bool fromTop = (rand() % 2 == 0);

    for (int i = 0; i < 3; i++) {
        int offset = rand() % 5 - 2;  // -2到2的随机偏移
        int x = playerPos.x + offset;

        if (x < 0) x = 0;
        if (x > 31) x = 31;

        if (fromTop) {
            // 从上侧生成，飞向下侧
            Position pos(x, 6);
            Position vel(0, 6);  // 6格/秒
            bossBullets.push_back(new BoneSwordBullet(pos, vel, false));
        }
        else {
            // 从下侧生成，飞向上侧
            Position pos(x, 17);
            Position vel(0, -6);
            bossBullets.push_back(new BoneSwordBullet(pos, vel, false));
        }
    }
}

void BossFightScene::generateAttack3() {
    if (!player) return;

    Position playerPos = player->getPosition();

    for (int i = 0; i < 4; i++) {
        // 在玩家周围7*7区域内随机生成
        int offsetX = rand() % 7 - 3;
        int offsetY = rand() % 7 - 3;

        Position pos(playerPos.x + offsetX, playerPos.y + offsetY);

        // 确保位置有效
        if (!isPositionValid(pos) || !isPositionInPlayerArea(pos)) {
            pos = playerPos;
        }

        FireballBullet* fireball = new FireballBullet(pos, playerPos);
        fireball->setTarget(playerPos);
        bossBullets.push_back(fireball);
    }
}

void BossFightScene::generateAttack4() {
    if (!player) return;

    Position playerPos = player->getPosition();

    // 生成三个激光，依次发射
    for (int i = 0; i < 3; i++) {
        bool horizontal = (rand() % 2 == 0);
        int delay = (i + 1) * 60;  // 每个间隔1秒

        if (horizontal) {
            // 横向激光
            bossBullets.push_back(new LaserBullet(0, playerPos.y,
                MAP_WIDTH - 1, playerPos.y,
                true, delay));
        }
        else {
            // 纵向激光
            bossBullets.push_back(new LaserBullet(playerPos.x, 6,
                playerPos.x, 17,
                false, delay));
        }
    }
}

// 子弹管理
void BossFightScene::updateBullets() {
    // 更新Boss子弹
    for (auto it = bossBullets.begin(); it != bossBullets.end();) {
        (*it)->update();

        // 移除无效子弹
        if (!(*it)->isActive()) {
            delete* it;
            it = bossBullets.erase(it);
        }
        else {
            ++it;
        }
    }

    // 更新玩家子弹
    for (auto it = playerBullets.begin(); it != playerBullets.end();) {
        (*it)->update();

        // 检查是否飞出屏幕
        Position pos = (*it)->getPosition();
        if (!isPositionValid(pos)) {
            delete* it;
            it = playerBullets.erase(it);
        }
        else {
            ++it;
        }
    }
}

void BossFightScene::renderBullets() {
    // 渲染Boss子弹
    for (const auto& bullet : bossBullets) {
        bullet->render(TILE_SIZE);
    }

    // 渲染玩家子弹
    for (const auto& bullet : playerBullets) {
        bullet->render(TILE_SIZE);
    }
}

void BossFightScene::clearAllBullets() {
    for (auto bullet : bossBullets) {
        delete bullet;
    }
    bossBullets.clear();

    for (auto bullet : playerBullets) {
        delete bullet;
    }
    playerBullets.clear();
}

// 阶段转换
void BossFightScene::initPhaseTransitionDialogues() {
    phaseTransitionDialogue.clear();

    // 第一阶段转第二阶段对话
    phaseTransitionDialogue.push_back("卡奥斯：你的光明如此微弱，又怎能照亮黑暗！");

    // 第二阶段转第三阶段对话
    phaseTransitionDialogue.push_back("卡奥斯：小子，还没完呢！");

    // 击败后对话
    phaseTransitionDialogue.push_back("卡奥斯（消散前）：不...不可能...这光...为什么...如此温暖...");
}

void BossFightScene::checkPhaseTransition() {
    if (!boss || inPhaseTransition) return;

    int health = boss->getHealth();
    BossPhase newPhase = currentPhase;

    if (currentPhase == BossPhase::PHASE_1 && health < 1700) {
        newPhase = BossPhase::PHASE_2;
    }
    else if (currentPhase == BossPhase::PHASE_2 && health < 400) {
        newPhase = BossPhase::PHASE_3;
    }

    if (newPhase != currentPhase) {
        currentPhase = newPhase;
        boss->setPhase(static_cast<int>(newPhase));
        startPhaseTransition();
    }
}

void BossFightScene::startPhaseTransition() {
    inPhaseTransition = true;
    inDialogue = true;
    currentDialogueStep = 0;

    // 根据阶段选择对话
    if (currentPhase == BossPhase::PHASE_2) {
        currentDialogueStep = 0;
    }
    else if (currentPhase == BossPhase::PHASE_3) {
        currentDialogueStep = 1;
    }

    showMessage("阶段转换！", 120, RGB(255, 100, 255));
}

void BossFightScene::updatePhaseTransition() {
    static bool spaceProcessed = false;

    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        if (!spaceProcessed) {
            spaceProcessed = true;
            endPhaseTransition();
        }
    }
    else {
        spaceProcessed = false;
    }
}

void BossFightScene::endPhaseTransition() {
    inPhaseTransition = false;
    inDialogue = false;
    phaseTransitionTimer = 0;

    // 清空所有子弹
    clearAllBullets();

    // 重置攻击计时器
    attackTimer = 0;

    // 显示阶段消息
    if (currentPhase == BossPhase::PHASE_2) {
        showMessage("第二阶段开始！注意火球和激光！", 180, RGB(255, 100, 0));
    }
    else if (currentPhase == BossPhase::PHASE_3) {
        showMessage("最终阶段！同时应对多种攻击！", 180, RGB(255, 50, 50));
    }
}

// 消息系统
void BossFightScene::showMessage(const std::string& text, int duration, COLORREF color) {
    GameMessage msg;
    msg.text = text;
    msg.duration = duration;
    msg.timer = duration;
    msg.color = color;
    messages.push_back(msg);
}

void BossFightScene::updateMessages() {
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

void BossFightScene::renderMessages() {
    if (messages.empty()) return;

    int startY = 450;
    int maxWidth = 600;

    for (const auto& msg : messages) {
        // 计算透明度
        int alpha = 255;
        if (msg.timer < 30) {
            alpha = (msg.timer * 255) / 30;
        }
        else if (msg.duration - msg.timer < 15) {
            alpha = ((msg.duration - msg.timer) * 255) / 15;
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

// UI绘制
void BossFightScene::drawUI() {
    // 绘制Boss血条（顶部中央）
    int bossBarWidth = 400;
    int bossBarX = (MAP_WIDTH * TILE_SIZE - bossBarWidth) / 2;
    drawHealthBar(bossBarX, 20, bossBarWidth, 25, bossHealth, bossMaxHealth, RGB(128, 0, 128));

    // 绘制Boss名字
    settextcolor(RGB(255, 100, 255));
    settextstyle(24, 0, _T("楷体"));
    setbkmode(TRANSPARENT);
    outtextxy(bossBarX + 150, 25, _T("魔王卡奥斯"));

    // 绘制阶段指示
    settextcolor(RGB(200, 200, 200));
    settextstyle(18, 0, _T("宋体"));

    char phaseText[50];
    sprintf_s(phaseText, "阶段: %d/3", static_cast<int>(currentPhase) + 1);
    outtextxy(bossBarX + bossBarWidth - 100, 25, phaseText);

    // 绘制玩家血条（左上角）
    drawHealthBar(20, 60, 200, 20, player->getHealth(), player->getMaxHealth(), RGB(0, 128, 255));

    // 绘制操作提示
    settextcolor(RGB(200, 200, 200));
    settextstyle(16, 0, _T("宋体"));
    outtextxy(20, MAP_HEIGHT * TILE_SIZE - 60, _T("WASD: 移动  J: 圣剑攻击"));

    // 绘制圣剑冷却指示
    if (player->hasHolySwordAbility()) {
        if (holySwordCooldown > 0) {
            settextcolor(RGB(255, 100, 100));
            char cooldownText[50];
            sprintf_s(cooldownText, "圣剑冷却: %.1f秒", holySwordCooldown / 60.0f);
            outtextxy(20, 100, cooldownText);
        }
        else {
            settextcolor(RGB(100, 255, 100));
            outtextxy(20, 100, _T("圣剑: 就绪"));
        }
    }
}

void BossFightScene::drawHealthBar(int x, int y, int width, int height, int current, int max, COLORREF color) {
    // 绘制背景
    setfillcolor(RGB(50, 50, 50));
    solidrectangle(x, y, x + width, y + height);

    // 绘制当前血量
    if (current > 0) {
        int fillWidth = width * current / max;
        setfillcolor(color);
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
    sprintf_s(healthText, "%d/%d", current, max);

    int textWidth = textwidth(healthText);
    outtextxy(x + (width - textWidth) / 2, y + 2, healthText);
}

// 工具函数
bool BossFightScene::isPositionValid(const Position& pos) const {
    return pos.x >= 0 && pos.x < MAP_WIDTH &&
        pos.y >= 0 && pos.y < MAP_HEIGHT;
}

Position BossFightScene::getRandomPositionNearPlayer(int radius) {
    if (!player) return Position(0, 0);

    Position playerPos = player->getPosition();

    for (int attempts = 0; attempts < 20; attempts++) {
        int offsetX = rand() % (2 * radius + 1) - radius;
        int offsetY = rand() % (2 * radius + 1) - radius;

        Position pos(playerPos.x + offsetX, playerPos.y + offsetY);

        if (isPositionValid(pos) && isPositionInPlayerArea(pos)) {
            return pos;
        }
    }

    return playerPos;  // 如果找不到合适位置，返回玩家位置
}