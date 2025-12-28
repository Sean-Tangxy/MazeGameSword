// main.cpp - 游戏入口点
#include <graphics.h>
#include <conio.h>
#include "SceneManager.h"
#include <ctime>

int main() {
    // 初始化图形窗口：800x600分辨率
    initgraph(800, 600);
    // 设置双缓冲模式
    setbkmode(TRANSPARENT);  // 设置文字背景透明
    
    // 创建场景管理器
    SceneManager sceneManager;
    
    // 启用双缓冲
    BeginBatchDraw();
    
    // 游戏主循环
    while (true) {
        // 清屏
        cleardevice();
        
        // 处理当前场景
        sceneManager.render();
        sceneManager.update();
        
        // 检查退出条件
        static bool escProcessed = false;
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            if (!escProcessed) {
//                sceneManager.switchTo(SceneType::MENU);
                // 可以在这里添加返回菜单的逻辑
                escProcessed = true;
            }
        } else {
            escProcessed = false;
        }
        
        // 批量绘制
        FlushBatchDraw();
        
        // 延时控制帧率
        Sleep(16);
    }
    
    // 结束双缓冲
    EndBatchDraw();
    
    // 关闭图形窗口
    closegraph();
    return 0;
}