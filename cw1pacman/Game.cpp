//
// Created by XOS on 24-11-4.
//
#include "Game.h"
#include "algorithm"

const int Game::MAP_WIDTH;
const int Game::MAP_HEIGHT;
const int Game::INFO_WIDTH;
const int Game::TOTAL_WIDTH;

#include <algorithm>
#include <sstream>
#include "Game.h"
#include "GameConfig.h"
#include <conio.h>

Game::Game() :
    pacman(GameConfig::PACMAN_START_X, GameConfig::PACMAN_START_Y), score(0), gameOver(false), powerMode(false),
    powerModeTimeLeft(0), level(1) {

    initializeMap();
    ghosts.push_back(Ghost(GameConfig::GHOST_HOME_X - 2, GameConfig::GHOST_HOME_Y));
    ghosts.push_back(Ghost(GameConfig::GHOST_HOME_X + 2, GameConfig::GHOST_HOME_Y));
    ghostsAlive = std::vector<bool>(ghosts.size(), true);

    console = std::make_unique<ConsoleBuffer>(TOTAL_WIDTH, MAP_HEIGHT);
    powerUpManager = std::make_unique<PowerUpManager>();
    remainingDots = countRemainingDots();
}

void Game::initializeMap() {
    map = std::vector<std::vector<char>>(MAP_HEIGHT, std::vector<char>(MAP_WIDTH));
    // 28x31的经典吃豆人地图布局
    const char initialMap[MAP_HEIGHT][MAP_WIDTH] = {
           {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#','#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
           {'#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.','#', '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
           {'#', '.', '#', '#', '#', '#', '.', '#', '#', '#', '#', '#', '.', '.','.', '.', '.', '#', '#', '#', '#', '#', '#', '.', '#', '#', '#', '.'},
           {'#', 'o', '#', '#', '#', '#', '.', '#', '#', '#', '#', '#', '.', '#','#', '#', '#', '.', '#', '#', '#', '#', '#', '.', '#', '#', '#', 'o'},
           {'#', '.', '#', '#', '#', '#', '.', '#', '#', '#', '#', '#', '.', '#','#', '#', '#', '.', '#', '#', '#', '#', '#', '.', '#', '#', '#', '.'},
           {'#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.','.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
           {'#', '.', '#', '#', '#', '#', '.', '#', '#', '.', '#', '#', '#', '#','#', '#', '#', '#', '#', '#', '.', '#', '#', '.', '#', '#', '#', '.'},
           {'#', '.', '#', '#', '#', '#', '.', '#', '#', '.', '#', '#', '#', '#','#', '#', '#', '#', '#', '#', '.', '#', '#', '.', '#', '#', '#', '.'},
           {'#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.','#', '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
           {'#', '#', '#', '#', '#', '#', '.', '#', '#', '#', '#', '#', '.', '.','.', '.', '.', '#', '#', '#', '#', '#', '.', '#', '#', '#', '#', '#'},
           {'#', '#', '#', '#', '#', '#', '.', '#', '#', '#', '#', '#', '.', '#','#', '#', '#', '.', '#', '#', '#', '#', '.', '#', '#', '#', '#', '#'},
           {'#', '#', '#', '#', '#', '#', '.', '#', '#', '.', '.', '.', '.', '.','.', '.', '.', '.', '.', '.', '#', '#', '.', '#', '#', '#', '#', '#'},
           {'#', '#', '#', '#', '#', '#', '.', '#', '#', '.', '#', '#', '#', ' ',' ', '#', '#', '#', '.', '.', '.', '.', '.', '#', '#', '#', '#', '#'},
           {'#', '#', '#', '#', '#', '#', '.', '#', '#', '.', '#', ' ', ' ', ' ',' ', ' ', ' ', '#', '.', '#', '#', '.', '#', '#', '#', '#', '#', '#'},
           {' ', ' ', ' ', ' ', ' ', ' ', '.', '.', '.', '.', '.', '#', ' ', ' ',' ', ' ', ' ', '#', '.', '#', '#', '.', '#', '#', '#', '#', '#', '#'},
           {'#', '#', '#', '#', '#', '#', '.', '#', '#', '.', '#', ' ', ' ', ' ',' ', ' ', ' ', '#', '.', '#', '#', '.', '#', '#', '#', '#', '#', '#'},
           {'#', '#', '#', '#', '#', '#', '.', '#', '#', '.', '#', '#', '#', '#','#', '#', '#', '#', '.', '#', '#', '.', '#', '#', '#', '#', '#', '#'},
           {'#', '#', '#', '#', '#', '#', '.', '#', '#', '.', '.', '.', '.', '.','.', '.', '.', '.', '.', '.', '.', '.', '.', '#', '#', '#', '#', '#'},
           {'#', '#', '#', '#', '#', '#', '.', '#', '#', '.', '#', '#', '#', '#','#', '#', '#', '#', '#', '#', '#', '.', '#', '#', '#', '#', '#', '#'},
           {'#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.','#', '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
           {'#', '.', '#', '#', '#', '#', '.', '#', '#', '#', '#', '#', '.', '.','.', '.', '.', '#', '#', '#', '#', '#', '.', '#', '#', '#', '#', '.'},
           {'#', '.', '#', '#', '#', '#', '.', '#', '#', '#', '#', '#', '.', '#','#', '#', '#', '.', '#', '#', '#', '#', '.', '#', '#', '#', '#', '.'},
           {'#', 'o', '.', '.', '#', '#', '.', '.', '.', '.', '.', '.', '.', '.','.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
           {'#', '#', '#', '.', '#', '#', '.', '#', '#', '.', '#', '#', '#', '#','#', '#', '#', '#', '#', '#', '.', '#', '#', '.', '#', '#', '#', '#'},
           {'#', '#', '#', '.', '#', '#', '.', '#', '#', '.', '#', '#', '#', '#','#', '#', '#', '#', '#', '#', '.', '#', '#', '.', '#', '#', '#', '#'},
           {'#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.','#', '#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
           {'#', '.', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '.', '.','.', '.', '.', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '.'},
           {'#', '.', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '.', '#','#', '#', '#', '.', '#', '#', '#', '#', '#', '#', '#', '#', '#', '.'},
           {'#', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.','.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
           {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#','#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'} };
    for (int i = 0; i < MAP_HEIGHT; ++i) {
        for (int j = 0; j < MAP_WIDTH; ++j) {
            map[i][j] = initialMap[i][j];
        }
    }
}

std::string Game::centerText(const std::string& text, int width) const {
    int padding = static_cast<int>((width - text.length()) / 2);
    if (padding < 0)
        padding = 0;
    return std::string(padding, ' ') + text;
}

int Game::countRemainingDots() const {
    int count = 0;
    for (const auto& row : map) {
        for (char cell : row) {
            if (cell == GameConfig::DOT || cell == GameConfig::POWER_DOT) {
                ++count;
            }
        }
    }
    return count;
}

void Game::drawInfoPanel() {
    // 绘制分隔线
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        console->draw(MAP_WIDTH, y, '|');
    }

    // 游戏信息显示
    console->drawString(MAP_WIDTH + 2, 1, centerText("PACMAN", INFO_WIDTH - 3));
    console->drawString(MAP_WIDTH + 2, 3, "SCORE:");
    console->drawString(MAP_WIDTH + 2, 4, centerText(std::to_string(score), INFO_WIDTH - 3));
    console->drawString(MAP_WIDTH + 2, 6, "LEVEL:");
    console->drawString(MAP_WIDTH + 2, 7, centerText(std::to_string(level), INFO_WIDTH - 3));
    console->drawString(MAP_WIDTH + 2, 9, "DOTS LEFT:");
    console->drawString(MAP_WIDTH + 2, 10, centerText(std::to_string(remainingDots), INFO_WIDTH - 3));

    // 状态显示
    int yPos = 12;
    if (powerMode) {
        std::string powerStatus = "POWER MODE: " + std::to_string(powerModeTimeLeft / 1000) + "s";
        console->drawString(MAP_WIDTH + 2, yPos, powerStatus);
        yPos += 2;
    }

    // 道具效果显示
    if (powerUpManager->getScoreMultiplier() > 1.0f) {
        console->drawString(MAP_WIDTH + 2, yPos, "2X SCORE");
        yPos += 2;
    }
    if (powerUpManager->getPlayerSpeed() > 1.0f) {
        console->drawString(MAP_WIDTH + 2, yPos, "SPEED UP");
        yPos += 2;
    }
    if (powerUpManager->getGhostSpeed() < 1.0f) {
        console->drawString(MAP_WIDTH + 2, yPos, "GHOSTS SLOWED");
        yPos += 2;
    }

    // 控制说明
    console->drawString(MAP_WIDTH + 2, MAP_HEIGHT - 5, "CONTROLS:");
    console->drawString(MAP_WIDTH + 2, MAP_HEIGHT - 4, "W - Up");
    console->drawString(MAP_WIDTH + 2, MAP_HEIGHT - 3, "S - Down");
    console->drawString(MAP_WIDTH + 2, MAP_HEIGHT - 2, "A - Left");
    console->drawString(MAP_WIDTH + 2, MAP_HEIGHT - 1, "D - Right");
}

void Game::updateGame() {
    if (gameOver)
        return;

    int currentTime = GetTickCount();

    // 更新能量模式状态
    updatePowerMode();

    // 更新道具系统
    powerUpManager->update(currentTime, map);

    // 获取当前速度倍率
    float speedMultiplier = powerUpManager->getPlayerSpeed();
    int moveCount = static_cast<int>(speedMultiplier);
    if (moveCount < 1)
        moveCount = 1;

    for (int i = 0; i < moveCount; ++i) {
        // 保存当前位置
        Position currentPos = pacman.getPosition();

        // 移动吃豆人
        pacman.move(map);
        Position newPos = pacman.getPosition();

        // 检查是否可以移动到新位置
        bool canMove = true;
        if (map[newPos.y][newPos.x] == GameConfig::WALL && !powerUpManager->isWallPassEnabled()) {
            canMove = false;
        }

        if (!canMove) {
            // 如果不能移动，恢复原位置
            pacman = Pacman(currentPos.x, currentPos.y);
        }
        else {
            // 如果可以移动，处理豆子收集
            if (map[newPos.y][newPos.x] == GameConfig::DOT) {
                score += static_cast<int>(GameConfig::NORMAL_DOT_SCORE * powerUpManager->getScoreMultiplier());
                map[newPos.y][newPos.x] = GameConfig::EMPTY;
                --remainingDots;
            }
            else if (map[newPos.y][newPos.x] == GameConfig::POWER_DOT) {
                score += static_cast<int>(GameConfig::POWER_DOT_SCORE * powerUpManager->getScoreMultiplier());
                map[newPos.y][newPos.x] = GameConfig::EMPTY;
                powerMode = true;
                powerModeTimeLeft = POWER_MODE_DURATION;
                --remainingDots;
            }
        }
    }

    // 检查道具碰撞
    powerUpManager->checkCollision(pacman.getPosition());

    // 更新幽灵位置和检查碰撞
    float ghostSpeedMultiplier = powerUpManager->getGhostSpeed();
    for (size_t i = 0; i < ghosts.size(); ++i) {
        if (!ghostsAlive[i])
            continue;

        if (ghostSpeedMultiplier > 0.0f) {
            ghosts[i].move(map);
            if (ghosts[i].getPosition().x == pacman.getPosition().x &&
                ghosts[i].getPosition().y == pacman.getPosition().y) {
                if (powerMode) {
                    score += static_cast<int>(GameConfig::GHOST_SCORE * powerUpManager->getScoreMultiplier());
                    ghostsAlive[i] = false;
                    ghosts[i].setVisible(false);
                }
                else {
                    gameOver = true;
                    return;
                }
            }
        }
    }

    // 检查是否完成关卡
    if (remainingDots == 0) {
        ++level;
        initializeMap();
        remainingDots = countRemainingDots();
        for (size_t i = 0; i < ghostsAlive.size(); ++i) {
            ghostsAlive[i] = true;
            ghosts[i].setVisible(true);
        }
        ghosts[0] = Ghost(GameConfig::GHOST_HOME_X - 2, GameConfig::GHOST_HOME_Y);
        ghosts[1] = Ghost(GameConfig::GHOST_HOME_X + 2, GameConfig::GHOST_HOME_Y);
        powerUpManager->reset();
    }
}

void Game::displayGame() {
    console->clear();

    // 创建显示地图
    std::vector<std::vector<char>> displayMap = map;

    // 放置吃豆人
    Position pPos = pacman.getPosition();
    displayMap[pPos.y][pPos.x] = pacman.getSymbol();

    // 放置存活的幽灵
    for (size_t i = 0; i < ghosts.size(); ++i) {
        if (ghostsAlive[i]) {
            Position gPos = ghosts[i].getPosition();
            displayMap[gPos.y][gPos.x] = ghosts[i].getSymbol();
        }
    }

    // 显示道具
    const auto& powerUps = powerUpManager->getPowerUps();
    for (const auto& powerUp : powerUps) {
        if (powerUp->isActive()) {
            Position pos = powerUp->getPosition();
            if (map[pos.y][pos.x] == GameConfig::EMPTY || map[pos.y][pos.x] == GameConfig::DOT) {
                displayMap[pos.y][pos.x] = powerUp->getSymbol();
            }
        }
    }

    console->drawMatrix(displayMap);
    drawInfoPanel();
    console->swap();
}

void Game::handleGameOver() {
    while (_kbhit())
        _getch(); // 清空输入缓冲

    bool exitGame = false;
    while (!exitGame && gameOver) {
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
            case 'r':
            case 'R':
                // 重置游戏
                score = 0;
                level = 1;
                powerMode = false;
                powerModeTimeLeft = 0;
                gameOver = false;
                initializeMap();
                pacman = Pacman(GameConfig::PACMAN_START_X, GameConfig::PACMAN_START_Y);
                ghosts.clear();
                ghosts.push_back(Ghost(GameConfig::GHOST_HOME_X - 2, GameConfig::GHOST_HOME_Y));
                ghosts.push_back(Ghost(GameConfig::GHOST_HOME_X + 2, GameConfig::GHOST_HOME_Y));
                ghostsAlive = std::vector<bool>(ghosts.size(), true);
                remainingDots = countRemainingDots();
                powerUpManager->reset();
                return;

            case 'q':
            case 'Q':
            case 27: // ESC键
                exitGame = true;
                break;
            }
        }
        Sleep(100);
    }
}

void Game::run() {
    while (true) {
        if (!gameOver) {
            updateGame();
            displayGame();
            Sleep(GameConfig::GAME_SPEED);
        }
        else {
            handleGameOver();
            if (gameOver)
                break; // 如果用户选择退出
        }
    }

    // 显示最终告别屏幕
    console->clear();
    std::string thankYou = "Thanks for playing!";
    console->drawString((TOTAL_WIDTH - thankYou.length()) / 2, MAP_HEIGHT / 2, thankYou);
    console->swap();

    while (!_kbhit())
        Sleep(100);
}

void Game::updatePowerMode() {
    if (powerMode) {
        powerModeTimeLeft -= GameConfig::GAME_SPEED;
        if (powerModeTimeLeft <= 0) {
            powerMode = false;
            powerModeTimeLeft = 0;
        }
    }
}