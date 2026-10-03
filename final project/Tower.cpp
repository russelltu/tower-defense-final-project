#include "Tower.hpp"

int Tower::buildCost() const
{
    switch (type) {
    case TowerType::Basic:  return 40;
    case TowerType::Sniper: return 70;
    case TowerType::Splash: return 65;
    case TowerType::Slow:   return 55;
    case TowerType::Laser:  return 80;
    }
    return 40;
}

void Tower::levelUp()
{
    ++level;
    damage = static_cast<int>(damage * 1.35f);
    range = range + 10.0f;
    if (fireRate > 0.2f) fireRate *= 0.9f;
    upgradeCost = static_cast<int>(upgradeCost * 1.5f);
}

SDL_Color Tower::color() const
{
    switch (type) {
    case TowerType::Basic:  return SDL_Color{ 80, 170, 255, 255 };
    case TowerType::Sniper: return SDL_Color{ 200, 230, 120, 255 };
    case TowerType::Splash: return SDL_Color{ 255, 140, 0, 255 };
    case TowerType::Slow:   return SDL_Color{ 120, 220, 255, 255 };
    case TowerType::Laser:  return SDL_Color{ 200, 40, 255, 255 };
    }
    return SDL_Color{ 255, 255, 255, 255 };
}
