#pragma once
#include <stdint.h>

#include <string>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

// error logging functions
#define error(message) MessageBoxA(NULL, message, "Error", 0);
#define error_number(message) \
  MessageBoxA(NULL, std::to_string(message).c_str(), "Error", 0);
#define error_coords(num1, num2)                                            \
  MessageBoxA(NULL,                                                         \
              (std::to_string(num1) + ", " + std::to_string(num2)).c_str(), \
              "Error", 0);

// important constants
#define SCREEN_WIDTH 155
#define SCREEN_HEIGHT 42
#define SCREEN_WIDTH_PIXELS 1280
#define SCREEN_HEIGHT_PIXELS 720
#define PI 3.14159f

// character width = 792/94 ish equal to 8.4255319148936170212765957446809
// character height = 790/46 ish equal to 17.173913043478260869565217391304
