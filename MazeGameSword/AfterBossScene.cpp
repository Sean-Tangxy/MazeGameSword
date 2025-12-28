// AfterBossScene.cpp
#include "AfterBossScene.h"
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
AfterBossScene::AfterBossScene(SceneManager* manager)
    : sceneManager(manager),
    currentStep(0),
    creditsY(650),  // 从屏幕下方开始
    creditsSpeed(1),
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
    showCredits(false) {

    initStorySteps();
    initCredits();
}

// 析构函数
AfterBossScene::~AfterBossScene() {
    exit();
}

// 进入场景
void AfterBossScene::enter() {
    // 初始化为全黑淡入
    alphaValue = 255;
    isFading = true;
    fadeIn = true;

    // 重置状态
    currentStep = 0;
    creditsY = 650;
    showCredits = false;

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
void AfterBossScene::exit() {
    unloadImages();
}

// 初始化剧情步骤
void AfterBossScene::initStorySteps() {
    steps.clear();

    // 第一幕：场景一描述
    AfterBossStep step1;
    step1.type = AfterBossStepType::TEXT_ONLY;
    step1.text =
        "魔王在圣光中痛苦地消散\n\n"
        "卡奥斯（消散前）：不...不可能...这光...为什么...如此温暖...\n\n"
        "魔王彻底消失，黑暗随之退散，阳光透过破碎的窗户照入王座厅";
    steps.push_back(step1);

    // 第二幕：场景一图片
    AfterBossStep step2;
    step2.type = AfterBossStepType::IMAGE_SCENE;
    step2.background = "assets/images/bg_after_boss1.jpg";
    step2.text = "阳光重新照耀大地，黑暗生物化为尘土，植物开始复苏";
    steps.push_back(step2);

    // 第三幕：场景二对话
    AfterBossStep step3;
    step3.type = AfterBossStepType::DIALOGUE;
    step3.background = "assets/images/bg_castle_hall.jpg";
    step3.leftImage = "assets/images/character_eden_honor.png";
    step3.rightImage = "assets/images/character_king_happy.png";
    step3.dialogue.speaker = "国王";
    step3.dialogue.content =
        "你做到了，艾登！王国...不，整个世界都欠你一份恩情。";
    steps.push_back(step3);

    // 第四幕：场景二对话
    AfterBossStep step4;
    step4.type = AfterBossStepType::DIALOGUE;
    step4.background = "assets/images/bg_castle_hall.jpg";
    step4.leftImage = "assets/images/character_eden_honor.png";
    step4.rightImage = "assets/images/character_king_happy.png";
    step4.dialogue.speaker = "艾登";
    step4.dialogue.content =
        "我只是完成了先祖的使命，陛下。";
    steps.push_back(step4);

    // 第五幕：场景二对话
    AfterBossStep step5;
    step5.type = AfterBossStepType::DIALOGUE;
    step5.background = "assets/images/bg_castle_hall.jpg";
    step5.leftImage = "assets/images/character_eden_honor.png";
    step5.rightImage = "assets/images/character_king_happy.png";
    step5.dialogue.speaker = "国王";
    step5.dialogue.content =
        "不，你完成了他们未能完成的——彻底消灭了黑暗。\n"
        "从今天起，你不再是单纯的勇者，而是这个时代的守护者。";
    steps.push_back(step5);

    // 第六幕：画面展示描述
    AfterBossStep step6;
    step6.type = AfterBossStepType::IMAGE_SCENE;
    step6.background = "assets/images/bg_ending_scene.jpg";
    step6.text =
        "【画面展示】\n"
        "艾登站在城堡高处，俯瞰重建的王国\n"
        "三处守护之地恢复正常，守护者们向城堡方向致敬\n"
        "圣剑被安放在王座旁，作为光明时代的象征";
    steps.push_back(step6);

    // 第七幕：最终文字
    AfterBossStep step7;
    step7.type = AfterBossStepType::TEXT_ONLY;
    step7.text =
        "黑暗终会退散，但勇气与希望的传承永不熄灭。\n"
        "真正的和平不是没有敌人，而是拥有守护所爱之物的力量。";
    steps.push_back(step7);

    // 第八幕：制作人员名单
    AfterBossStep step8;
    step8.type = AfterBossStepType::CREDITS;
    step8.text = "制作人员名单";
    steps.push_back(step8);
}

// 初始化制作人员名单
void AfterBossScene::initCredits() {
    credits.clear();

    credits.push_back({ "", "THE END" });
    credits.push_back({ "", "" });
    credits.push_back({ "", "感谢您的游玩" });
    credits.push_back({ "", "" });
    credits.push_back({ "制作人", "txy" });
    credits.push_back({ "制作人", "cyy" });
    credits.push_back({ "", "" });
    credits.push_back({ "程序", "txy & cyy" });
    credits.push_back({ "美术", "txx & cyy" });
    credits.push_back({ "策划", "txy & cyy" });
    credits.push_back({ "", "" });
    credits.push_back({ "音乐", "背景音乐来源网络" });
    credits.push_back({ "音效", "效果音来源网络" });
    credits.push_back({ "", "" });
    credits.push_back({ "特别感谢", "所有测试玩家" });
    credits.push_back({ "特别感谢", "EasyX图形库" });
    credits.push_back({ "", "" });
    credits.push_back({ "", "© 2025?6 勇者斗魔王开发组" });
    credits.push_back({ "", "All Rights Reserved" });
}

// 加载当前步骤的图片
void AfterBossScene::loadCurrentImages() {
    // 先卸载之前的图片
    unloadImages();

    if (currentStep >= steps.size()) return;

    const AfterBossStep& step = steps[currentStep];

    if (step.type == AfterBossStepType::IMAGE_SCENE ||
        step.type == AfterBossStepType::DIALOGUE) {

        // 尝试加载背景图片
        if (!step.background.empty()) {
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

        // 对话场景需要加载立绘
        if (step.type == AfterBossStepType::DIALOGUE) {
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
}

// 创建默认图片
void AfterBossScene::createDefaultImages() {
    // 创建默认胜利背景
    SetWorkingImage(&bgImg);
    Resize(NULL, 800, 600);

    // 绘制金色胜利背景
    for (int i = 0; i < 600; i++) {
        int r = 255 - i * 100 / 600;
        int g = 215 - i * 100 / 600;
        int b = 0 + i * 50 / 600;
        setlinecolor(RGB(r, g, b));
        line(0, i, 800, i);
    }

    // 绘制阳光
    setfillcolor(RGB(255, 255, 200, 100));
    for (int i = 0; i < 8; i++) {
        int angle = i * 45;
        int length = 300;
        int centerX = 400, centerY = 300;

        int endX = centerX + static_cast<int>(cos(angle * 3.14159 / 180) * length);
        int endY = centerY + static_cast<int>(sin(angle * 3.14159 / 180) * length);

        setlinestyle(PS_SOLID, 20);
        setlinecolor(RGB(255, 255, 150, 80));
        line(centerX, centerY, endX, endY);
    }

    // 绘制城堡轮廓
    setfillcolor(RGB(150, 100, 50));
    solidrectangle(300, 200, 500, 400);

    // 绘制旗帜
    setfillcolor(RGB(255, 0, 0));
    solidrectangle(390, 150, 410, 200);
    setfillcolor(RGB(255, 255, 255));
    solidrectangle(395, 155, 405, 195);

    SetWorkingImage();
    bgLoaded = true;

    // 创建默认荣誉版艾登立绘
    if (!leftLoaded) {
        SetWorkingImage(&leftImg);
        Resize(NULL, 250, 350);
        setfillcolor(RGB(218, 165, 32));  // 金色盔甲
        solidrectangle(0, 0, 250, 350);

        // 绘制荣誉服装
        setfillcolor(RGB(255, 255, 255));  // 白色披风
        solidrectangle(50, 100, 200, 300);

        setfillcolor(RGB(200, 180, 150));  // 肤色
        solidellipse(75, 50, 175, 150);   // 头部

        setfillcolor(RGB(255, 215, 0));    // 金色头发
        solidrectangle(60, 60, 190, 100);

        // 微笑表情
        setlinecolor(RGB(0, 0, 0));
        setlinestyle(PS_SOLID, 3);
        line(100, 120, 120, 120);          // 左眼
        line(150, 120, 170, 120);          // 右眼

        // 微笑的嘴巴
        arc(130, 150, 140, 160, 0, 180);

        SetWorkingImage();
        leftLoaded = true;
    }

    if (!rightLoaded) {
        SetWorkingImage(&rightImg);
        Resize(NULL, 250, 350);
        setfillcolor(RGB(139, 69, 19));    // 棕色国王袍
        solidrectangle(0, 0, 250, 350);

        // 绘制国王
        setfillcolor(RGB(200, 180, 150));  // 肤色
        solidellipse(75, 50, 175, 150);   // 头部

        // 王冠
        setfillcolor(RGB(255, 215, 0));
        POINT crown[7] = {
            {100, 40}, {125, 20}, {150, 40},
            {175, 20}, {200, 40}, {200, 60},
            {100, 60}
        };
        solidpolygon(crown, 7);

        // 喜悦表情
        setlinecolor(RGB(0, 0, 0));
        setlinestyle(PS_SOLID, 3);
        line(100, 120, 110, 130);          // 左眼（眯起）
        line(150, 120, 160, 130);          // 右眼（眯起）

        // 大笑的嘴巴
        arc(120, 160, 160, 180, 180, 360);

        SetWorkingImage();
        rightLoaded = true;
    }
}

// 卸载图片资源
void AfterBossScene::unloadImages() {
    bgLoaded = false;
    leftLoaded = false;
    rightLoaded = false;
}

// 开始显示文本
void AfterBossScene::startTextDisplay(const std::string& text) {
    currentText = text;
    textCharIndex = 0;
    textFinished = false;
    lastTextTime = GetTickCount();
}

// 更新文本显示
void AfterBossScene::updateTextDisplay() {
    if (textFinished || skipRequested) return;

    DWORD currentTime = GetTickCount();
    if (currentTime - lastTextTime > 40) { // 40ms显示一个字，稍慢一些
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
void AfterBossScene::update() {
    // 如果请求跳过，直接结束
    if (skipRequested) {
        sceneManager->switchTo(SceneType::MENU);
        return;
    }

    // 更新淡入淡出效果
    if (isFading) {
        updateFade();
        return;
    }

    // 如果正在显示制作人员名单
    if (showCredits) {
        // 滚动制作人员名单
        creditsY -= creditsSpeed;

        // 如果名单完全滚出屏幕，返回菜单
        if (creditsY < -static_cast<int>(credits.size() * 60)) {
            sceneManager->switchTo(SceneType::MENU);
            return;
        }
    }
    else {
        // 更新文本显示
        updateTextDisplay();
    }

    // 处理输入
    handleInput();
}

// 渲染场景
void AfterBossScene::render() {
    // 先清后台缓冲区
    cleardevice();

    // 绘制淡入淡出效果
    if (isFading) {
        drawFadeEffect();
        return;
    }

    if (currentStep >= steps.size()) {
        // 所有步骤结束
        sceneManager->switchTo(SceneType::MENU);
        return;
    }

    const AfterBossStep& step = steps[currentStep];

    switch (step.type) {
    case AfterBossStepType::TEXT_ONLY:
        drawTextOnlyScene();
        break;
    case AfterBossStepType::IMAGE_SCENE:
        drawImageScene();
        break;
    case AfterBossStepType::DIALOGUE:
        drawDialogueScene();
        break;
    case AfterBossStepType::CREDITS:
        drawCredits();
        break;
    default:
        break;
    }

    // 如果不是制作人员名单，绘制操作提示
    if (!showCredits && step.type != AfterBossStepType::CREDITS) {
        drawTextWithShadow(600, 550, "按空格键继续", RGB(200, 200, 200), RGB(0, 0, 0), 20);
        drawTextWithShadow(50, 550, "按ESC跳过", RGB(200, 200, 200), RGB(0, 0, 0), 20);
    }
}

// 绘制纯文本场景
void AfterBossScene::drawTextOnlyScene() {
    const AfterBossStep& step = steps[currentStep];

    // 根据步骤选择背景色
    if (currentStep == 6) { // 最终文字步骤
        setbkcolor(BLACK);
        cleardevice();

        // 绘制居中文字
        string displayingText = currentText.substr(0, textCharIndex);

        // 分割文本行
        vector<string> lines;
        istringstream iss(displayingText);
        string line;

        while (getline(iss, line, '\n')) {
            lines.push_back(line);
        }

        // 居中显示
        settextcolor(RGB(255, 255, 200));
        settextstyle(32, 0, _T("楷体"));
        setbkmode(TRANSPARENT);

        int startY = 200;
        int lineHeight = 50;

        for (size_t i = 0; i < lines.size(); i++) {
            int textWidth = textwidth(lines[i].c_str());
            outtextxy(400 - textWidth / 2, startY + i * lineHeight, lines[i].c_str());
        }
    }
    else {
        // 正常文本场景
        setbkcolor(RGB(20, 30, 50));
        cleardevice();

        // 绘制标题
        if (currentStep == 0) {
            drawTextWithShadow(280, 50, "光明胜利",
                RGB(255, 215, 0), RGB(100, 80, 0), 48, L"楷体");
        }

        // 绘制文本内容
        string displayingText = currentText.substr(0, textCharIndex);
        drawTextWithWrap(100, 150, 600, displayingText,
            RGB(230, 230, 255), 28, L"宋体");

        // 绘制装饰性边框
        setlinecolor(RGB(100, 150, 255));
        setlinestyle(PS_SOLID, 2);
        rectangle(50, 40, 750, 550);
    }
}

// 绘制图片场景
void AfterBossScene::drawImageScene() {
    // 绘制背景
    if (bgLoaded) {
        putimage(0, 0, &bgImg);
    }
    else {
        setbkcolor(RGB(50, 100, 150));
        cleardevice();
    }

    // 绘制描述文字（底部）
    const AfterBossStep& step = steps[currentStep];
    if (!step.text.empty()) {
        // 绘制半透明背景
        setfillcolor(RGB(0, 0, 0, 150));
        solidrectangle(50, 450, 750, 550);

        // 绘制文字
        string displayingText = currentText.substr(0, textCharIndex);
        drawTextWithWrap(60, 460, 680, displayingText,
            RGB(255, 255, 200), 20, L"宋体");
    }
}

// 绘制对话场景
void AfterBossScene::drawDialogueScene() {
    // 清屏设置背景色
    setbkcolor(BLACK);
    cleardevice();

    // 绘制背景
    if (bgLoaded) {
        putimage(0, 0, &bgImg);
    }
    else {
        setbkcolor(RGB(100, 80, 60));
        cleardevice();
    }

    // 绘制角色立绘
    const AfterBossStep& step = steps[currentStep];

    // 左侧立绘（艾登）
    if (leftLoaded) {
        int leftX = 50;
        int leftY = 200;
        putimage(leftX, leftY, &leftImg);

        // 绘制角色名字标签
        setfillcolor(RGB(0, 0, 0, 180));
        solidrectangle(leftX, leftY - 40, leftX + 200, leftY - 10);
        drawTextWithShadow(leftX + 10, leftY - 35, "艾登",
            RGB(218, 165, 32), RGB(0, 0, 0), 20, L"楷体");
    }

    // 右侧立绘（国王）
    if (rightLoaded) {
        int rightX = 800 - 50 - 250;
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

// 绘制制作人员名单
void AfterBossScene::drawCredits() {
    if (!showCredits) {
        // 开始显示制作人员名单
        showCredits = true;
        creditsY = 650;
        return;
    }

    // 黑色背景
    setbkcolor(BLACK);
    cleardevice();

    // 绘制标题
    drawCenteredText(100, "制作人员名单", RGB(255, 215, 0), 48);

    // 绘制制作人员名单
    int y = creditsY;
    for (size_t i = 0; i < credits.size(); i++) {
        const CreditEntry& entry = credits[i];

        if (y >= 0 && y < 600) {
            if (entry.role.empty() && entry.name.empty()) {
                // 空行
                y += 30;
                continue;
            }

            if (entry.role.empty()) {
                // 只有名字（标题）
                settextcolor(RGB(255, 255, 255));
                settextstyle(36, 0, _T("楷体"));
                setbkmode(TRANSPARENT);

                int textWidth = textwidth(entry.name.c_str());
                outtextxy(400 - textWidth / 2, y, entry.name.c_str());
                y += 60;
            }
            else {
                // 角色和名字
                settextcolor(RGB(200, 200, 200));
                settextstyle(24, 0, _T("宋体"));

                int roleWidth = textwidth(entry.role.c_str());
                outtextxy(300 - roleWidth, y, entry.role.c_str());

                settextcolor(RGB(255, 255, 255));
                settextstyle(28, 0, _T("楷体"));
                outtextxy(350, y - 2, entry.name.c_str());

                y += 40;
            }
        }
        else {
            y += 40;
        }
    }

    // 绘制提示
    if (creditsY < 200) {
        settextcolor(RGB(150, 150, 150));
        settextstyle(18, 0, _T("宋体"));
        outtextxy(300, 550, _T("制作人员名单滚动中..."));
    }
}

// 绘制居中文字
void AfterBossScene::drawCenteredText(int y, const std::string& text,
    COLORREF color, int fontSize, const wchar_t* fontName) {

    settextcolor(color);
    settextstyle(fontSize, 0, _T("楷体"));
    setbkmode(TRANSPARENT);

    int textWidth = textwidth(text.c_str());
    outtextxy(400 - textWidth / 2, y, text.c_str());
}

// 绘制淡入淡出效果
void AfterBossScene::drawFadeEffect() {
    // 先绘制场景内容
    if (currentStep < steps.size()) {
        const AfterBossStep& step = steps[currentStep];
        switch (step.type) {
        case AfterBossStepType::TEXT_ONLY:
            drawTextOnlyScene();
            break;
        case AfterBossStepType::IMAGE_SCENE:
            drawImageScene();
            break;
        case AfterBossStepType::DIALOGUE:
            drawDialogueScene();
            break;
        default:
            break;
        }
    }

    // 在顶层绘制黑色覆盖层
    setfillcolor(RGB(0, 0, 0, alphaValue));
    solidrectangle(0, 0, 800, 600);
}

// 开始淡入淡出
void AfterBossScene::startFade(bool isFadeIn) {
    this->fadeIn = isFadeIn;
    isFading = true;
    alphaValue = isFadeIn ? 255 : 0;
}

// 更新淡入淡出效果
void AfterBossScene::updateFade() {
    if (fadeIn) {
        alphaValue -= 6;  // 淡入速度
        if (alphaValue <= 0) {
            alphaValue = 0;
            isFading = false;

            // 淡入结束后开始显示文本
            if (currentStep < steps.size()) {
                const AfterBossStep& step = steps[currentStep];
                if (step.type == AfterBossStepType::TEXT_ONLY ||
                    step.type == AfterBossStepType::IMAGE_SCENE) {
                    startTextDisplay(step.text);
                }
                else if (step.type == AfterBossStepType::DIALOGUE) {
                    startTextDisplay(step.dialogue.content);
                }
            }
        }
    }
    else {
        alphaValue += 6;  // 淡出速度
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
        }
    }
}

// 处理输入
void AfterBossScene::handleInput() {
    static bool spaceProcessed = false;
    static bool escProcessed = false;

    // 空格键继续
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        if (!spaceProcessed) {
            spaceProcessed = true;

            if (!textFinished && !showCredits) {
                // 如果文本还没显示完，立即显示全部
                textCharIndex = (int)currentText.length();
                textFinished = true;
            }
            else if (!showCredits) {
                // 文本已显示完，继续下一步
                startFade(false);
            }
        }
    }
    else {
        spaceProcessed = false;
    }

    // ESC键跳过
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        if (!escProcessed) {
            escProcessed = true;
            if (showCredits) {
                // 跳过制作人员名单
                sceneManager->switchTo(SceneType::MENU);
            }
            else {
                skipRequested = true;
            }
        }
    }
    else {
        escProcessed = false;
    }
}

// 绘制自动换行文本
void AfterBossScene::drawTextWithWrap(int x, int y, int width, const std::string& text,
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
void AfterBossScene::drawTextWithShadow(int x, int y, const std::string& text,
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