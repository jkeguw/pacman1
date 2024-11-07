#include "Pacman.h"
#include <conio.h>

Pacman::Pacman(int x, int y) : Character(x, y, GameConfig::PACMAN), directionX(0), directionY(0) {}

void Pacman::move(const std::vector<std::vector<char>>& map) {
    // Check keyboard input
    if (_kbhit()) {
        int input = _getch();
        switch (input) {
        case 'W':
        case 'w':
            directionX = 0;
            directionY = -1;
            break;
        case 'S':
        case 's':
            directionX = 0;
            directionY = 1;
            break;
        case 'A':
        case 'a':
            directionX = -1;
            directionY = 0;
            break;
        case 'D':
        case 'd':
            directionX = 1;
            directionY = 0;
            break;
        }
    }

    // Calculate new position
    int newX = pos.x + directionX;
    int newY = pos.y + directionY;

    // Check Bounds
    if (newX < 0)
        newX = static_cast<int>(map[0].size()) - 1;
    if (newX >= static_cast<int>(map[0].size()))
        newX = 0;
    if (newY < 0)
        newY = static_cast<int>(map.size()) - 1;
    if (newY >= static_cast<int>(map.size()))
        newY = 0;

    // Update Location
    pos.x = newX;
    pos.y = newY;
}

void Pacman::getDirection(int& dx, int& dy) const {
    dx = directionX;
    dy = directionY;
}
