# Tower Defense ++ | C++ Tower Defense Game

A final course project developed with **C++ and SDL2** in **Visual Studio 2022**. Build different types of towers, upgrade their firepower, and manage your money to defend against incoming enemies and survive increasingly difficult waves.

## Overview

The game runs in a 960 × 640 window. You start with **120 money** and **20 lives**. Defeating enemies earns money, while enemies that reach the goal deduct lives based on their type. The game ends when your lives reach 0 or below. Press any key on the game-over screen to restart.

Enemy counts and health increase with each wave. Once all enemies in the current wave have been cleared, the next wave starts automatically.

## Features

- **Five tower types:** Basic, Sniper, Splash, Slow, and Laser.
- **Tower upgrades and selling:** Increase damage, range, and attack speed through upgrades. Selling a tower refunds 50% of its original build cost.
- **Multiple enemy types:** Normal, Runner, Shield, Flying, Stealth, and three Boss variants.
- **Automatic targeting and projectile effects:** Towers attack enemies within range, with splash damage, slowing effects, and laser animations.
- **Game states:** Main menu, playing, paused, and game over.
- **Adjustable game speed:** Switch between 1×, 2×, and 4× speed.
- **Basic save/load support:** Save part of the game state to `save.dat` and load it during gameplay.

## Controls

| Input | Action |
| --- | --- |
| Any key on the main menu | Start the game |
| Left-click a tower button, then a location on the map | Build a tower if you have enough money |
| Left-click an existing tower | Select it and view its stats and range |
| Click `Upgrade` with a tower selected | Spend money to upgrade the tower |
| Click `Sell` with a tower selected | Sell the tower for a partial refund |
| Click an empty area with a tower selected | Deselect it; click a location again to build |
| `P` | Pause / resume |
| `F` | Cycle speed: 1× → 2× → 4× → 1× |
| `S` | Save to `save.dat` in the current working directory |
| `L` | Load `save.dat` from the current working directory |
| `Next` button | Manually advance to the next wave when no enemies are on screen |
| Any key on the game-over screen | Restart the game |
| Close the game window | Exit |

While paused, only `P` to resume and closing the window are accepted. Speed changes, saving, and loading are available during active gameplay.

## Tower Types

| Type | Build Cost | Characteristics |
| --- | ---: | --- |
| Basic | 40 | Single-target attacks; suitable for an early defense |
| Sniper | 70 | High damage and long range, with a longer firing interval |
| Splash | 65 | Deals area damage to nearby enemies when a projectile hits |
| Slow | 55 | Deals damage and temporarily slows enemies; Flying enemies are immune to slowing |
| Laser | 80 | Short firing interval with charge, beam, and fade effects; cannot target Stealth enemies |

## Development Environment and Dependencies

| Component | Project Configuration |
| --- | --- |
| Operating system | Windows |
| IDE | Visual Studio 2022 |
| Compiler toolset | MSVC v143 |
| Windows SDK | Windows 10 SDK, as specified by the project |
| Build platform | x64 |
| SDL2 | [2.30.8](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.8) — window management, events, and rendering |
| SDL2_image | [2.8.2](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.2) — PNG image loading |
| SDL2_ttf | [2.22.0](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-2.22.0) — text rendering |

The library versions above reflect the current `.vcxproj` settings. Install these libraries separately; they are not included in the source code or `assets` directory.

## Build and Run

### 1. Set Up Visual Studio

Install Visual Studio 2022 with the **Desktop development with C++** workload. Make sure MSVC v143 and the Windows SDK are installed.

### 2. Set Up the SDL Libraries

Download the Windows **Visual C++ development packages** from the official release pages linked above. Look for package names containing `devel` and `VC`. Extract them into the following locations to match the current project settings:

```text
C:\libs\
├── SDL2-2.30.8\
├── SDL2_image-2.8.2\
└── SDL2_ttf-2.22.0\
```

Each package directory should contain `include` and `lib\x64`. If you install the libraries elsewhere, update the following project properties for your chosen configuration and the x64 platform:

- **C/C++ → General → Additional Include Directories:** The `include` directory of each package.
- **Linker → General → Additional Library Directories:** The `lib\x64` directory of each package.
- **Linker → Input → Additional Dependencies:** Keep `SDL2main.lib`, `SDL2.lib`, `SDL2_image.lib`, `SDL2_ttf.lib`, and `shell32.lib`.

### 3. Open and Build the Solution

1. Download or clone this repository.
2. Open `final project.sln` in the **repository root** with Visual Studio 2022.
3. Select `Debug | x64` or `Release | x64`.
4. Choose **Build → Build Solution**.

The current Win32 configurations also reference x64 libraries. Use **x64** to avoid an architecture mismatch.

### 4. Add the Runtime DLLs

Copy the runtime DLLs from each development package's `lib\x64` directory into the folder containing the generated `.exe`. These include:

```text
SDL2.dll
SDL2_image.dll
SDL2_ttf.dll
```

Copy any additional dependency DLLs supplied with the packages as well. All DLLs must match the x64 executable architecture.

### 5. Configure Assets and Fonts

Images are loaded using relative paths such as `assets/...`. In the Visual Studio project properties, set **Debugging → Working Directory** to:

```text
$(ProjectDir)
```

This loads `assets` from the inner `final project` directory and writes save files there. If you launch the `.exe` directly by double-clicking it, copy the entire `assets` directory next to the executable.

Text rendering currently uses `C:\Windows\Fonts\msjh.ttc` (Microsoft JhengHei). If this font is unavailable on your system, update the `TTF_OpenFont` path in `Game.cpp` to point to an available font file.

Once setup is complete, press `F5` or `Ctrl + F5` in Visual Studio to run the game.

## Project Structure

The main source files and documents are listed below. Build outputs and temporary directories are omitted.

```text
.
├── README.md
├── final project.sln                    # Root solution entry point
├── final project/
│   ├── final project.vcxproj            # C++ project and build settings
│   ├── final project.vcxproj.filters    # Visual Studio file grouping
│   ├── main.cpp                        # SDL initialization and main loop
│   ├── Constants.hpp                   # Window size, vectors, and distance utilities
│   ├── Game.cpp / Game.hpp              # States, input, waves, UI, and save/load
│   ├── Enemy.cpp / Enemy.hpp            # Enemy attributes, movement, and damage
│   ├── Tower.cpp / Tower.hpp            # Tower types, costs, and upgrades
│   ├── Projectile.cpp / Projectile.hpp  # Projectiles, hit effects, and laser animations
│   ├── Sound.hpp                       # Sound interface with empty implementations
│   └── assets/                         # Background, tower, and enemy images
├── 期末專題報告.pdf
├── Tower_Defense_Class_Diagram_Visual.pdf
└── Tower_Defense_Logic_Flowcharts.pdf
```

## Project Documents

- [Final Project Report (Chinese)](期末專題報告.pdf)
- [Class Diagram](Tower_Defense_Class_Diagram_Visual.pdf)
- [Logic Flowcharts](Tower_Defense_Logic_Flowcharts.pdf)

## Current Limitations and Future Improvements

- **Environment setup:** Library and font paths are fixed Windows paths and may require adjustment on another computer.
- **Save/load completeness:** Only part of the game state is preserved. Loading resets wave spawning information and does not fully restore upgraded tower combat attributes, enemy path progress, or projectile states.
- **Audio:** `Sound.hpp` provides an interface, but sound playback is not implemented yet.
- **Possible improvements:** Complete state restoration, sound effects, additional maps, and more portable dependency and asset configuration.
