#pragma once
#include "Character.h"
#include "GameConfig.h"
#include <cstdlib>

class Ghost : public Character {
private:
    bool m_visible;  // Avoid naming conflicts

public:
    Ghost(int x, int y) : Character(x, y, GameConfig::GHOST), m_visible(true) {}

    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; } 

    void move(const std::vector<std::vector<char>>& map) override {
        if (!m_visible) return;  

        int direction = rand() % 4;
        Position newPos = pos;

        switch (direction) {
        case 0: newPos.y--; break;
        case 1: newPos.y++; break;
        case 2: newPos.x--; break;
        case 3: newPos.x++; break;
        }

        if (isValidMove(newPos, map)) {
            pos = newPos;
        }
    }

private:
    bool isValidMove(const Position& newPos, const std::vector<std::vector<char>>& map) const {
        if (newPos.y < 0 || newPos.y >= static_cast<int>(map.size())) return false;
        if (newPos.x < 0 || newPos.x >= static_cast<int>(map[0].size())) return false;
        return map[newPos.y][newPos.x] != GameConfig::WALL;
    }
};
