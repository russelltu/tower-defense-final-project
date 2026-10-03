#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "Constants.hpp"
#include "Enemy.hpp"
#include "Tower.hpp"
#include "Projectile.hpp"

enum class GameSpeed {
    Normal = 1,
    Double = 2,
    Quad = 4
};

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    GameOver
};

class Game {
public:
    Game(SDL_Renderer* renderer);
    ~Game();
    SDL_Texture* texBasicTower = nullptr;
    SDL_Texture* texSniperTower = nullptr;
    SDL_Texture* texSplashTower = nullptr;
    SDL_Texture* texSlowTower = nullptr;
    SDL_Texture* texLaserTower = nullptr;
    // Enemy textures
    SDL_Texture* texEnemyNormal = nullptr;
    SDL_Texture* texEnemyRunner = nullptr;
    SDL_Texture* texEnemyShield = nullptr;
    SDL_Texture* texEnemyFlying = nullptr;
    SDL_Texture* texEnemyStealth = nullptr;
    SDL_Texture* texEnemyBoss1 = nullptr;
    SDL_Texture* texEnemyBoss2 = nullptr;
    SDL_Texture* texEnemyBoss3 = nullptr;
    SDL_Texture* backgroundTexture;



    void handleEvents();
    void update(float dt);
    void render();
    bool isRunning() const { return running; }

    void togglePause();
    void cycleSpeed();
    void saveGame();
    void loadGame();
    SDL_Rect button_sell;

private:
    float timeScale = 1.0f;       // 用來控制倍速
    GameSpeed speedMode = GameSpeed::Normal;

    SDL_Renderer* renderer{};
    bool running{ true };
    GameState state{ GameState::MainMenu };

    // 地圖路徑
    std::vector<Vec2> path;

    // 物件
    std::vector<Enemy*> enemies;
    std::vector<Tower>  towers;
    std::vector<Projectile*> projectiles;

    // 遊戲數值
    int money{ 120 };
    int lives{ 20 };
    int wave{ 1 };

    // 產怪
    int   enemiesToSpawn{ 0 };
    float spawnTimer{ 0.0f };
    float spawnInterval{ 1.0f };

    // 選塔/選取
    TowerType selectedTowerType{ TowerType::Basic };
    int selectedTowerIndex{ -1 };

    // UI
    TTF_Font* font{};
    SDL_Rect button_basic{};
    SDL_Rect button_sniper{};
    SDL_Rect button_splash{};
    SDL_Rect button_slow{};
    SDL_Rect button_laser{};
    SDL_Rect button_nextWave{};

    // 內部函式
    void buildPath();
    void startWave(int w);
    void spawnEnemy();

    void handleMouseDown(const SDL_MouseButtonEvent& btn);
    int  towerAt(int mx, int my) const;
    void placeTower(int mx, int my);

    Enemy* findTargetForTower(const Tower& t);
    void towersAttack(float dt);

    void updatePlaying(float dt);

    // 繪圖
    void drawPath();
    void drawTowers();
    void drawEnemies();
    void drawProjectiles();
    void drawUI();

    void drawText(int x, int y, const std::string& text,
        SDL_Color color = SDL_Color{ 255,255,255,255 });
    void drawButton(const SDL_Rect& rect, const std::string& label,
        bool selected = false);

    void renderMainMenu();
    void renderGameOver();
    SDL_Rect button_upgrade;       // 升級按鈕位置

};
