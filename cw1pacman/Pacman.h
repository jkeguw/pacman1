#pragma once
#include <vector>
#include "Character.h"
#include "GameConfig.h"

class Pacman : public Character {
private:
    int directionX; 
    int directionY;

public:
    Pacman(int x, int y);
    void move(const std::vector<std::vector<char>>& map) override;
    void getDirection(int& dx, int& dy) const;
};