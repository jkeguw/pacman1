#include "ConsoleBuffer.h"
#include <algorithm>
#include <stdexcept>


ConsoleBuffer::ConsoleBuffer(int width, int height)
    : gameWidth(width), gameHeight(height), currentBuffer(0) {

    // ��ȡ��׼������
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to get console handle");
    }

    // ���ÿ���̨ģʽ�����ÿ��ٱ༭ģʽ�Ͳ���ģʽ
    DWORD mode;
    GetConsoleMode(hConsole, &mode);
    mode &= ~(ENABLE_QUICK_EDIT_MODE | ENABLE_INSERT_MODE);
    SetConsoleMode(hConsole, mode);

    // ���ô��ڴ�С
    COORD bufferSize = { static_cast<SHORT>(width), static_cast<SHORT>(height) };
    SMALL_RECT windowSize = { 0, 0, static_cast<SHORT>(width - 1), static_cast<SHORT>(height - 1) };

    // ������С�Ĵ��ڴ�С�������û�������С������������յĴ��ڴ�С
    SMALL_RECT minWindow = { 0, 0, 1, 1 };
    SetConsoleWindowInfo(hConsole, TRUE, &minWindow);
    SetConsoleScreenBufferSize(hConsole, bufferSize);
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);

    // ��������
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 8;    // ��С�����С
    cfi.dwFontSize.Y = 16;   // �����ʵ��ĸ߶�
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, LF_FACESIZE, L"Consolas");
    SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);

    // ���ع��
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    // ����˫����
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

        // Ϊÿ��������������ͬ������
        SetConsoleScreenBufferSize(screenBuffer[i], bufferSize);
        SetConsoleCursorInfo(screenBuffer[i], &cursorInfo);
        SetConsoleMode(screenBuffer[i], mode);
    }

    // ���仺�����ڴ�
    bufferData = new CHAR_INFO[width * height];
    clear();

    // ���ô���λ�ã�������ʾ��
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

    // ��ֹ�������ڴ�С
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
        // ������ɫ
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
    case 'C': return 'O';  // �Զ����� O ��ʾ
    case 'M': return 'M';  // ������ M ��ʾ
    case '.': return '.';  // С�㱣�ֲ���
    case 'o': return 'O';  // ���������ô�дO
    case '#': return '#';  // ǽ���� # ��ʾ
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

    WriteConsoleOutputA(  // ʹ��A�汾ȷ��ASCII�ַ���ȷ��ʾ
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
    // ������ʱ�ָ�����̨����
    SetConsoleActiveScreenBuffer(hConsole);
    for (int i = 0; i < 2; ++i) {
        if (screenBuffer[i] != INVALID_HANDLE_VALUE) {
            CloseHandle(screenBuffer[i]);
        }
    }
    delete[] bufferData;

    // �ָ�����̨ģʽ
    DWORD mode;
    GetConsoleMode(hConsole, &mode);
    mode |= ENABLE_QUICK_EDIT_MODE | ENABLE_INSERT_MODE;
    SetConsoleMode(hConsole, mode);
}

void ConsoleBuffer::clear() {
    // ��ջ��������������ַ�����Ϊ�ո���ɫ����ΪĬ��ֵ
    for (int i = 0; i < gameWidth * gameHeight; ++i) {
        bufferData[i].Char.AsciiChar = ' ';
        bufferData[i].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
}