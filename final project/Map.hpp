// Map.hpp
#pragma once
#include <SDL.h>
#include <vector>
#include "Constants.hpp"

class Map {
public:
    Map();

    void draw(SDL_Renderer* renderer) const;

    const std::vector<SDL_FPoint>& getPath() const { return path; }

    bool canBuild(int row, int col) const;

    SDL_Point gridFromPixel(int x, int y) const;
    SDL_Point pixelFromGrid(int row, int col) const;

private:
    std::vector<SDL_FPoint> path;
    bool buildable[GRID_ROWS][GRID_COLS]{};
};
