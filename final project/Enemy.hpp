#pragma once
#include <SDL.h>
#include <vector>
#include "Constants.hpp"

enum class EnemyType {
    Normal,
    Runner,
    Shield,
    Flying,
    Stealth,
    Boss1,
    Boss2,
    Boss3
};

class Enemy {
public:
    Enemy(EnemyType t, const std::vector<Vec2>& p, int wave);
    int waveNumber;

    void update(float dt);
    void render(SDL_Renderer* renderer);

    void takeDamage(int dmg);
    void applySlow(float factor, float duration);

    bool isDead()        const { return dead; }
    bool reachedGoal()   const { return goalReached; }
    const Vec2& getPos() const { return pos; }
    EnemyType getType()  const { return type; }

    bool isVisibleToLaser() const;
    bool hasShield() const { return shield > 0.0f; }

    int  getReward() const { return reward; }          // 被殺掉給玩家多少錢
    int  getLifeDamage() const { return lifeDamage; }  // 到終點扣玩家幾滴血

    float getHP() const { return hp; }
    void setHP(float value) { hp = value; }

    void setPos(const Vec2& p) { pos = p; }
    void drawHpBar(SDL_Renderer* renderer);
private:
    EnemyType type{};
    Vec2 pos{};
    float speed{};
    float baseSpeed{};
    float hp{};
    float maxHp{};
    float shield{};           // 減傷比例 0~1
    int   reward{ 10 };
    int   lifeDamage{ 1 };

    float slowTimer{ 0.0f };
    float slowFactor{ 1.0f };

    bool  dead{ false };
    bool  goalReached{ false };

    std::vector<Vec2> path;
    int targetIndex{ 1 };

    // Boss 用
    float dashCooldown{ 4.0f };
    float dashTimer{ 0.0f };
    bool  dashing{ false };

    float shieldCooldown{ 8.0f };
    float electricShieldTimer{ 0.0f };

    // 隱形敵人透明度
    Uint8 invisibilityAlpha{ 150 };

    void updateNormal(float dt);
    void updateFlying(float dt);
    void updateBoss(float dt);
};
