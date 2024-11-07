#pragma once
#include "Position.h"
#include <string>

class PowerUp {
public:
    enum class Type {
        SPEED_BOOST,     
        GHOST_FREEZER,   
        POINT_MULTIPLIER,
        WALL_PASS,       
        GHOST_VACUUM,    
        TIME_SLOW        
    };

    struct Effect {
        int duration;         // Duration time 
        float magnitude;      // Effect strangth
        std::string message;  // Message displayed when activated
    };
    int getSpawnTime() const { return spawnTime; }

private:
    Type type;
    Position pos;
    bool active;
    int spawnTime;
    int displayDuration;  // The time the props is displayed on the map
    char symbol;
    Effect effect;

public:
    PowerUp(Type t, Position p, int displayTime = 10000);

    Position getPosition() const { return pos; }
    bool isActive() const { return active; }
    void setActive(bool state) { active = state; }
    char getSymbol() const { return symbol; }
    Type getType() const { return type; }
    const Effect& getEffect() const { return effect; }

    void update(int currentTime);
    bool shouldRemove(int currentTime) const;
};
