#include "Enemy.hpp"
#include "Sound.hpp"
#include <cmath>

Enemy::Enemy(EnemyType t, const std::vector<Vec2>& p, int wave)
    : type(t), path(p), waveNumber(wave)
{
    pos = path[0];
    targetIndex = 1;

    float hpGrowth = 1.0f + (waveNumber - 1) * 0.32f;

    switch (type) {
    case EnemyType::Normal:
        maxHp = 100 * hpGrowth;
        baseSpeed = speed = 60;
        shield = 0.0f;
        reward = 12;
        lifeDamage = 1;
        break;

    case EnemyType::Runner:
        maxHp = 70 * hpGrowth;
        baseSpeed = speed = 120;
        shield = 0.0f;
        reward = 14;
        lifeDamage = 1;
        break;

    case EnemyType::Shield:
        maxHp = 160 * hpGrowth;
        baseSpeed = speed = 55;
        shield = 0.4f;
        reward = 18;
        lifeDamage = 1;
        break;

    case EnemyType::Flying:
        maxHp = 90 * hpGrowth;
        baseSpeed = speed = 150;
        shield = 0.0f;
        reward = 16;
        lifeDamage = 2;
        break;

    case EnemyType::Stealth:
        maxHp = 100 * hpGrowth;
        baseSpeed = speed = 60;
        shield = 0.0f;
        invisibilityAlpha = 80;
        reward = 20;
        lifeDamage = 2;
        break;

    case EnemyType::Boss1:
        maxHp = 5400 * hpGrowth + (waveNumber - 1) * 160;
        baseSpeed = speed = 70;
        shield = 0.3f;
        reward = 120;
        lifeDamage = 10;
        break;

    case EnemyType::Boss2:
        maxHp = 7200 * hpGrowth + (waveNumber - 1) * 200;
        baseSpeed = speed = 55;
        shield = 0.35f;
        reward = 160;
        lifeDamage = 12;
        break;

    case EnemyType::Boss3:
        maxHp = 9000 * hpGrowth + (waveNumber - 1) * 260;
        baseSpeed = speed = 60;
        shield = 0.5f;
        reward = 220;
        lifeDamage = 15;
        break;
    }

    hp = maxHp;
}

void Enemy::update(float dt)
{
    if (dead) return;

    if (slowTimer > 0.0f) {
        slowTimer -= dt;
        speed = baseSpeed * slowFactor;
    }
    else speed = baseSpeed;

    if (type == EnemyType::Flying) {
        updateFlying(dt);
        return;
    }

    if (type == EnemyType::Boss1 || type == EnemyType::Boss2 || type == EnemyType::Boss3) {
        updateBoss(dt);
        return;
    }

    updateNormal(dt);
}

void Enemy::updateNormal(float dt)
{
    if (targetIndex >= (int)path.size()) {
        goalReached = true;
        dead = true;
        return;
    }

    Vec2 target = path[targetIndex];
    float dx = target.x - pos.x;
    float dy = target.y - pos.y;
    float d = std::sqrt(dx * dx + dy * dy);

    if (d < 2.0f) {
        pos = target;
        ++targetIndex;
        return;
    }

    pos.x += dx / d * speed * dt;
    pos.y += dy / d * speed * dt;
}

void Enemy::updateFlying(float dt)
{
    Vec2 goal = path.back();
    float dx = goal.x - pos.x;
    float dy = goal.y - pos.y;
    float d = std::sqrt(dx * dx + dy * dy);

    if (d < 3.0f) {
        goalReached = true;
        dead = true;
        return;
    }

    pos.x += dx / d * speed * dt;
    pos.y += dy / d * speed * dt;
}

void Enemy::updateBoss(float dt)
{
    updateNormal(dt);

    if (type == EnemyType::Boss1) {
        dashCooldown -= dt;
        if (dashCooldown <= 0.0f) {
            dashCooldown = 4.0f;
            dashing = true;
            baseSpeed *= 2.5f;
            dashTimer = 1.0f;
        }
        if (dashing) {
            dashTimer -= dt;
            if (dashTimer <= 0.0f) {
                dashing = false;
                baseSpeed /= 2.5f;
            }
        }
    }

    if (type == EnemyType::Boss3) {
        shieldCooldown -= dt;
        if (shieldCooldown <= 0.0f) {
            shieldCooldown = 8.0f;
            electricShieldTimer = 3.0f;
        }
        if (electricShieldTimer > 0.0f)
            electricShieldTimer -= dt;
    }
}

void Enemy::takeDamage(int dmg)
{
    if (dead) return;

    if (shield > 0.0f)
        dmg = (int)(dmg * (1.0f - shield));

    hp -= dmg;
    Sound::playHit();

    if (hp <= 0.0f) {
        dead = true;
        Sound::playExplosion();
    }
}

void Enemy::applySlow(float factor, float duration)
{
    if (type == EnemyType::Flying) return;
    slowFactor = factor;
    slowTimer = duration;
}

bool Enemy::isVisibleToLaser() const
{
    return type != EnemyType::Stealth;
}

void Enemy::render(SDL_Renderer* renderer)
{
    // 基本 fallback，用色塊表示敵人（如果沒載到貼圖時使用）
    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
    SDL_Rect box{ (int)pos.x - 10, (int)pos.y - 10, 20, 20 };
    SDL_RenderFillRect(renderer, &box);
}



void Enemy::drawHpBar(SDL_Renderer* renderer)
{
    float ratio = hp / maxHp;
    if (ratio < 0) ratio = 0;

    SDL_Rect bg{
        (int)(pos.x - 12),
        (int)(pos.y - 20),
        24, 4
    };
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &bg);

    SDL_Rect fg{
        bg.x,
        bg.y,
        (int)(24 * ratio),
        4
    };
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &fg);
}
