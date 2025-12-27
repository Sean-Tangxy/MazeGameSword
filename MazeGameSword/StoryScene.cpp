// StoryScene.cpp
#include "StoryScene.h"
#include "SceneManager.h"
#include <graphics.h>
#include <conio.h>
#include <string>
#include <vector>
#include <windows.h>
#include <sstream>
#include <fstream>

using namespace std;

// 构造函数
StoryScene::StoryScene(SceneManager* manager)
    : sceneManager(manager),
    currentStep(0),
    bgLoaded(false),
    leftLoaded(false),
    rightLoaded(false),
    alphaValue(255),
    isFading(true),
    fadeIn(true),
    textCharIndex(0),
    lastTextTime(0),
    textFinished(false),
    skipRequested(false),
    autoMode(false) {

    initStorySteps();
}

// 析构函数
StoryScene::~StoryScene() {
    exit();
}

// 进入场景
void StoryScene::enter() {
    // 初始化为全黑淡入
    alphaValue = 255;
    isFading = true;
    fadeIn = true;

    // 重置文本状态
    currentText = "";
    textCharIndex = 0;
    textFinished = false;
    lastTextTime = GetTickCount();
    skipRequested = false;

    // 加载第一幕的图片
    loadCurrentImages();
}

// 退出场景
void StoryScene::exit() {
    unloadImages();
}

// 初始化剧情步骤
void StoryScene::initStorySteps() {
    steps.clear();

    // 第一幕：纯文本背景介绍
    StoryStep step1;
    step1.type = StoryStepType::TEXT_ONLY;
    step1.text =
        "在悠远的幻想大陆上，曾经被三位圣骑士封印的魔王「卡奥斯」\n"
        "在沉睡千年后重新苏醒。他的黑暗魔力侵蚀着大地，\n"
        "使王国陷入恐惧与绝望。\n"
        "然而，传说中有一把能够彻底消灭魔王的「圣光之剑」，\n"
        "这把剑在一次远古战争中被分解成三块碎片，\n"
        "散落在世界的三个守护之地。\n\n"
        "「艾登」，是圣骑士后裔中仅存的血脉。\n"
        "年迈的国王在城堡大厅召见了艾登，\n"
        "将这项关乎世界存亡的使命托付于他。";
    steps.push_back(step1);

    // 第二幕：城堡对话 - 第一段
    StoryStep step2;
    step2.type = StoryStepType::DIALOGUE;
    step2.background = "assets/images/bg_castle.jpg";
    step2.leftImage = "assets/images/character_eden.png";
    step2.rightImage = "assets/images/character_king.png";
    step2.dialogue.speaker = "国王";
    step2.dialogue.content =
        "艾登，我的孩子。黑暗再次笼罩大地，这次比以往更加凶猛。\n"
        "你的先祖曾用圣光之剑封印魔王，现在，这把剑是我们唯一的希望。";
    steps.push_back(step2);

    // 第三幕：城堡对话 - 第二段
    StoryStep step3;
    step3.type = StoryStepType::DIALOGUE;
    step3.background = "assets/images/bg_castle.jpg";
    step3.leftImage = "assets/images/character_eden.png";
    step3.rightImage = "assets/images/character_king.png";
    step3.dialogue.speaker = "国王";
    step3.dialogue.content =
        "三块圣剑碎片分别藏在：被遗忘的「森林迷宫」、高耸入云的「天空塔」、\n"
        "以及深埋地下的「熔岩遗迹」。每块碎片都由古老的守护者看管，\n"
        "它们只会屈服于真正勇者的心。";
    steps.push_back(step3);

    // 第四幕：城堡对话 - 第三段
    StoryStep step4;
    step4.type = StoryStepType::DIALOGUE;
    step4.background = "assets/images/bg_castle.jpg";
    step4.leftImage = "assets/images/character_eden.png";
    step4.rightImage = "assets/images/character_king.png";
    step4.dialogue.speaker = "艾登";
    step4.dialogue.content =
        "陛下，我会找回所有碎片，合成圣剑，终结这场黑暗。";
    steps.push_back(step4);

    // 第五幕：城堡对话 - 第四段
    StoryStep step5;
    step5.type = StoryStepType::DIALOGUE;
    step5.background = "assets/images/bg_castle.jpg";
    step5.leftImage = "assets/images/character_eden.png";
    step5.rightImage = "assets/images/character_king.png";
    step5.dialogue.speaker = "国王";
    step5.dialogue.content =
        "记住，艾登。真正的力量不仅来自武器，更来自你的勇气与智慧。\n"
        "愿光明指引你的道路。";
    steps.push_back(step5);
}

// 加载当前步骤的图片
void StoryScene::loadCurrentImages() {
    // 先卸载之前的图片
    unloadImages();

    if (currentStep >= steps.size()) return;

    const StoryStep& step = steps[currentStep];

    if (step.type == StoryStepType::DIALOGUE) {
        // 尝试加载背景图片
        if (!step.background.empty()) {
            // 检查文件是否存在
            ifstream file(step.background);
            if (file.good()) {
                file.close();
                loadimage(&bgImg, step.background.c_str(), 800, 600);
                bgLoaded = true;
            }
            else {
                createDefaultImages();
            }
        }

        // 尝试加载左侧立绘
        if (!step.leftImage.empty()) {
            ifstream file(step.leftImage);
            if (file.good()) {
                file.close();
                loadimage(&leftImg, step.leftImage.c_str());
                leftLoaded = true;
            }
        }

        // 尝试加载右侧立绘
        if (!step.rightImage.empty()) {
            ifstream file(step.rightImage);
            if (file.good()) {
                file.close();
                loadimage(&rightImg, step.rightImage.c_str());
                rightLoaded = true;
            }
        }
    }
}

// 创建默认图片（如果素材不存在）
void StoryScene::createDefaultImages() {
    // 创建默认城堡背景
    SetWorkingImage(&bgImg);
    Resize(NULL, 800, 600);

    // 绘制渐变背景
    for (int i = 0; i < 600; i++) {
        int r = 50 + i * 20 / 600;
        int g = 30 + i * 10 / 600;
        int b = 20 + i * 5 / 600;
        setlinecolor(RGB(r, g, b));
        line(0, i, 800, i);
    }

    // 绘制窗户
    setfillcolor(RGB(200, 180, 120));
    solidrectangle(300, 100, 500, 250);

    // 绘制地毯
    setfillcolor(RGB(139, 0, 0));
    solidrectangle(200, 300, 600, 450);

    // 绘制王座
    setfillcolor(RGB(160, 120, 80));
    solidrectangle(350, 200, 450, 300);

    SetWorkingImage();
    bgLoaded = true;

    // 创建默认角色立绘
    if (!leftLoaded) {
        SetWorkingImage(&leftImg);
        Resize(NULL, 300, 400);
        setfillcolor(RGB(30, 60, 150));  // 蓝色盔甲
        solidrectangle(0, 0, 300, 400);

        // 绘制简易角色轮廓
        setfillcolor(RGB(200, 180, 150));  // 肤色
        solidellipse(100, 50, 200, 150);   // 头部

        setfillcolor(RGB(255, 215, 0));    // 金色头发
        solidrectangle(80, 60, 220, 100);

        setlinecolor(RGB(255, 255, 255));
        setlinestyle(PS_SOLID, 3);
        line(120, 120, 180, 120);          // 眼睛
        line(150, 140, 150, 160);          // 鼻子
        arc(130, 170, 170, 190, 0, 3.14);  // 嘴巴

        SetWorkingImage();
        leftLoaded = true;
    }

    if (!rightLoaded) {
        SetWorkingImage(&rightImg);
        Resize(NULL, 300, 400);
        setfillcolor(RGB(150, 50, 30));    // 红色长袍
        solidrectangle(0, 0, 300, 400);

        // 绘制国王轮廓
        setfillcolor(RGB(200, 180, 150));  // 肤色
        solidellipse(100, 50, 200, 150);   // 头部

        setfillcolor(RGB(255, 215, 0));    // 金色王冠
        solidrectangle(80, 30, 220, 80);

        // 绘制胡子
        setfillcolor(RGB(200, 200, 200));  // 灰色胡子
        solidrectangle(120, 140, 180, 160);

        setlinecolor(RGB(0, 0, 0));
        setlinestyle(PS_SOLID, 2);
        line(120, 120, 180, 120);          // 眼睛
        line(150, 130, 150, 140);          // 鼻子

        SetWorkingImage();
        rightLoaded = true;
    }
}

// 卸载图片资源
void StoryScene::unloadImages() {
    // EasyX会自动管理IMAGE对象的释放
    bgLoaded = false;
    leftLoaded = false;
    rightLoaded = false;
}

// 开始显示文本
void StoryScene::startTextDisplay(const std::string& text) {
    currentText = text;
    textCharIndex = 0;
    textFinished = false;
    lastTextTime = GetTickCount();
}

// 更新文本显示（逐字效果）
void StoryScene::updateTextDisplay() {
    if (textFinished || skipRequested) return;

    DWORD currentTime = GetTickCount();
    if (currentTime - lastTextTime > 30) { // 30ms显示一个字，可以调整速度
        if (textCharIndex < currentText.length()) {
            textCharIndex++;
            lastTextTime = currentTime;

            // 播放打字音效（预留接口）
            // playSoundEffect("assets/sounds/type.wav");
        }
        else {
            textFinished = true;
        }
    }
}

// 更新场景
void StoryScene::update() {
    // 如果请求跳过，直接进入游戏
    if (skipRequested) {
        sceneManager->switchTo(SceneType::GAME);
        return;
    }

    // 更新淡入淡出效果
    if (isFading) {
        updateFade();
        return;
    }

    // 更新文本显示
    updateTextDisplay();

    // 处理输入
    handleInput();
}

// 渲染场景
void StoryScene::render() {
    // 先清后台缓冲区
    cleardevice();

    // 绘制淡入淡出效果
    if (isFading) {
        drawFadeEffect();
        return;
    }

    if (currentStep >= steps.size()) {
        // 剧情结束，切换到游戏场景
        sceneManager->switchTo(SceneType::GAME);
        return;
    }

    const StoryStep& step = steps[currentStep];

    switch (step.type) {
    case StoryStepType::TEXT_ONLY:
        drawTextOnlyScene();
        break;
    case StoryStepType::DIALOGUE:
        drawDialogueScene();
        break;
    default:
        break;
    }

    // 绘制操作提示
    drawTextWithShadow(600, 550, "按空格键继续", RGB(200, 200, 200), RGB(0, 0, 0), 20);
    drawTextWithShadow(50, 550, "按ESC跳过剧情", RGB(200, 200, 200), RGB(0, 0, 0), 20);
}

void StoryScene::drawTextOnlyScene() {
    // 纯黑色背景
    setbkcolor(BLACK);
    cleardevice();  // 这里也需要清屏

    // 绘制标题
    drawTextWithShadow(250, 50, "序章：使命的召唤",
        RGB(255, 215, 0), RGB(100, 80, 0), 48, L"楷体");

    // 绘制华丽的分隔线
    setlinecolor(RGB(184, 134, 11));  // 暗金色
    setlinestyle(PS_SOLID, 4);

    // 绘制双线
    line(100, 120, 700, 120);
    setlinecolor(RGB(218, 165, 32));  // 金色
    setlinestyle(PS_SOLID, 2);
    line(100, 124, 700, 124);

    // 绘制文本内容
    const StoryStep& step = steps[currentStep];
    string displayingText = currentText.substr(0, textCharIndex);
    drawTextWithWrap(100, 150, 600, displayingText,
        RGB(230, 230, 230), 28, L"宋体");

    // 绘制装饰性边框
    setlinecolor(RGB(139, 69, 19));
    setlinestyle(PS_SOLID, 2);
    rectangle(50, 40, 750, 550);
}

// 绘制对话场景
void StoryScene::drawDialogueScene() {
    // 清屏设置背景色
    setbkcolor(BLACK);
    cleardevice();

    // 绘制背景
    if (bgLoaded) {
        putimage(0, 0, &bgImg);
    }
    else {
        // 如果没有背景，绘制默认颜色
        setbkcolor(RGB(50, 30, 20));
        cleardevice();
    }

    // 绘制角色立绘
    const StoryStep& step = steps[currentStep];

    // 左侧立绘（主角艾登）
    if (leftLoaded) {
        int leftX = 50;
        int leftY = 200;  // 从200像素高度开始显示
        putimage(leftX, leftY, &leftImg);

        // 绘制角色名字标签
        setfillcolor(RGB(0, 0, 0, 180));
        solidrectangle(leftX, leftY - 40, leftX + 200, leftY - 10);
        drawTextWithShadow(leftX + 10, leftY - 35, "艾登",
            RGB(135, 206, 235), RGB(0, 0, 0), 20, L"楷体");
    }

    // 右侧立绘（国王）
    if (rightLoaded) {
        int rightX = 800 - 50 - 300;  // 距右边50像素，假设立绘宽度300
        int rightY = 200;
        putimage(rightX, rightY, &rightImg);

        // 绘制角色名字标签
        setfillcolor(RGB(0, 0, 0, 180));
        solidrectangle(rightX, rightY - 40, rightX + 200, rightY - 10);
        drawTextWithShadow(rightX + 10, rightY - 35, "国王",
            RGB(255, 215, 0), RGB(0, 0, 0), 20, L"楷体");
    }

    // 绘制对话框
    int dialogX = 50;
    int dialogY = 400;
    int dialogWidth = 700;
    int dialogHeight = 170;

    // 对话框背景（半透明黑色）
    setfillcolor(RGB(0, 0, 0, 200));
    solidrectangle(dialogX, dialogY, dialogX + dialogWidth, dialogY + dialogHeight);

    // 对话框边框（金色装饰）
    setlinecolor(RGB(218, 165, 32));  // 金色
    setlinestyle(PS_SOLID, 4);
    rectangle(dialogX, dialogY, dialogX + dialogWidth, dialogY + dialogHeight);

    // 内边框
    setlinecolor(RGB(139, 69, 19));  // 棕色
    setlinestyle(PS_SOLID, 2);
    rectangle(dialogX + 5, dialogY + 5, dialogX + dialogWidth - 5, dialogY + dialogHeight - 5);

    // 绘制说话者名字（带背景）
    setfillcolor(RGB(139, 69, 19, 220));  // 半透明棕色
    solidrectangle(dialogX + 20, dialogY + 15, dialogX + 200, dialogY + 55);

    drawTextWithShadow(dialogX + 30, dialogY + 20, step.dialogue.speaker,
        RGB(255, 255, 255), RGB(100, 50, 0), 28, L"楷体");

    // 绘制对话内容
    string displayingText = currentText.substr(0, textCharIndex);
    drawTextWithWrap(dialogX + 30, dialogY + 70, dialogWidth - 60,
        displayingText, RGB(255, 255, 255), 24, L"宋体");
}

// 绘制淡入淡出效果
void StoryScene::drawFadeEffect() {
    // 先绘制场景内容
    if (currentStep < steps.size()) {
        const StoryStep& step = steps[currentStep];
        if (step.type == StoryStepType::TEXT_ONLY) {
            drawTextOnlyScene();
        }
        else if (step.type == StoryStepType::DIALOGUE) {
            drawDialogueScene();
        }
    }

    // 在顶层绘制黑色覆盖层
    setfillcolor(RGB(0, 0, 0, alphaValue));
    solidrectangle(0, 0, 800, 600);
}

// 开始淡入淡出
void StoryScene::startFade(bool isFadeIn) {
    this->fadeIn = isFadeIn;
    isFading = true;
    alphaValue = isFadeIn ? 255 : 0;
}

// 更新淡入淡出效果
void StoryScene::updateFade() {
    if (fadeIn) {
        alphaValue -= 8;  // 淡入速度
        if (alphaValue <= 0) {
            alphaValue = 0;
            isFading = false;

            // 淡入结束后开始显示文本
            if (currentStep < steps.size()) {
                const StoryStep& step = steps[currentStep];
                if (step.type == StoryStepType::TEXT_ONLY) {
                    startTextDisplay(step.text);
                }
                else if (step.type == StoryStepType::DIALOGUE) {
                    startTextDisplay(step.dialogue.content);
                }
            }
        }
    }
    else {
        alphaValue += 8;  // 淡出速度
        if (alphaValue >= 255) {
            alphaValue = 255;
            isFading = false;

            // 淡出结束后切换到下一步
            currentStep++;
            if (currentStep < steps.size()) {
                // 加载新图片
                loadCurrentImages();
                // 开始淡入
                startFade(true);
            }
            else {
                // 剧情结束，切换到游戏场景
                sceneManager->switchTo(SceneType::GAME);
            }
        }
    }
}

// 处理输入
void StoryScene::handleInput() {
    static bool spaceProcessed = false;
    static bool escProcessed = false;

    // 空格键继续
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        if (!spaceProcessed) {
            spaceProcessed = true;

            if (!textFinished) {
                // 如果文本还没显示完，立即显示全部
                textCharIndex = (int)currentText.length();
                textFinished = true;
                // 播放完成音效
                // playSoundEffect("assets/sounds/complete.wav");
            }
            else {
                // 文本已显示完，继续下一步
                startFade(false);  // 开始淡出
            }
        }
    }
    else {
        spaceProcessed = false;
    }

    // ESC键跳过全部剧情
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        if (!escProcessed) {
            escProcessed = true;
            skipRequested = true;
            // 播放跳过音效
            // playSoundEffect("assets/sounds/skip.wav");
        }
    }
    else {
        escProcessed = false;
    }

    // 回车键也可以继续
    if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
        if (!textFinished) {
            textCharIndex = (int)currentText.length();
            textFinished = true;
        }
        else {
            startFade(false);
        }
    }
}

// 绘制自动换行文本
void StoryScene::drawTextWithWrap(int x, int y, int width, const std::string& text,
    COLORREF color, int fontSize, const wchar_t* fontName) {
    settextcolor(color);
    settextstyle(fontSize, 0, _T("宋体"));
    setbkmode(TRANSPARENT);

    // 处理换行符和自动换行
    std::string currentLine;
    int lineHeight = fontSize + 8;
    int currentY = y;

    std::istringstream iss(text);
    std::string paragraph;

    while (std::getline(iss, paragraph, '\n')) {
        std::string word;
        std::istringstream words(paragraph);
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

        // 输出段落最后一行
        if (!currentLine.empty()) {
            outtextxy(x, currentY, currentLine.c_str());
            currentY += lineHeight;
        }

        // 段落间空一行
        currentY += lineHeight / 2;
    }
}

// 绘制带阴影的文本
void StoryScene::drawTextWithShadow(int x, int y, const std::string& text,
    COLORREF color, COLORREF shadowColor,
    int fontSize, const wchar_t* fontName) {
    settextstyle(fontSize, 0, _T("宋体"));
    setbkmode(TRANSPARENT);

    // 先绘制阴影
    settextcolor(shadowColor);
    outtextxy(x + 2, y + 2, text.c_str());

    // 再绘制主文本
    settextcolor(color);
    outtextxy(x, y, text.c_str());
}

// 音乐接口实现（预留）
void StoryScene::playBackgroundMusic(const std::string& musicFile) {
    // TODO: 使用mciSendString播放背景音乐
    // mciSendString(("open \"" + musicFile + "\" alias bgm").c_str(), NULL, 0, NULL);
    // mciSendString("play bgm repeat", NULL, 0, NULL);
}

void StoryScene::stopBackgroundMusic() {
    // TODO: 停止背景音乐
    // mciSendString("stop bgm", NULL, 0, NULL);
    // mciSendString("close bgm", NULL, 0, NULL);
}

void StoryScene::playSoundEffect(const std::string& soundFile) {
    // TODO: 播放音效
    // mciSendString(("play \"" + soundFile + "\"").c_str(), NULL, 0, NULL);
}