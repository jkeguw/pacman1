#pragma once
#include "Position.h"
#include <vector>

class Character {
protected:
    Position pos;
    char symbol;
public:
    Character(int x, int y, char sym);
    virtual ~Character();

    Position getPosition() const;
    char getSymbol() const;
    virtual void move(const std::vector<std::vector<char>>& map) = 0;
};