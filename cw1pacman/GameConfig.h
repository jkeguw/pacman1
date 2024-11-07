#pragma once

namespace GameConfig {
    extern const char WALL;       
    extern const char DOT;        
    extern const char POWER_DOT;  
    extern const char EMPTY;      // empty space
    extern const char PACMAN;     
    extern const char GHOST;      
    extern const char DOOR;       // Ghost door

    extern const int NORMAL_DOT_SCORE;    
    extern const int POWER_DOT_SCORE;     
    extern const int GHOST_SCORE;         

    extern const int GAME_SPEED;          
    extern const int GHOST_SPEED;         
    extern const int POWER_MODE_DURATION; 

    // Initial postion
    extern const int PACMAN_START_X;
    extern const int PACMAN_START_Y;
    extern const int GHOST_HOME_X;
    extern const int GHOST_HOME_Y;
    // Map size
    extern const int MAP_WIDTH;
    extern const int MAP_HEIGHT;

    // Prop system configuration
    const int POWERUP_PERSIST_TIME = 30000;    // Prop display duration
    const int POWERUP_SPAWN_INTERVAL = 15000;  // Prop spawn interval

    // Props effect duration
    const int SPEED_BOOST_DURATION = 5000;
    const int GHOST_FREEZER_DURATION = 7000;
    const int POINT_MULTIPLIER_DURATION = 10000;
    const int WALL_PASS_DURATION = 4000;
    const int GHOST_VACUUM_DURATION = 8000;
    const int TIME_SLOW_DURATION = 6000;

    // Props Effect Strength
    const float SPEED_BOOST_MULTIPLIER = 2.0f;
    const float POINT_MULTIPLIER_VALUE = 2.0f;
    const float TIME_SLOW_FACTOR = 0.5f;
}
