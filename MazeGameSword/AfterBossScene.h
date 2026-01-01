// AfterBossScene.h
#pragma once
#include "Scene.h"
#include <graphics.h>
#include <string>
#include <vector>
#include <windows.h>

// 剧情步骤类型
enum class AfterBossStepType {
    TEXT_ONLY,      // 纯文本
    IMAGE_SCENE,    // 图片场景
    DIALOGUE,       // 对话场景
    FADE_OUT,       // 淡出
    FADE_IN,        // 淡入
    CREDITS         // 制作人员名单
};

// 对话结构
struct AfterBossDialogue {
    std::string speaker;     // 说话者名字
    std::string content;     // 对话内容
};

// 剧情步骤
struct AfterBossStep {
    AfterBossStepType type;
    std::string background;  // 背景图片路径
    std::string leftImage;   // 左侧立绘路径
    std::string rightImage;  // 右侧立绘路径
    AfterBossDialogue dialogue; // 对话内容
    std::string text;        // 纯文本内容
};

// 制作人员条目
struct CreditEntry {
    std::string role;
    std::string name;
};

class AfterBossScene : public Scene {
private:
    SceneManager* sceneManager;

    // 剧情数据
    std::vector<AfterBossStep> steps;
    size_t currentStep;

    // 制作人员名单
    std::vector<CreditEntry> credits;
    int creditsY;            // 制作人员名单的Y坐标
    int creditsSpeed;        // 滚动速度

    // 图片资源
    IMAGE bgImg;            // 背景图片
    IMAGE leftImg;          // 左侧角色立绘
    IMAGE rightImg;         // 右侧角色立绘

    // 图片加载状态
    bool bgLoaded;
    bool leftLoaded;
    bool rightLoaded;

    // 淡入淡出效果
    int alphaValue;         // 透明度(0-255)
    bool isFading;
    bool fadeIn;           // true为淡入，false为淡出

    // 文本显示相关
    std::string currentText;    // 当前显示的文本
    int textCharIndex;          // 已显示字符索引
    DWORD lastTextTime;         // 上次更新文本时间
    bool textFinished;          // 文本是否显示完毕

    // 控制变量
    bool skipRequested;         // 跳过请求
    bool showCredits;          // 是否显示制作人员名单

public:
    explicit AfterBossScene(SceneManager* manager);
    ~AfterBossScene() override;

    // 实现Scene基类的纯虚函数
    void enter() override;
    void exit() override;
    void update() override;
    void render() override;
    void setSceneManager(SceneManager* manager) override { sceneManager = manager; }

private:
    // 初始化剧情步骤
    void initStorySteps();
    void initCredits();

    // 资源管理
    void loadCurrentImages();
    void unloadImages();
    void createDefaultImages();

    // 文本处理
    void startTextDisplay(const std::string& text);
    void updateTextDisplay();

    // 绘制方法
    void drawTextOnlyScene();
    void drawImageScene();
    void drawDialogueScene();
    void drawCredits();

    // 效果控制
    void startFade(bool isFadeIn);
    void updateFade();
    void drawFadeEffect();

    // 输入处理
    void handleInput();

    // 工具函数
    void drawTextWithWrap(int x, int y, int width, const std::string& text,
        COLORREF color = WHITE, int fontSize = 24,
        const wchar_t* fontName = L"宋体");
    void drawTextWithShadow(int x, int y, const std::string& text,
        COLORREF color = WHITE, COLORREF shadowColor = BLACK,
        int fontSize = 24, const wchar_t* fontName = L"宋体");
    void drawCenteredText(int y, const std::string& text,
        COLORREF color = WHITE, int fontSize = 36,
        const wchar_t* fontName = L"楷体");

    // 辅助函数
    bool fileExists(const std::string& filename);
};