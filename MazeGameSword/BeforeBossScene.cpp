// BeforeBossScene.cpp - BOSS前剧情场景（修复版）
#include "BeforeBossScene.h"
#include "SceneManager.h"
#include <graphics.h>
#include <conio.h>
#include <string>
#include <vector>
#include <windows.h>
#include <sstream>
#include <fstream>
#include <direct.h>  // 用于获取当前目录

using namespace std;

// 安全释放图像资源
void BeforeBossScene::safeDeleteImage(IMAGE* img) {
    if (img != nullptr) {
        // 检查图像是否有有效数据
        if (img->getwidth() > 0 || img->getheight() > 0) {
            // 创建空图像并交换
            IMAGE emptyImg;
            *img = emptyImg;
        }
        delete img;
    }
}

// 构造函数
BeforeBossScene::BeforeBossScene(SceneManager* manager)
    : sceneManager(manager),
    bgImg(nullptr),
    leftImg(nullptr),
    rightImg(nullptr),
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
    skipRequested(false) {

    initStorySteps();
}

// 析构函数
BeforeBossScene::~BeforeBossScene() {
    exit();
}

// 进入场景
void BeforeBossScene::enter() {
    // 清理旧资源
    unloadImages();

    // 创建新的图像对象
    bgImg = new IMAGE();
    leftImg = new IMAGE();
    rightImg = new IMAGE();

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
    currentStep = 0;

    // 加载第一幕的图片
    try {
        loadCurrentImages();
    }
    catch (...) {
        // 加载失败，创建默认图片
        createDefaultImages();
    }
}

// 退出场景
void BeforeBossScene::exit() {
    unloadImages();
}

// 初始化剧情步骤
void BeforeBossScene::initStorySteps() {
    steps.clear();

    // 第一幕：场景描述
    BossStoryStep step1;
    step1.type = BossStoryStepType::TEXT_ONLY;
    step1.text =
        "带着完整的圣剑，艾登来到魔王的黑暗城堡。\n"
        "这里一片死寂，只有魔王卡奥斯在王座厅等待着最终的决战。\n\n"
        "进入时剧情：\n"
        "艾登推开沉重的黑铁大门，魔王卡奥斯缓缓从王座上站起";
    steps.push_back(step1);

    // 第二幕：魔王对话
    BossStoryStep step2;
    step2.type = BossStoryStepType::DIALOGUE;
    step2.background = "assets/images/bg_dark_throne.jpg";
    step2.leftImage = "assets/images/character_eden_battle.png";
    step2.rightImage = "assets/images/character_kaos.png";
    step2.dialogue.speaker = "卡奥斯";
    step2.dialogue.content =
        "圣骑士的后裔...我等你很久了。你以为那把剑就能击败我吗？\n"
        "我已经不是千年前的那个我了！";
    steps.push_back(step2);

    // 第三幕：艾登回应
    BossStoryStep step3;
    step3.type = BossStoryStepType::DIALOGUE;
    step3.background = "assets/images/bg_dark_throne.jpg";
    step3.leftImage = "assets/images/character_eden_battle.png";
    step3.rightImage = "assets/images/character_kaos.png";
    step3.dialogue.speaker = "艾登";
    step3.dialogue.content =
        "你的黑暗时代该结束了，卡奥斯。\n"
        "今天，我要完成先祖未竟的使命！";
    steps.push_back(step3);

    // 第四幕：魔王最终挑战
    BossStoryStep step4;
    step4.type = BossStoryStepType::DIALOGUE;
    step4.background = "assets/images/bg_dark_throne.jpg";
    step4.leftImage = "assets/images/character_eden_battle.png";
    step4.rightImage = "assets/images/character_kaos.png";
    step4.dialogue.speaker = "卡奥斯";
    step4.dialogue.content =
        "可笑！那就让我看看，你的「光明」能否照亮这无尽的黑暗！";
    steps.push_back(step4);
}

// 尝试加载图片的辅助函数
bool BeforeBossScene::tryLoadImage(const std::string& path, IMAGE** img, int width, int height) {
    if (path.empty() || img == nullptr) return false;

    // 检查文件是否存在
    ifstream file(path.c_str());
    if (!file.good()) {
        // 尝试在当前目录下查找
        char buffer[256];
        _getcwd(buffer, 256);
        std::string currentDir = buffer;
        std::string fullPath = currentDir + "\\" + path;

        file.open(fullPath.c_str());
        if (!file.good()) {
            return false;
        }
    }
    file.close();

    // 清理旧图像
    safeDeleteImage(*img);
    *img = new IMAGE();

    // 尝试加载图片
    try {
        if (width > 0 && height > 0) {
            if (loadimage(*img, path.c_str(), width, height)) {
                return true;
            }
        }
        else {
            if (loadimage(*img, path.c_str())) {
                return true;
            }
        }
    }
    catch (...) {
        // 加载失败，清理资源
        safeDeleteImage(*img);
        *img = nullptr;
        return false;
    }

    // 加载失败，清理资源
    safeDeleteImage(*img);
    *img = nullptr;
    return false;
}

// 加载当前步骤的图片
void BeforeBossScene::loadCurrentImages() {
    // 重置加载状态
    bgLoaded = false;
    leftLoaded = false;
    rightLoaded = false;

    if (currentStep >= steps.size()) return;

    const BossStoryStep& step = steps[currentStep];

    if (step.type == BossStoryStepType::DIALOGUE) {
        // 尝试加载背景图片
        if (!step.background.empty() && bgImg != nullptr) {
            if (tryLoadImage(step.background, &bgImg, 800, 600)) {
                bgLoaded = true;
            }
        }

        // 尝试加载左侧立绘
        if (!step.leftImage.empty() && leftImg != nullptr) {
            if (tryLoadImage(step.leftImage, &leftImg)) {
                leftLoaded = true;
            }
        }

        // 尝试加载右侧立绘
        if (!step.rightImage.empty() && rightImg != nullptr) {
            if (tryLoadImage(step.rightImage, &rightImg)) {
                rightLoaded = true;
            }
        }

        // 如果有任何图片加载失败，创建默认图片
        if (!bgLoaded || !leftLoaded || !rightLoaded) {
            createDefaultImages();
        }
    }
}

// 创建默认图片 - 修复版
void BeforeBossScene::createDefaultImages() {
    // 清理旧图像
    safeDeleteImage(bgImg);
    safeDeleteImage(leftImg);
    safeDeleteImage(rightImg);

    // 创建新图像
    bgImg = new IMAGE();
    leftImg = new IMAGE();
    rightImg = new IMAGE();

    // 1. 创建默认背景
    Resize(bgImg, 800, 600);
    SetWorkingImage(bgImg);

    // 绘制简单的渐变色背景
    for (int y = 0; y < 600; y++) {
        int r = 10 + y * 5 / 600;
        int g = 10 + y * 3 / 600;
        int b = 20 + y * 2 / 600;
        setlinecolor(RGB(r, g, b));
        line(0, y, 800, y);
    }

    // 简单的王座
    setfillcolor(RGB(40, 40, 40));
    solidrectangle(300, 100, 500, 400);

    SetWorkingImage();  // 恢复工作图像
    bgLoaded = true;

    // 2. 创建默认左侧立绘
    Resize(leftImg, 250, 350);
    SetWorkingImage(leftImg);

    // 绘制简单的蓝色矩形代表艾登
    setfillcolor(RGB(30, 60, 150));
    solidrectangle(0, 0, 250, 350);

    SetWorkingImage();  // 恢复工作图像
    leftLoaded = true;

    // 3. 创建默认右侧立绘
    Resize(rightImg, 300, 400);
    SetWorkingImage(rightImg);

    // 绘制简单的紫色矩形代表魔王
    setfillcolor(RGB(50, 0, 50));
    solidrectangle(0, 0, 300, 400);

    SetWorkingImage();  // 恢复工作图像
    rightLoaded = true;
}

// 卸载图片资源
void BeforeBossScene::unloadImages() {
    // 安全释放所有图像资源
    safeDeleteImage(bgImg);
    safeDeleteImage(leftImg);
    safeDeleteImage(rightImg);

    bgImg = nullptr;
    leftImg = nullptr;
    rightImg = nullptr;

    // 清除加载状态
    bgLoaded = false;
    leftLoaded = false;
    rightLoaded = false;
}

// 开始显示文本
void BeforeBossScene::startTextDisplay(const std::string& text) {
    currentText = text;
    textCharIndex = 0;
    textFinished = false;
    lastTextTime = GetTickCount();
}

// 更新文本显示
void BeforeBossScene::updateTextDisplay() {
    if (textFinished || skipRequested) return;

    DWORD currentTime = GetTickCount();
    if (currentTime - lastTextTime > 30) { // 30ms显示一个字
        if (textCharIndex < currentText.length()) {
            textCharIndex++;
            lastTextTime = currentTime;
        }
        else {
            textFinished = true;
        }
    }
}

// 更新场景
void BeforeBossScene::update() {
    // 如果请求跳过，直接进入Boss战
    if (skipRequested) {
        sceneManager->switchTo(SceneType::BOSS);
        return;
    }

    // 检查步骤索引有效性
    if (currentStep >= steps.size()) {
        sceneManager->switchTo(SceneType::BOSS);
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
void BeforeBossScene::render() {
    // 先清后台缓冲区
    cleardevice();

    // 绘制淡入淡出效果
    if (isFading) {
        drawFadeEffect();
        return;
    }

    if (currentStep >= steps.size()) {
        // 剧情结束，切换到Boss战
        sceneManager->switchTo(SceneType::BOSS);
        return;
    }

    const BossStoryStep& step = steps[currentStep];

    switch (step.type) {
    case BossStoryStepType::TEXT_ONLY:
        drawTextOnlyScene();
        break;
    case BossStoryStepType::DIALOGUE:
        drawDialogueScene();
        break;
    default:
        break;
    }

    // 绘制操作提示
    drawTextWithShadow(600, 550, "按空格键继续", RGB(200, 200, 200), RGB(0, 0, 0), 20);
    drawTextWithShadow(50, 550, "按ESC跳过", RGB(200, 200, 200), RGB(0, 0, 0), 20);
}

// 绘制纯文本场景
void BeforeBossScene::drawTextOnlyScene() {
    // 深色背景
    setbkcolor(RGB(10, 10, 30));
    cleardevice();

    // 绘制标题
    drawTextWithShadow(250, 50, "最终章：黑暗王座",
        RGB(255, 100, 100), RGB(50, 0, 0), 48, L"楷体");

    // 绘制华丽的分隔线
    setlinecolor(RGB(139, 0, 139));  // 深紫色
    setlinestyle(PS_SOLID, 4);
    line(100, 120, 700, 120);

    setlinecolor(RGB(255, 0, 255));  // 亮紫色
    setlinestyle(PS_SOLID, 2);
    line(100, 124, 700, 124);

    // 绘制文本内容
    const BossStoryStep& step = steps[currentStep];
    string displayingText = currentText.substr(0, textCharIndex);
    drawTextWithWrap(100, 150, 600, displayingText,
        RGB(230, 230, 255), 28, L"宋体");

    // 绘制装饰性边框
    setlinecolor(RGB(75, 0, 130));
    setlinestyle(PS_SOLID, 2);
    rectangle(50, 40, 750, 550);
}

// 绘制对话场景
void BeforeBossScene::drawDialogueScene() {
    // 清屏设置背景色
    setbkcolor(BLACK);
    cleardevice();

    // 绘制背景
    if (bgLoaded && bgImg != nullptr && bgImg->getwidth() > 0) {
        putimage(0, 0, bgImg);
    }
    else {
        // 如果没有背景，绘制默认颜色
        setbkcolor(RGB(20, 10, 30));
        cleardevice();
    }

    // 绘制角色立绘
    const BossStoryStep& step = steps[currentStep];

    // 左侧立绘（艾登）
    if (leftLoaded && leftImg != nullptr && leftImg->getwidth() > 0) {
        int leftX = 50;
        int leftY = 200;
        putimage(leftX, leftY, leftImg);

        // 绘制角色名字标签
        setfillcolor(RGB(0, 0, 0, 180));
        solidrectangle(leftX, leftY - 40, leftX + 200, leftY - 10);
        drawTextWithShadow(leftX + 10, leftY - 35, "艾登",
            RGB(135, 206, 235), RGB(0, 0, 0), 20, L"楷体");
    }
    else {
        // 如果没有立绘，绘制默认的艾登
        int leftX = 50;
        int leftY = 200;
        setfillcolor(RGB(30, 60, 150));
        solidrectangle(leftX, leftY, leftX + 250, leftY + 350);
    }

    // 右侧立绘（魔王）
    if (rightLoaded && rightImg != nullptr && rightImg->getwidth() > 0) {
        int rightX = 800 - 50 - 300;
        int rightY = 180;
        putimage(rightX, rightY, rightImg);

        // 绘制角色名字标签
        setfillcolor(RGB(0, 0, 0, 180));
        solidrectangle(rightX, rightY - 40, rightX + 200, rightY - 10);
        drawTextWithShadow(rightX + 10, rightY - 35, "卡奥斯",
            RGB(255, 0, 0), RGB(0, 0, 0), 20, L"楷体");
    }
    else {
        // 如果没有立绘，绘制默认的魔王
        int rightX = 800 - 50 - 300;
        int rightY = 180;
        setfillcolor(RGB(50, 0, 50));
        solidrectangle(rightX, rightY, rightX + 300, rightY + 400);
    }

    // 绘制对话框
    int dialogX = 50;
    int dialogY = 400;
    int dialogWidth = 700;
    int dialogHeight = 170;

    // 对话框背景（半透明黑色）
    setfillcolor(RGB(0, 0, 0, 200));
    solidrectangle(dialogX, dialogY, dialogX + dialogWidth, dialogY + dialogHeight);

    // 对话框边框（紫色装饰）
    setlinecolor(RGB(128, 0, 128));  // 紫色
    setlinestyle(PS_SOLID, 4);
    rectangle(dialogX, dialogY, dialogX + dialogWidth, dialogY + dialogHeight);

    // 内边框
    setlinecolor(RGB(75, 0, 130));  // 靛青色
    setlinestyle(PS_SOLID, 2);
    rectangle(dialogX + 5, dialogY + 5, dialogX + dialogWidth - 5, dialogY + dialogHeight - 5);

    // 绘制说话者名字（带背景）
    setfillcolor(RGB(75, 0, 130, 220));  // 半透明靛青色
    solidrectangle(dialogX + 20, dialogY + 15, dialogX + 200, dialogY + 55);

    drawTextWithShadow(dialogX + 30, dialogY + 20, step.dialogue.speaker,
        RGB(255, 255, 255), RGB(50, 0, 100), 28, L"楷体");

    // 绘制对话内容
    string displayingText = currentText.substr(0, textCharIndex);
    drawTextWithWrap(dialogX + 30, dialogY + 70, dialogWidth - 60,
        displayingText, RGB(255, 255, 255), 24, L"宋体");
}

// 绘制淡入淡出效果
void BeforeBossScene::drawFadeEffect() {
    // 先绘制场景内容
    if (currentStep < steps.size()) {
        const BossStoryStep& step = steps[currentStep];
        if (step.type == BossStoryStepType::TEXT_ONLY) {
            drawTextOnlyScene();
        }
        else if (step.type == BossStoryStepType::DIALOGUE) {
            drawDialogueScene();
        }
    }

    // 在顶层绘制黑色覆盖层
    setfillcolor(RGB(0, 0, 0, alphaValue));
    solidrectangle(0, 0, 800, 600);
}

// 开始淡入淡出
void BeforeBossScene::startFade(bool isFadeIn) {
    this->fadeIn = isFadeIn;
    isFading = true;
    alphaValue = isFadeIn ? 255 : 0;
}

// 更新淡入淡出效果
void BeforeBossScene::updateFade() {
    // 安全检查：确保步骤索引有效
    if (currentStep >= steps.size()) {
        sceneManager->switchTo(SceneType::BOSS);
        return;
    }

    if (fadeIn) {
        alphaValue -= 8;  // 淡入速度
        if (alphaValue <= 0) {
            alphaValue = 0;
            isFading = false;

            // 淡入结束后开始显示文本
            if (currentStep < steps.size()) {
                const BossStoryStep& step = steps[currentStep];
                if (step.type == BossStoryStepType::TEXT_ONLY) {
                    if (!step.text.empty()) {
                        startTextDisplay(step.text);
                    }
                }
                else if (step.type == BossStoryStepType::DIALOGUE) {
                    if (!step.dialogue.content.empty()) {
                        startTextDisplay(step.dialogue.content);
                    }
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
                try {
                    loadCurrentImages();
                    startFade(true);
                }
                catch (...) {
                    // 加载失败，直接进入Boss战
                    sceneManager->switchTo(SceneType::BOSS);
                }
            }
            else {
                // 剧情结束，切换到Boss战
                sceneManager->switchTo(SceneType::BOSS);
            }
        }
    }
}

// 处理输入
void BeforeBossScene::handleInput() {
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
void BeforeBossScene::drawTextWithWrap(int x, int y, int width, const std::string& text,
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
void BeforeBossScene::drawTextWithShadow(int x, int y, const std::string& text,
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