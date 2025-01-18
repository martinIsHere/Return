#pragma once
#include <iostream>

#include "Macros.h"
#include "windows.h"

class Scribbler {
  HWND _windowHandle;

  DWORD _consoleMode_data;

  RECT _r;

  HANDLE _hScreenBuffer;

  SMALL_RECT _srctWriteRect;

  // create array of CHAR_INFOs with character and attribute(color) information

  CHAR_INFO _chiBuffer[SCREEN_HEIGHT * SCREEN_WIDTH];

  COORD _coordBufSize;

  COORD _coordBufCoord;

  BOOL _fSuccess;

 public:
  /*
  protocl for color values
  BG = background, FG = foreground
      BG   FG
  0x (N1) (N2)

  */

  // colors to be used in console
  enum class color {
    RED = BACKGROUND_RED,
    GREEN = BACKGROUND_GREEN,
    BLUE = BACKGROUND_BLUE,
    WHITE_TEXT = 0x0F,
    WHITE_BACKGROUND_BLACK_TEXT = 0xF0,
    GRAY_BACKGROUND_BLACK_TEXT = 0x70,
    BLACK = 0x00
  };

  // using the color attribute of characters in console, draw rectangle
  void drawRectangle_color(uint32_t x, uint32_t y, uint32_t width,
                           uint32_t height, color c) {
    // assert boundaries
    if (x >= (uint32_t)_coordBufSize.X || y >= (uint32_t)_coordBufSize.Y) {
      return;
    }
    // assign attribute values to relevant cells
    for (uint32_t i = 0; i < height && y + i < (uint32_t)_coordBufSize.Y; i++) {
      for (uint32_t j = 0; j < width && x + j < (uint32_t)_coordBufSize.X;
           j++) {
        _chiBuffer[(x + _coordBufSize.X * y) + (j + (i)*_coordBufSize.X)]
            .Attributes = (WORD)c;
      }
    }
  }
  // using the color attribute of characters in console, draw rectangle
  void drawRectangle_colorAndChar(uint32_t x, uint32_t y, uint32_t width,
                                  uint32_t height, color c, WCHAR a) {
    // assert boundaries
    if (x >= (uint32_t)_coordBufSize.X || y >= (uint32_t)_coordBufSize.Y) {
      return;
    }
    // assign attribute values to relevant cells
    for (uint32_t i = 0; i < height && y + i < (uint32_t)_coordBufSize.Y; i++) {
      for (uint32_t j = 0; j < width && x + j < (uint32_t)_coordBufSize.X;
           j++) {
        _chiBuffer[(x + _coordBufSize.X * y) + (j + (i)*_coordBufSize.X)]
            .Attributes = (WORD)c;
        _chiBuffer[(x + _coordBufSize.X * y) + (j + (i)*_coordBufSize.X)]
            .Char.UnicodeChar = a;
      }
    }
  }

  // using the character and color information of each character in console
  // write inputted text at given coordinate
  // does not change color attributes
  void writeText(uint32_t x, uint32_t y, size_t stringLen,
                 std::string message) {
    // assert boundaries
    if (x >= (uint32_t)_coordBufSize.X || y >= (uint32_t)_coordBufSize.Y) {
      return;
    }
    // assign attribute values to relevant cells
    for (uint32_t i = 0; i < stringLen && x + i < (uint32_t)_coordBufSize.X;
         i++) {
      _chiBuffer[(x + _coordBufSize.X * y) + i].Char.UnicodeChar =
          (WCHAR)message.at((size_t)i);
    }
  }
  // ----------------------- temporary --------------------------------
  size_t getScreenWidth() { return _srctWriteRect.Right; }
  // ----------------------- temporary --------------------------------

  void clearScreen() {
    // temp bool
    bool a = true;
    for (CHAR_INFO& c : _chiBuffer) {
      if (a) {
        c.Char.UnicodeChar = L':';
        a = false;
      } else {
        c.Char.UnicodeChar = L'*';
        a = true;
      }

      // black background, white text
      c.Attributes = 0x0F;
    }
  }
  // write to console using character_info array buffer
  void drawToConsole() {
    // QUICK-FIX
    // The temporary buffer size
    _coordBufSize.Y = SCREEN_HEIGHT;
    _coordBufSize.X = SCREEN_WIDTH;

    // coordinate in conosle
    _coordBufCoord.X = 0;
    _coordBufCoord.Y = 0;

    // coordinates in conosle describing rectangle in console
    _srctWriteRect.Top = 0;
    _srctWriteRect.Left = 0;
    _srctWriteRect.Bottom = SCREEN_HEIGHT - 1;
    _srctWriteRect.Right = SCREEN_WIDTH - 1;

    // QUICK-FIX END

    _fSuccess =
        WriteConsoleOutput(_hScreenBuffer,  // screen buffer to write to
                           _chiBuffer,      // buffer to copy from
                           _coordBufSize,   // col-row size of chiBuffer
                           _coordBufCoord,  // top left src cell in chiBuffer
                           &_srctWriteRect  // dest. screen buffer rectangle
        );
  }

  Scribbler() {
    // get attached console
    _windowHandle = GetConsoleWindow();

    if (!_windowHandle) exit(1);

    // set console using wide character string
    SetConsoleTitleW(L"Fake 3D");

    // TODO -- CLEAN UP
    // would like to change the size of
    // the characters in the console
    // CONSOLE_FONT_INFOEX _fontInfo{};
    // _fontInfo.dwFontSize = COORD{4, 4};
    // WCHAR fontNameBuffer[32] = L"Niagara Solid Regular";
    // for (int i = 0; i < 32; i++) {
    //  _fontInfo.FaceName[i] = fontNameBuffer[i];
    // }
    // _fontInfo.cbSize = sizeof(_fontInfo);
    // SetCurrentConsoleFontEx(_windowHandle, true, &_fontInfo);

    // get console mode
    if (GetConsoleMode(_windowHandle, &_consoleMode_data)) exit(1);

    // set console mode. Hinders character input
    SetConsoleMode(_windowHandle, (ENABLE_EXTENDED_FLAGS |
                                   (_consoleMode_data & ~ENABLE_MOUSE_INPUT &
                                    ~ENABLE_INSERT_MODE & ~ENABLE_LINE_INPUT)));

    // stores the console's current dimensions
    GetWindowRect(_windowHandle, &_r);

    // changes the console's dimensions. dimensions perfectly (kind of) fit all
    // charcacters
    MoveWindow(_windowHandle, _r.left, _r.top, SCREEN_WIDTH_PIXELS,
               SCREEN_HEIGHT_PIXELS, TRUE);

    // disables the window's resizable property
    SetWindowLong(_windowHandle, GWL_STYLE,
                  GetWindowLong(_windowHandle, GWL_STYLE) & ~WS_MAXIMIZEBOX &
                      ~WS_SIZEBOX);

    _hScreenBuffer = CreateConsoleScreenBuffer(
        GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
        CONSOLE_TEXTMODE_BUFFER, NULL);

    SetConsoleActiveScreenBuffer(_hScreenBuffer);

    // a value shows if odd iteration
    bool a = true;

    // loop through character_info array buffer / screen buffer and set every
    // character to ':' or '*' with black background and white text
    for (CHAR_INFO& c : _chiBuffer) {
      if (a) {
        c.Char.UnicodeChar = L':';
        a = false;
      } else {
        c.Char.UnicodeChar = L'*';
        a = true;
      }

      // black background, white text
      c.Attributes = 0x0F;
    }

    // The temporary buffer size
    _coordBufSize.Y = SCREEN_HEIGHT;
    _coordBufSize.X = SCREEN_WIDTH;

    // coordinate in conosle
    _coordBufCoord.X = 0;
    _coordBufCoord.Y = 0;

    // coordinates in conosle describing rectangle in console
    _srctWriteRect.Top = 0;
    _srctWriteRect.Left = 0;
    _srctWriteRect.Bottom = SCREEN_HEIGHT - 1;
    _srctWriteRect.Right = SCREEN_WIDTH - 1;

    // Copy from the temporary buffer to the new screen buffer.
    drawToConsole();
  }
};
