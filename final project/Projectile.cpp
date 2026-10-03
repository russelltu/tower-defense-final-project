#include "Projectile.hpp"
#include "Sound.hpp"
#include <cmath>
static void SDL_RenderDrawCircle(SDL_Renderer* renderer, int cx, int cy, int radius)
{
    for (int w = -radius; w <= radius; w++) {
        int h = (int)std::sqrt(radius * radius - w * w);
        SDL_RenderDrawPoint(renderer, cx + w, cy + h);
        SDL_RenderDrawPoint(renderer, cx + w, cy - h);
    }
}

// =======================
//   Laser Beam Effect (Blue/Black Sci-fi)
// =======================
static void drawLaserBeam(SDL_Renderer* renderer, const Vec2& a, const Vec2& b)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);

    // Outer dark-blue glow
    SDL_SetRenderDrawColor(renderer, 20, 40, 120, 90);
    for (int offset = -2; offset <= 2; offset++) {
        SDL_RenderDrawLine(renderer,
            (int)a.x, (int)a.y + offset,
            (int)b.x, (int)b.y + offset);
    }

    // Bright blue core
    SDL_SetRenderDrawColor(renderer, 80, 160, 255, 255);
    SDL_RenderDrawLine(renderer,
        (int)a.x, (int)a.y,
        (int)b.x, (int)b.y);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

Projectile::Projectile(const Vec2& start,
    Enemy* target,
    int dmg,
    bool splash,
    bool slow,
    float slowFactor,
    float slowDuration)
    : pos(start)
    , target(target)
    , damage(dmg)
    , splash(splash)
    , slow(slow)
    , slowFactor(slowFactor)
    , slowDuration(slowDuration)
{
    Sound::playShoot();
}

void Projectile::update(float dt, std::vector<Enemy*>& enemies)
{
    if (dead) return;
    if (!target || target->isDead()) {
        dead = true;
        return;
    }

    // ======================
    // 雷射：不移動、直接追蹤
    // ======================
    if (isLaser) {

        timer += dt;

        // =========================
        // Stage 1: charging
        // =========================
        if (stage == LaserStage::Charge) {
            if (timer >= chargeTime) {
                stage = LaserStage::Beam;
                timer = 0.0f;

                // 射出瞬間造成傷害
                target->takeDamage(damage);
                if (slow) target->applySlow(slowFactor, slowDuration);
            }
            return;
        }

        // =========================
        // Stage 2: beam firing
        // =========================
        if (stage == LaserStage::Beam) {
            pos = target->getPos();
            if (timer >= beamTime) {
                stage = LaserStage::Fade;
                timer = 0.0f;
            }
            return;
        }

        // =========================
        // Stage 3: fade out
        // =========================
        if (stage == LaserStage::Fade) {
            pos = target->getPos();
            if (timer >= fadeTime) {
                dead = true;
            }
            return;
        }
    }


    // ======================
    // 一般子彈邏輯
    // ======================
    Vec2 tp = target->getPos();
    float dx = tp.x - pos.x;
    float dy = tp.y - pos.y;
    float d = std::sqrt(dx * dx + dy * dy);

    if (d < 4.0f) {
        // Impact
        if (splash) {
            float r2 = 55.f * 55.f;
            for (auto* e : enemies) {
                if (!e || e->isDead()) continue;
                if (dist2(e->getPos(), pos) <= r2) {
                    e->takeDamage(damage);
                    if (slow) e->applySlow(slowFactor, slowDuration);
                }
            }
        }
        else {
            target->takeDamage(damage);
            if (slow) target->applySlow(slowFactor, slowDuration);
        }

        dead = true;
        return;
    }

    pos.x += dx / d * speed * dt;
    pos.y += dy / d * speed * dt;
}

void Projectile::render(SDL_Renderer* renderer) const
{
    if (dead) return;

    if (isLaser) {

        Vec2 end = target->getPos();

        // ----------------------------
        // Stage 1: charging animation
        // ----------------------------
        if (stage == LaserStage::Charge) {
            float scale = timer / chargeTime;
            int radius = (int)(6 + scale * 12);

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
            SDL_SetRenderDrawColor(renderer, 60, 120, 255, (Uint8)(180 * scale));

            for (int i = 0; i < radius; i++) {
                SDL_RenderDrawCircle(renderer, (int)startPos.x, (int)startPos.y, i);
            }

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            return;
        }

        // ----------------------------
        // Stage 2: full-power beam
        // ----------------------------
        if (stage == LaserStage::Beam) {
            drawLaserBeam(renderer, startPos, end);

            // impact spark
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
            SDL_SetRenderDrawColor(renderer, 80, 160, 255, 255);
            for (int i = 0; i < 6; i++) {
                SDL_RenderDrawCircle(renderer, (int)end.x, (int)end.y, i);
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

            return;
        }

        // ----------------------------
        // Stage 3: fading beam
        // ----------------------------
        if (stage == LaserStage::Fade) {

            float alpha = 1.0f - (timer / fadeTime);

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);

            // glow fade
            SDL_SetRenderDrawColor(renderer, 20, 40, 120, (Uint8)(90 * alpha));
            for (int offset = -2; offset <= 2; offset++) {
                SDL_RenderDrawLine(renderer,
                    (int)startPos.x, (int)startPos.y + offset,
                    (int)end.x, (int)end.y + offset);
            }

            // core fade
            SDL_SetRenderDrawColor(renderer, 80, 160, 255, (Uint8)(255 * alpha));
            SDL_RenderDrawLine(renderer,
                (int)startPos.x, (int)startPos.y,
                (int)end.x, (int)end.y);

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            return;
        }
    }


    // Non-laser bullet
    SDL_Rect r{
        (int)(pos.x - 3),
        (int)(pos.y - 3),
        6, 6
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &r);
}
