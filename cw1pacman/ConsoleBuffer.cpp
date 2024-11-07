#include "ConsoleBuffer.h"
#include <algorithm>
#include <stdexcept>


ConsoleBuffer::ConsoleBuffer(int width, int height)
    : gameWidth(width), gameHeight(height), currentBuffer(0) {

    // 获取标准输出句柄
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to get console handle");
    }

    // 设置控制台模式，禁用快速编辑模式和插入模式
    DWORD mode;
    GetConsoleMode(hConsole, &mode);
    mode &= ~(ENABLE_QUICK_EDIT_MODE | ENABLE_INSERT_MODE);
    SetConsoleMode(hConsole, mode);

    // 设置窗口大小
    COORD bufferSize = { static_cast<SHORT>(width), static_cast<SHORT>(height) };
    SMALL_RECT windowSize = { 0, 0, static_cast<SHORT>(width - 1), static_cast<SHORT>(height - 1) };

    // 先设置小的窗口大小，再设置缓冲区大小，最后设置最终的窗口大小
    SMALL_RECT minWindow = { 0, 0, 1, 1 };
    SetConsoleWindowInfo(hConsole, TRUE, &minWindow);
    SetConsoleScreenBufferSize(hConsole, bufferSize);
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);

    // 设置字体
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 8;    // 减小字体大小
    cfi.dwFontSize.Y = 16;   // 保持适当的高度
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, LF_FACESIZE, L"Consolas");
    SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);

    // 隐藏光标
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    // 创建双缓冲
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

        // 为每个缓冲区设置相同的属性
        SetConsoleScreenBufferSize(screenBuffer[i], bufferSize);
        SetConsoleCursorInfo(screenBuffer[i], &cursorInfo);
        SetConsoleMode(screenBuffer[i], mode);
    }

    // 分配缓冲区内存
    bufferData = new CHAR_INFO[width * height];
    clear();

    // 设置窗口位置（居中显示）
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

    // 禁止调整窗口大小
    LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
    style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
    SetWindowLong(consoleWindow, GWL_STYLE, style);
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
        // 道具颜色
    case 'S':
    case 'F':
    case 'P':
    case 'W':
    case 'V':
    case 'T':
        attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    }
    return attributes;
}

char ConsoleBuffer::getCharacterDisplay(char ch) {
    switch (ch) {
    case 'C': return 'O';  // 吃豆人用 O 表示
    case 'M': return 'M';  // 幽灵用 M 表示
    case '.': return '.';  // 小点保持不变
    case 'o': return 'O';  // 大能量豆用大写O
    case '#': return '#';  // 墙壁用 # 表示
    default: return ch;
    }
}

void ConsoleBuffer::draw(int x, int y, char ch) {
    if (x < 0 || x >= gameWidth || y < 0 || y >= gameHeight) return;

    int index = y * gameWidth + x;
    bufferData[index].Char.AsciiChar = ch;
    bufferData[index].Attributes = getCharacterAttributes(ch);
}

void ConsoleBuffer::drawString(int x, int y, const std::string& str) {
    for (size_t i = 0; i < str.length(); ++i) {
        if (x + i >= gameWidth) break;
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

//void ConsoleBuffer::swap() {
//    SMALL_RECT writeRegion = {
//        0,
//        0,
//        static_cast<SHORT>(displayWidth - 1),
//        static_cast<SHORT>(displayHeight - 1)
//    };
//    COORD bufferSize = {
//        static_cast<SHORT>(displayWidth),
//        static_cast<SHORT>(displayHeight)
//    };
//    COORD bufferCoord = { 0, 0 };
//
//    WriteConsoleOutput(
//        screenBuffer[currentBuffer],
//        bufferData,
//        bufferSize,
//        bufferCoord,
//        &writeRegion
//    );
//
//    SetConsoleActiveScreenBuffer(screenBuffer[currentBuffer]);
//    currentBuffer = 1 - currentBuffer;
//}

void ConsoleBuffer::swap() {
    COORD bufferCoord = { 0, 0 };
    SMALL_RECT writeRegion = {
        0,
        0,
        static_cast<SHORT>(gameWidth - 1),
        static_cast<SHORT>(gameHeight - 1)
    };

    WriteConsoleOutputA(  // 使用A版本确保ASCII字符正确显示
        screenBuffer[currentBuffer],
        bufferData,
        { static_cast<SHORT>(gameWidth), static_cast<SHORT>(gameHeight) },
        bufferCoord,
        &writeRegion
    );

    SetConsoleActiveScreenBuffer(screenBuffer[currentBuffer]);
    currentBuffer = 1 - currentBuffer;
}

ConsoleBuffer::~ConsoleBuffer() {
    // 在析构时恢复控制台设置
    SetConsoleActiveScreenBuffer(hConsole);
    for (int i = 0; i < 2; ++i) {
        if (screenBuffer[i] != INVALID_HANDLE_VALUE) {
            CloseHandle(screenBuffer[i]);
        }
    }
    delete[] bufferData;

    // 恢复控制台模式
    DWORD mode;
    GetConsoleMode(hConsole, &mode);
    mode |= ENABLE_QUICK_EDIT_MODE | ENABLE_INSERT_MODE;
    SetConsoleMode(hConsole, mode);
}

void ConsoleBuffer::clear() {
    // 清空缓冲区，将所有字符设置为空格，颜色设置为默认值
    for (int i = 0; i < gameWidth * gameHeight; ++i) {
        bufferData[i].Char.AsciiChar = ' ';
        bufferData[i].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
}