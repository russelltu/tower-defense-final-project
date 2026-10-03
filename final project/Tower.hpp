#pragma once
#include <SDL.h>
#include "Constants.hpp"

enum class TowerType {
    Basic,
    Sniper,
    Splash,
    Slow,
    Laser
};

struct Tower {
    TowerType type{ TowerType::Basic };
    int   x{ 0 }, y{ 0 };
    float range{ 140.0f };
    int   damage{ 40 };
    float fireRate{ 0.8f };    // seconds per shot
    float cooldown{ 0.0f };
    bool  splash{ false };
    bool  slow{ false };
    float slowFactor{ 0.5f };
    float slowDuration{ 1.5f };
    int   level{ 1 };
    int   upgradeCost{ 30 };

    int  buildCost() const;
    void levelUp();
    SDL_Color color() const;
};
