#pragma once
#include <windows.h>
#include <string>
#include <vector>

class ConsoleBuffer {
public:
    ConsoleBuffer(int gameWidth, int gameHeight);
    ~ConsoleBuffer();

    ConsoleBuffer(const ConsoleBuffer&) = delete;
    ConsoleBuffer& operator=(const ConsoleBuffer&) = delete;

    void clear();
    void draw(int x, int y, char ch);
    void drawString(int x, int y, const std::string& str);
    void drawMatrix(const std::vector<std::vector<char>>& matrix);
    void swap();

private:
    HANDLE hConsole;
    HANDLE screenBuffer[2];
    int currentBuffer;
    int gameWidth;      // Game logic width
    int gameHeight;     // Game logic height
    int displayWidth;   // Actual display width (2 times gameWidth)
    int displayHeight;  // Actual display height
    CHAR_INFO* bufferData;

    // Add Character Map
    static const char CHAR_PACMAN = 'C';
    static const char CHAR_GHOST = 'M';
    static const char CHAR_DOT = '.';
    static const char CHAR_POWER_DOT = 'O';
    static const char CHAR_WALL = '#';
    static const char CHAR_EMPTY = ' ';

    // Get the display attributes of a character
    WORD getCharacterAttributes(char ch);
    // Get the display style of a character
    char getCharacterDisplay(char ch);
};
