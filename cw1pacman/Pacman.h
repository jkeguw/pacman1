#pragma once
#include <vector>
#include "Character.h"
#include "GameConfig.h"

class Pacman : public Character {
private:
    int directionX; // 当前移动方向
    int directionY;

public:
    Pacman(int x, int y);
    void move(const std::vector<std::vector<char>>& map) override;
    void getDirection(int& dx, int& dy) const;
};