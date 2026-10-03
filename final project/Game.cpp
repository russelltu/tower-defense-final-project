#include "Game.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <SDL_image.h>


Game::Game(SDL_Renderer* r)
    : renderer(r)
{
    running = true;
    backgroundTexture = IMG_LoadTexture(renderer, "assets/background.png");
    if (!backgroundTexture) {
        std::cerr << "Failed to load background: " << IMG_GetError() << "\n";
    }

    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << "\n";
    }
    font = TTF_OpenFont("C:\\Windows\\Fonts\\msjh.ttc", 20);
    if (!font) {
        std::cerr << "OpenFont failed: " << TTF_GetError() << "\n";
    }

    buildPath();

    // UI buttons
    int y = WINDOW_HEIGHT - 70;
    button_basic = SDL_Rect{ 20, y, 80, 40 };
    button_sniper = SDL_Rect{ 120, y, 80, 40 };
    button_splash = SDL_Rect{ 220, y, 80, 40 };
    button_slow = SDL_Rect{ 320, y, 80, 40 };
    button_laser = SDL_Rect{ 420, y, 80, 40 };
    button_nextWave = SDL_Rect{ 520, y, 120, 40 };

    startWave(wave);
    button_upgrade = SDL_Rect{ 650, WINDOW_HEIGHT - 70, 120, 40 };
    button_sell = SDL_Rect{ 780, WINDOW_HEIGHT - 70, 120, 40 };

    // 載入 PNG 函式
    auto loadTex = [&](const char* path) -> SDL_Texture* {
        SDL_Surface* s = IMG_Load(path);
        if (!s) {
            std::cerr << "Failed to load " << path << ": " << IMG_GetError() << "\n";
            return nullptr;
        }
        SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
        SDL_FreeSurface(s);
        return t;
        };

    // 載入所有塔的貼圖
    texBasicTower = loadTex("assets/basic_tower.png");
    texSniperTower = loadTex("assets/sniper_tower.png");
    texSplashTower = loadTex("assets/splash_tower.png");
    texSlowTower = loadTex("assets/slow_tower.png");
    texLaserTower = loadTex("assets/laser_tower.png"); // 你原本的貼圖
    // Load enemy images
    texEnemyNormal = loadTex("assets/normal_enemy.png");
    texEnemyRunner = loadTex("assets/runner_enemy.png");
    texEnemyShield = loadTex("assets/shield_enemy.png");
    texEnemyFlying = loadTex("assets/flying_enemy.png");
    texEnemyStealth = loadTex("assets/stealth_enemy.png");
    texEnemyBoss1 = loadTex("assets/boss1_enemy.png");
    texEnemyBoss2 = loadTex("assets/boss2_enemy.png");
    texEnemyBoss3 = loadTex("assets/boss3_enemy.png");

}
Game::~Game()
{
    for (auto* e : enemies) delete e;
    for (auto* p : projectiles) delete p;
    if (font) TTF_CloseFont(font);
    if (texBasicTower) SDL_DestroyTexture(texBasicTower);
    if (texSniperTower) SDL_DestroyTexture(texSniperTower);
    if (texSplashTower) SDL_DestroyTexture(texSplashTower);
    if (texSlowTower) SDL_DestroyTexture(texSlowTower);
    if (texLaserTower) SDL_DestroyTexture(texLaserTower);
    if (texEnemyNormal)  SDL_DestroyTexture(texEnemyNormal);
    if (texEnemyRunner)  SDL_DestroyTexture(texEnemyRunner);
    if (texEnemyShield)  SDL_DestroyTexture(texEnemyShield);
    if (texEnemyFlying)  SDL_DestroyTexture(texEnemyFlying);
    if (texEnemyStealth) SDL_DestroyTexture(texEnemyStealth);
    if (texEnemyBoss1)   SDL_DestroyTexture(texEnemyBoss1);
    if (texEnemyBoss2)   SDL_DestroyTexture(texEnemyBoss2);
    if (texEnemyBoss3)   SDL_DestroyTexture(texEnemyBoss3);
    SDL_DestroyTexture(backgroundTexture);
    TTF_Quit();
}
void Game::buildPath()
{
    path.clear();
    path.push_back(Vec2{ 40, 60 });
    path.push_back(Vec2{ 40, 260 });
    path.push_back(Vec2{ 260, 260 });
    path.push_back(Vec2{ 260, 500 });
    path.push_back(Vec2{ 600, 500 });
    path.push_back(Vec2{ 600, 140 });
    path.push_back(Vec2{ 860, 140 });
}
void Game::startWave(int w)
{
    enemiesToSpawn = 8 + w * 3;
    spawnInterval = std::max(0.6f, 1.0f - w * 0.05f);
    spawnTimer = 1.0f;
}
void Game::spawnEnemy()
{
    EnemyType t = EnemyType::Normal;

    if (wave >= 2 && wave % 3 == 0) t = EnemyType::Runner;
    if (wave >= 3 && wave % 4 == 0) t = EnemyType::Shield;
    if (wave >= 4 && wave % 5 == 0) t = EnemyType::Flying;
    if (wave >= 5 && wave % 6 == 0) t = EnemyType::Stealth;
    if (wave >= 7 && enemiesToSpawn == 1) t = EnemyType::Boss1;
    if (wave >= 10 && enemiesToSpawn == 1) t = EnemyType::Boss2;
    if (wave >= 14 && enemiesToSpawn == 1) t = EnemyType::Boss3;

    enemies.push_back(new Enemy(t, path,wave));
    --enemiesToSpawn;
}
void Game::handleEvents()
{
    SDL_Event e;

    while (SDL_PollEvent(&e)) {

        if (e.type == SDL_QUIT) {
            running = false;
        }

        // MAIN MENU
        if (state == GameState::MainMenu) {
            if (e.type == SDL_KEYDOWN)
                state = GameState::Playing;
            continue;
        }

        // PLAYING
        if (state == GameState::Playing) {

            if (e.type == SDL_MOUSEBUTTONDOWN)
                handleMouseDown(e.button);

            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_p: togglePause(); break;
                case SDLK_s: saveGame(); break;
                case SDLK_l: loadGame(); break;
                case SDLK_f: cycleSpeed(); break;
                }
            }
            continue;
        }

        // PAUSED
        if (state == GameState::Paused) {
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_p)
                togglePause();
            continue;  // Block all other input
        }

        // GAME OVER
        if (state == GameState::GameOver) {
            if (e.type == SDL_KEYDOWN) {
                money = 120;
                lives = 20;
                wave = 1;
                for (auto* en : enemies) delete en;
                enemies.clear();
                for (auto* pr : projectiles) delete pr;
                projectiles.clear();
                towers.clear();
                buildPath();
                startWave(wave);
                state = GameState::Playing;
            }
        }
    }
}
void Game::handleMouseDown(const SDL_MouseButtonEvent& btn)
{
    if (btn.button != SDL_BUTTON_LEFT) return;
    int mx = btn.x;
    int my = btn.y;
    SDL_Point p{ mx, my };
    // ---------- 點擊 'Sell' 鍵 ----------
    if (selectedTowerIndex != -1) {
        if (SDL_PointInRect(&p, &button_sell)) {

            Tower& t = towers[selectedTowerIndex];

            // 賣掉金額 = 建造費用 50%
            int sellPrice = t.buildCost() / 2;
            money += sellPrice;

            // 移除塔
            towers.erase(towers.begin() + selectedTowerIndex);

            selectedTowerIndex = -1; // 取消選取
            return;
        }
    }

    // ---------- 1. 點防禦塔升級按鈕 ----------
    if (selectedTowerIndex != -1) {
        if (SDL_PointInRect(&p, &button_upgrade)) {
            Tower& t = towers[selectedTowerIndex];
            if (money >= t.upgradeCost) {
                money -= t.upgradeCost;
                t.levelUp();
            }
            return;
        }
    }

    // ---------- 2. 點 UI 選塔按鈕 ----------
    if (SDL_PointInRect(&p, &button_basic)) { selectedTowerType = TowerType::Basic;  return; }
    if (SDL_PointInRect(&p, &button_sniper)) { selectedTowerType = TowerType::Sniper; return; }
    if (SDL_PointInRect(&p, &button_splash)) { selectedTowerType = TowerType::Splash; return; }
    if (SDL_PointInRect(&p, &button_slow)) { selectedTowerType = TowerType::Slow;   return; }
    if (SDL_PointInRect(&p, &button_laser)) { selectedTowerType = TowerType::Laser;  return; }

    if (SDL_PointInRect(&p, &button_nextWave)) {
        if (enemies.empty()) {
            ++wave;
            startWave(wave);
        }
        return;
    }

    // ---------- 3. 檢查是否點到塔 ----------
    int idx = towerAt(mx, my);
    if (idx != -1) {
        selectedTowerIndex = idx;  // 進入「查看塔」模式
        return;                    // 不立即升級，要按升級鍵
    }

    // ---------- 4. 如果點其他空白 → 取消選取塔 ----------
    if (selectedTowerIndex != -1) {
        selectedTowerIndex = -1;
        return;
    }

    // ---------- 5. 在沒有選塔時點空白 → 放置新塔 ----------
    placeTower(mx, my);
}
int Game::towerAt(int mx, int my) const
{
    for (int i = 0; i < (int)towers.size(); i++) {
        int dx = mx - towers[i].x;
        int dy = my - towers[i].y;
        if (dx * dx + dy * dy <= 20 * 20)
            return i;
    }
    return -1;
}
void Game::placeTower(int mx, int my)
{
    Tower t;
    t.type = selectedTowerType;

    switch (selectedTowerType) {
    case TowerType::Basic:
        t.damage = 22; t.range = 140; t.fireRate = 0.8f; break;
    case TowerType::Sniper:
        t.damage = 60; t.range = 220; t.fireRate = 1.6f; break;
    case TowerType::Splash:
        t.damage = 35; t.range = 150; t.fireRate = 1.0f; t.splash = true; break;
    case TowerType::Slow:
        t.damage = 18; t.range = 150; t.fireRate = 0.9f; t.slow = true; t.slowFactor = 0.4f; t.slowDuration = 2.0f; break;
    case TowerType::Laser:
        t.damage = 50; t.range = 190; t.fireRate = 0.25f; break;
    }

    int cost = t.buildCost();
    if (money < cost) return;

    t.x = mx; t.y = my;
    t.cooldown = 0;
    towers.push_back(t);
    money -= cost;
}
Enemy* Game::findTargetForTower(const Tower& t)
{
    Enemy* best = nullptr;
    float bestDist = 1e9f;

    for (auto* e : enemies) {
        if (!e || e->isDead()) continue;

        if (t.type == TowerType::Laser && !e->isVisibleToLaser())
            continue;

        float d = dist2(Vec2{ (float)t.x, (float)t.y }, e->getPos());
        if (d <= t.range * t.range && (!best || d < bestDist)) {
            best = e;
            bestDist = d;
        }
    }
    return best;
}
void Game::towersAttack(float dt)
{
    for (Tower& t : towers) {
        t.cooldown -= dt;
        if (t.cooldown > 0) continue;

        Enemy* target = findTargetForTower(t);
        if (!target) continue;

        Vec2 start{ (float)t.x, (float)t.y };
        Projectile* p = new Projectile(start, target,
            t.damage, t.splash, t.slow, t.slowFactor, t.slowDuration);

        if (t.type == TowerType::Laser) {
            p->isLaser = true;
            p->startPos = start;   // 記住雷射起始點（塔）
        }

        projectiles.push_back(p);


        t.cooldown = t.fireRate;
    }

    for (auto* p : projectiles)
        p->update(dt, enemies);

    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](Projectile* p) { if (p->isDead()) { delete p; return true; } return false; }),
        projectiles.end()
    );
}
void Game::updatePlaying(float dt)
{
    if (enemiesToSpawn > 0) {
        spawnTimer -= dt;
        if (spawnTimer <= 0) {
            spawnEnemy();
            spawnTimer = spawnInterval;
        }
    }

    for (auto* e : enemies)
        e->update(dt);

    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [&](Enemy* e) {
                if (!e) return true;
                if (e->isDead()) {
                    if (e->reachedGoal()) lives -= e->getLifeDamage();
                    else money += e->getReward();
                    delete e;
                    return true;
                }
                return false;
            }),
        enemies.end()
    );

    towersAttack(dt);

    if (lives <= 0) state = GameState::GameOver;

    if (enemies.empty() && enemiesToSpawn <= 0) {
        ++wave;
        startWave(wave);
    }
}
void Game::update(float dt)
{
    if (!running) return;

    if (state == GameState::Paused)
        return;

    dt *= timeScale;

    if (state == GameState::Playing)
        updatePlaying(dt);
}
void Game::render()
{
    if (!running) return;

    if (state == GameState::MainMenu) {
        SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
        SDL_RenderClear(renderer);
        drawText(260, 200, "TOWER DEFENSE ++");
        drawText(260, 260, "Press any key to start");
        SDL_RenderPresent(renderer);
        return;
    }

    if (state == GameState::Paused) {
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 220);
        SDL_RenderClear(renderer);
        drawText(270, 250, "PAUSED");
        drawText(230, 300, "Press P to Resume");
        SDL_RenderPresent(renderer);
        return;
    }

    if (state == GameState::GameOver) {
        SDL_SetRenderDrawColor(renderer, 40, 0, 0, 255);
        SDL_RenderClear(renderer);
        drawText(260, 220, "GAME OVER");
        drawText(240, 260, "Press any key to restart");
        SDL_RenderPresent(renderer);
        return;
    }

    SDL_SetRenderDrawColor(renderer, 40, 120, 40, 255);
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    drawPath();
    drawTowers();
    drawEnemies();
    drawProjectiles();
    drawUI();

    drawText(700, 10, "Speed x" + std::to_string((int)timeScale));

    SDL_RenderPresent(renderer);
}
void Game::togglePause()
{
    if (state == GameState::Playing)
        state = GameState::Paused;
    else if (state == GameState::Paused)
        state = GameState::Playing;
}
void Game::cycleSpeed()
{
    if (speedMode == GameSpeed::Normal) {
        speedMode = GameSpeed::Double;
        timeScale = 2.0f;
    }
    else if (speedMode == GameSpeed::Double) {
        speedMode = GameSpeed::Quad;
        timeScale = 4.0f;
    }
    else {
        speedMode = GameSpeed::Normal;
        timeScale = 1.0f;
    }

    std::cout << "Game Speed Changed: x" << timeScale << "\n";
}
void Game::saveGame()
{
    std::ofstream out("save.dat");
    if (!out) return;

    out << money << " " << lives << " " << wave << " "
        << enemiesToSpawn << " " << spawnTimer << " " << spawnInterval << "\n";

    out << (int)speedMode << " " << timeScale << "\n";

    out << enemies.size() << "\n";
    for (auto* e : enemies) {
        Vec2 p = e->getPos();
        out << (int)e->getType() << " " << p.x << " " << p.y << " " << e->getHP() << "\n";
    }

    out << towers.size() << "\n";
    for (auto& t : towers) {
        out << (int)t.type << " " << t.x << " " << t.y << " " << t.level << "\n";
    }

    std::cout << "Game Saved\n";
}
void Game::loadGame()
{
    std::ifstream in("save.dat");
    if (!in) return;

    enemies.clear();
    towers.clear();

    int speedM;

    in >> money >> lives >> wave >> enemiesToSpawn >> spawnTimer >> spawnInterval;
    in >> speedM >> timeScale;

    speedMode = (GameSpeed)speedM;

    startWave(wave);

    int count;

    in >> count;
    for (int i = 0; i < count; i++) {
        int type;
        float x, y, hp;
        in >> type >> x >> y >> hp;

        Enemy* e = new Enemy((EnemyType)type, path,wave);
        e->setPos(Vec2{ x, y });
        e->setHP(hp);
        enemies.push_back(e);
    }

    in >> count;
    for (int i = 0; i < count; i++) {
        int t, x, y, lvl;
        in >> t >> x >> y >> lvl;

        Tower tower;
        tower.type = (TowerType)t;
        tower.x = x;
        tower.y = y;
        tower.level = lvl;
        towers.push_back(tower);
    }

    std::cout << "Game Loaded\n";
}
void Game::drawPath()
{
    SDL_SetRenderDrawColor(renderer, 120, 80, 40, 255);

    for (size_t i = 0; i + 1 < path.size(); ++i) {
        SDL_RenderDrawLine(renderer,
            (int)path[i].x, (int)path[i].y,
            (int)path[i + 1].x, (int)path[i + 1].y);
    }
}
void Game::drawTowers()
{
    for (size_t i = 0; i < towers.size(); ++i) {
        Tower& t = towers[i];

        // -----------------------------
        //  使用圖片畫雷射塔
        // -----------------------------
        SDL_Texture* tex = nullptr;
        int size = 64; // 顯示大小（可調整）

        switch (t.type) {
        case TowerType::Basic:  tex = texBasicTower; break;
        case TowerType::Sniper: tex = texSniperTower; break;
        case TowerType::Splash: tex = texSplashTower; break;
        case TowerType::Slow:   tex = texSlowTower; break;
        case TowerType::Laser:  tex = texLaserTower; break;
        }

        if (tex) {
            SDL_Rect dst{ t.x - size / 2, t.y - size / 2, size, size };
            SDL_RenderCopy(renderer, tex, nullptr, &dst);
        }
        else {
            // fallback: 畫方塊塔
            SDL_Color c = t.color();
            SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
            SDL_Rect box{ t.x - 12, t.y - 12, 24, 24 };
            SDL_RenderFillRect(renderer, &box);
        }


        // ----------選取顯示射程----------
        if ((int)i == selectedTowerIndex) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 80);
            int R = (int)t.range;
            for (int r = -R; r <= R; r += 4) {
                int h = (int)std::sqrt((double)R * R - r * r);
                SDL_RenderDrawLine(renderer,
                    t.x - h, t.y + r,
                    t.x + h, t.y + r);
            }
        }
    }
}
void Game::drawEnemies()
{
    for (auto* e : enemies) {
        if (!e) continue;

        SDL_Texture* tex = nullptr;

        switch (e->getType()) {
        case EnemyType::Normal:  tex = texEnemyNormal;  break;
        case EnemyType::Runner:  tex = texEnemyRunner;  break;
        case EnemyType::Shield:  tex = texEnemyShield;  break;
        case EnemyType::Flying:  tex = texEnemyFlying;  break;
        case EnemyType::Stealth: tex = texEnemyStealth; break;
        case EnemyType::Boss1:   tex = texEnemyBoss1;   break;
        case EnemyType::Boss2:   tex = texEnemyBoss2;   break;
        case EnemyType::Boss3:   tex = texEnemyBoss3;   break;
        }

        Vec2 p = e->getPos();
        int size = 64;

        if (e->getType() == EnemyType::Boss1 ||
            e->getType() == EnemyType::Boss2 ||
            e->getType() == EnemyType::Boss3)
        {
            size = 120;    // Boss 放大
        }

        SDL_Rect dst{
            (int)p.x - size / 2,
            (int)p.y - size / 2,
            size, size
        };

        if (tex) {
            SDL_RenderCopy(renderer, tex, nullptr, &dst);
        }
        else {
            // 載圖失敗時，用 Enemy 自己的 render 畫方塊
            e->render(renderer);
        }

        e->drawHpBar(renderer);
    }
}
void Game::drawProjectiles()
{
    for (auto* p : projectiles)
        p->render(renderer);
}
void Game::drawUI()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
    SDL_Rect bar{ 0, WINDOW_HEIGHT - 80, WINDOW_WIDTH, 80 };
    SDL_RenderFillRect(renderer, &bar);

    drawButton(button_basic, "Basic", selectedTowerType == TowerType::Basic);
    drawButton(button_sniper, "Sniper", selectedTowerType == TowerType::Sniper);
    drawButton(button_splash, "Splash", selectedTowerType == TowerType::Splash);
    drawButton(button_slow, "Slow", selectedTowerType == TowerType::Slow);
    drawButton(button_laser, "Laser", selectedTowerType == TowerType::Laser);
    drawButton(button_nextWave, "Next", false);

    drawText(10, 10, "Money: " + std::to_string(money));
    drawText(10, 35, "Lives: " + std::to_string(lives));
    drawText(10, 60, "Wave:  " + std::to_string(wave));

    drawText(10, WINDOW_HEIGHT - 110, "[F] Speed  [P] Pause  [S] Save  [L] Load");

    if (selectedTowerIndex != -1) {
        drawButton(button_upgrade, "Upgrade", false);
        drawButton(button_sell, "Sell", false);
        Tower& t = towers[selectedTowerIndex];
        drawText(button_upgrade.x + 8, button_upgrade.y - 20,
            "Lv " + std::to_string(t.level) +
            "  Cost: " + std::to_string(t.upgradeCost));
        drawText(button_sell.x, button_sell.y - 20,
            "Sell: " + std::to_string(t.buildCost() / 2));
    }

}
void Game::drawButton(const SDL_Rect& rect, const std::string& label, bool selected)
{
    if (selected)
        SDL_SetRenderDrawColor(renderer, 200, 200, 80, 255);
    else
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);

    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);

    drawText(rect.x + 8, rect.y + 8, label);
}
void Game::drawText(int x, int y, const std::string& text, SDL_Color color)
{
    if (!font) return;

    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect r{ x, y, surf->w, surf->h };
    SDL_RenderCopy(renderer, tex, nullptr, &r);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

   


