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
    // 28x31 classic pacman map
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
    // Draw a separator line
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        console->draw(MAP_WIDTH, y, '|');
    }

    // Game info display
    console->drawString(MAP_WIDTH + 2, 1, centerText("PACMAN", INFO_WIDTH - 3));
    console->drawString(MAP_WIDTH + 2, 3, "SCORE:");
    console->drawString(MAP_WIDTH + 2, 4, centerText(std::to_string(score), INFO_WIDTH - 3));
    console->drawString(MAP_WIDTH + 2, 6, "LEVEL:");
    console->drawString(MAP_WIDTH + 2, 7, centerText(std::to_string(level), INFO_WIDTH - 3));
    console->drawString(MAP_WIDTH + 2, 9, "DOTS LEFT:");
    console->drawString(MAP_WIDTH + 2, 10, centerText(std::to_string(remainingDots), INFO_WIDTH - 3));

    // Status display
    int yPos = 12;
    if (powerMode) {
        std::string powerStatus = "POWER MODE: " + std::to_string(powerModeTimeLeft / 1000) + "s";
        console->drawString(MAP_WIDTH + 2, yPos, powerStatus);
        yPos += 2;
    }

    // Prop effect display
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

    // Contorl details
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

    // Update power mode status
    updatePowerMode();

    // Updated prop system
    powerUpManager->update(currentTime, map);

    // Get the current speed multiplier
    float speedMultiplier = powerUpManager->getPlayerSpeed();
    int moveCount = static_cast<int>(speedMultiplier);
    if (moveCount < 1)
        moveCount = 1;

    for (int i = 0; i < moveCount; ++i) {
        // Save current location
        Position currentPos = pacman.getPosition();

        // Move pacman
        pacman.move(map);
        Position newPos = pacman.getPosition();

        // Check if can move to a new location
        bool canMove = true;
        if (map[newPos.y][newPos.x] == GameConfig::WALL && !powerUpManager->isWallPassEnabled()) {
            canMove = false;
        }

        if (!canMove) {
            // If it cannot be moved, restore it to its original position
            pacman = Pacman(currentPos.x, currentPos.y);
        }
        else {
            // If you can move it, process the bean collection
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

    // Checking Prop Collision
    powerUpManager->checkCollision(pacman.getPosition());

    // Update ghost position and check for collision
    float ghostSpeedMultiplier = powerUpManager->getGhostSpeed();
    for (size_t i = 0; i < ghosts.size(); ++i) {
        if (!ghostsAlive[i]) continue;

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
                    displayGameOverScreen();  // Immediately display the game over screen
                    return;
                }
            }
        }
    }

    // Check if the level is completed
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

    // Create map
    std::vector<std::vector<char>> displayMap = map;

    // Display pacman
    Position pPos = pacman.getPosition();
    displayMap[pPos.y][pPos.x] = pacman.getSymbol();

    // Place the ghost alive
    for (size_t i = 0; i < ghosts.size(); ++i) {
        if (ghostsAlive[i]) {
            Position gPos = ghosts[i].getPosition();
            displayMap[gPos.y][gPos.x] = ghosts[i].getSymbol();
        }
    }

    // Display props
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
    displayGameOverScreen();

    // Clear the input buffer
    while (_kbhit()) {
        _getch();
    }

    bool exitGame = false;
    while (!exitGame && gameOver) {
        if (_kbhit()) {
            char key = static_cast<char>(_getch());
            switch (key) {
            case 'r':
            case 'R':
                // Reset Game State
                score = 0;
                level = 1;
                powerMode = false;
                powerModeTimeLeft = 0;
                gameOver = false;

                // Initialize the map and characters
                initializeMap();
                pacman = Pacman(GameConfig::PACMAN_START_X, GameConfig::PACMAN_START_Y);

                // Reset Ghost
                ghosts.clear();
                ghosts.push_back(Ghost(GameConfig::GHOST_HOME_X - 2, GameConfig::GHOST_HOME_Y));
                ghosts.push_back(Ghost(GameConfig::GHOST_HOME_X + 2, GameConfig::GHOST_HOME_Y));
                ghostsAlive = std::vector<bool>(ghosts.size(), true);

                // Reset Other status
                remainingDots = countRemainingDots();
                powerUpManager->reset();
                return;

            case 'q':
            case 'Q':
            case 27:  // ESC
                exitGame = true;
                gameOver = true;  // Make sure the game is over
                break;

            default:
                Sleep(100);  // Avoid CPU overuse
                break;
            }
        }
        Sleep(100);  // Avoid CPU overuse
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
            if (gameOver) {  // If the user chooses to opt out
                // Show final farewell screen
                console->clear();
                std::string thankYou = "Thanks for playing!";
                console->drawString((TOTAL_WIDTH - thankYou.length()) / 2,
                    MAP_HEIGHT / 2,
                    thankYou);
                console->swap();

                // Waiting for the last key press
                Sleep(1000);  // Display 1 second
                while (!_kbhit()) {
                    Sleep(100);
                }
                break;
            }
        }
    }
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

void Game::displayGameOverScreen() {
    console->clear();

    // Calculate the monitor's center position
    int centerY = MAP_HEIGHT / 2;
    int centerX = TOTAL_WIDTH / 2;

    // Game Over Message
    std::vector<std::string> messages = {
        "GAME OVER!",
        "",
        "Final Score: " + std::to_string(score),
        "Level: " + std::to_string(level),
        "",
        "Press 'R' to Restart",
        "Press 'Q' or ESC to Quit"
    };

    // Draw a border
    std::string border(TOTAL_WIDTH - 4, '=');
    console->drawString(2, centerY - 5, border);
    console->drawString(2, centerY + 5, border);

    // Display message
    for (size_t i = 0; i < messages.size(); ++i) {
        std::string centeredText = centerText(messages[i], TOTAL_WIDTH);
        console->drawString(0, centerY - 3 + i, centeredText);
    }

    console->swap();
}