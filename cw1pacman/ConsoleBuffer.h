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
    int gameWidth;      // ��Ϸ�߼����
    int gameHeight;     // ��Ϸ�߼��߶�
    int displayWidth;   // ʵ����ʾ��ȣ�2��gameWidth��
    int displayHeight;  // ʵ����ʾ�߶�
    CHAR_INFO* bufferData;

    // ����ַ�ӳ���
    static const char CHAR_PACMAN = 'C';
    static const char CHAR_GHOST = 'M';
    static const char CHAR_DOT = '.';
    static const char CHAR_POWER_DOT = 'O';
    static const char CHAR_WALL = '#';
    static const char CHAR_EMPTY = ' ';

    // ��ȡ�ַ�����ʾ����
    WORD getCharacterAttributes(char ch);
    // ��ȡ�ַ�����ʾ��ʽ
    char getCharacterDisplay(char ch);
};
