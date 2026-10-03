#pragma once
#include <SDL.h>

// 視窗大小
constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 640;

// 2D 向量
struct Vec2 {
    float x{};
    float y{};
};

inline float dist2(const Vec2& a, const Vec2& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}
