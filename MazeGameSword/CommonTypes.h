// CommonTypes.h
#pragma once
#include <graphics.h>
#include <string>

//通用位置结构体
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

//通用游戏消息结构体
struct GameMessage {
    std::string text;
    int duration;
    int timer;
    COLORREF color;
};