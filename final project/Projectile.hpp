#pragma once
#include <SDL.h>
#include <vector>
#include "Constants.hpp" 
#include "Enemy.hpp"

enum class LaserStage {
    Charge,
    Beam,
    Fade
};
class Projectile {
public:
    Projectile(const Vec2& start,
        Enemy* target,
        int dmg,
        bool splash,
        bool slow,
        float slowFactor,
        float slowDuration);

    void update(float dt, std::vector<Enemy*>& enemies);
    void render(SDL_Renderer* renderer) const;

    bool isDead() const { return dead; }

    // 雷射專用
    bool isLaser = false;
    float life = 0.2f;   // 雷射存活 0.1 秒用來顯示動畫
    Vec2 startPos;
    LaserStage stage = LaserStage::Charge;

    // 時間配置（可調整）
    float chargeTime = 0.1f;   // 蓄力
    float beamTime = 0.05f;  // 雷射主體
    float fadeTime = 0.10f;  // 淡出

    float timer = 0.0f;
    

private:
    Vec2 pos;
    Enemy* target;
    int damage;
    bool splash;
    bool slow;
    float slowFactor;
    float slowDuration;
    bool dead = false;

    float speed = 260.f; // 一般子彈速度
};
