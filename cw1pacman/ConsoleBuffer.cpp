#include "ConsoleBuffer.h"
#include <algorithm>
#include <stdexcept>

ConsoleBuffer::ConsoleBuffer(int width, int height)
    : gameWidth(width), gameHeight(height), currentBuffer(0) {

    // Get standard output handle
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to get console handle");
    }

    // Set console mode
    DWORD mode;
    GetConsoleMode(hConsole, &mode);
    mode &= ~(ENABLE_QUICK_EDIT_MODE | ENABLE_INSERT_MODE);
    SetConsoleMode(hConsole, mode);

    // Adjust actual display width
    int displayWidth = width * 2;  // Double the width

    // Set window and buffer size
    COORD bufferSize = { static_cast<SHORT>(displayWidth), static_cast<SHORT>(height) };
    SMALL_RECT windowSize = { 0, 0, static_cast<SHORT>(displayWidth - 1), static_cast<SHORT>(height - 1) };

    // with correct order
    SMALL_RECT minWindow = { 0, 0, 1, 1 };
    SetConsoleWindowInfo(hConsole, TRUE, &minWindow);
    SetConsoleScreenBufferSize(hConsole, bufferSize);
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);

    // font
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 8;  // 宽度设为8
    cfi.dwFontSize.Y = 16; // 高度设为16，保持1:2的比例
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, LF_FACESIZE, L"Terminal");  // 使用Terminal字体
    SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);

    // hide cursor
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    // Create double buffer with display width
    for (int i = 0; i < 2; ++i) {
        screenBuffer[i] = CreateConsoleScreenBuffer(
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            CONSOLE_TEXTMODE_BUFFER,
            NULL
        );

        if (screenBuffer[i] == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Failed to create screen buffer");
        }

        // set same element with every buffer
        SetConsoleScreenBufferSize(screenBuffer[i], bufferSize);
        SetConsoleCursorInfo(screenBuffer[i], &cursorInfo);
        SetConsoleMode(screenBuffer[i], mode);
    }

    // set buffer memory with display width
    bufferData = new CHAR_INFO[displayWidth * height];

    // use new showing width
    this->gameWidth = displayWidth;

    clear();

    // window position
    HWND consoleWindow = GetConsoleWindow();
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    RECT rect;
    GetWindowRect(consoleWindow, &rect);
    int consoleWidth = rect.right - rect.left;
    int consoleHeight = rect.bottom - rect.top;
    int posX = (screenWidth - consoleWidth) / 2;
    int posY = (screenHeight - consoleHeight) / 2;
    SetWindowPos(consoleWindow, 0, posX, posY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    // prevent modify the size of window
    LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
    style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
    SetWindowLong(consoleWindow, GWL_STYLE, style);
}

void ConsoleBuffer::draw(int x, int y, char ch) {
    // Convert coordinates to fit double width
    int displayX = x * 2;
    if (displayX < 0 || displayX >= gameWidth || y < 0 || y >= gameHeight) return;

    // set char and attributes
    WORD attrs = getCharacterAttributes(ch);
    int index = y * gameWidth + displayX;

    // Draw characters at double-width positions
    bufferData[index].Char.AsciiChar = ch;
    bufferData[index].Attributes = attrs;

    // Draw spaces at adjacent positions, keeping the same color
    bufferData[index + 1].Char.AsciiChar = ' ';
    bufferData[index + 1].Attributes = attrs;
}

void ConsoleBuffer::drawString(int x, int y, const std::string& str) {
    for (size_t i = 0; i < str.length(); ++i) {
        draw(x + i, y, str[i]);
    }
}

void ConsoleBuffer::drawMatrix(const std::vector<std::vector<char>>& matrix) {
    for (size_t y = 0; y < matrix.size(); ++y) {
        for (size_t x = 0; x < matrix[y].size(); ++x) {
            draw(x, y, matrix[y][x]);
        }
    }
}

void ConsoleBuffer::clear() {
    for (int i = 0; i < gameWidth * gameHeight; ++i) {
        bufferData[i].Char.AsciiChar = ' ';
        bufferData[i].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
}

void ConsoleBuffer::swap() {
    COORD bufferCoord = { 0, 0 };
    SMALL_RECT writeRegion = {
        0,
        0,
        static_cast<SHORT>(gameWidth - 1),
        static_cast<SHORT>(gameHeight - 1)
    };

    WriteConsoleOutputA(
        screenBuffer[currentBuffer],
        bufferData,
        { static_cast<SHORT>(gameWidth), static_cast<SHORT>(gameHeight) },
        bufferCoord,
        &writeRegion
    );

    SetConsoleActiveScreenBuffer(screenBuffer[currentBuffer]);
    currentBuffer = 1 - currentBuffer;
}

WORD ConsoleBuffer::getCharacterAttributes(char ch) {
    WORD attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

    switch (ch) {
    case 'C': // Pacman
    case 'O':
        attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case 'M': // Ghost
        attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;
    case '.': // Dot
        attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
    case '*': // Power dot
    case 'o':
        attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
    case '#': // Wall
        attributes = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
        // Prop Color
    case 'S': // Speed boost
    case 'F': // Ghost freezer
    case 'P': // Point multiplier
    case 'W': // Wall pass
    case 'V': // Ghost vacuum
    case 'T': // Time slow
        attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case '|': // Divider
        attributes = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
    }
    return attributes;
}

ConsoleBuffer::~ConsoleBuffer() {
    // Restore console settings on destruction
    SetConsoleActiveScreenBuffer(hConsole);

    // Cleaning the Buffer
    for (int i = 0; i < 2; ++i) {
        if (screenBuffer[i] != INVALID_HANDLE_VALUE) {
            CloseHandle(screenBuffer[i]);
        }
    }

    // Freeing up memory
    delete[] bufferData;

    // Recovery Console Mode
    DWORD mode;
    GetConsoleMode(hConsole, &mode);
    mode |= ENABLE_QUICK_EDIT_MODE | ENABLE_INSERT_MODE;
    SetConsoleMode(hConsole, mode);

    // Restore window style
    HWND consoleWindow = GetConsoleWindow();
    if (consoleWindow != NULL) {
        LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
        style |= WS_MAXIMIZEBOX | WS_SIZEBOX;
        SetWindowLong(consoleWindow, GWL_STYLE, style);
    }
}