#pragma once
#include <memory>
#include <string>
#include <vector>
#include "ConsoleBuffer.h"
#include "Ghost.h"
#include "Pacman.h"
#include "PowerUpManager.h"

class Game {
public:
    static constexpr int MAP_WIDTH = 28;
    static constexpr int MAP_HEIGHT = 31;
    static constexpr int INFO_WIDTH = 20;
    static constexpr int TOTAL_WIDTH = MAP_WIDTH + INFO_WIDTH;
    static constexpr int POWER_MODE_DURATION = 20000; // 20 seconds

private:
    std::vector<std::vector<char>> map;
    Pacman pacman;
    std::vector<Ghost> ghosts;
    std::vector<bool> ghostsAlive;
    std::unique_ptr<ConsoleBuffer> console;
    std::unique_ptr<PowerUpManager> powerUpManager;
    int score;
    bool gameOver;
    bool powerMode;
    int powerModeTimeLeft;
    int remainingDots;
    int level;

    void initializeMap();
    void updatePowerMode();
    void displayGameOverScreen();
    void updateGame();
    void displayGame();
    void drawInfoPanel();
    int countRemainingDots() const;
    std::string centerText(const std::string& text, int width) const;
    void handleGameOver();

public:
    Game();
    void run();
};