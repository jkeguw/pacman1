#include "Character.h"

Character::Character(int x, int y, char sym) : pos(x, y), symbol(sym) {}
Character::~Character() {}

Position Character::getPosition() const { return pos; }
char Character::getSymbol() const { return symbol; }
