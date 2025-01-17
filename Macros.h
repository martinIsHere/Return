#pragma once
#include <stdint.h>

#include <string>
#define _WIN32_WINNT 0x0500

// error logging functions
#define error(message) MessageBoxA(NULL, message, "Error", NULL);
#define error_number(message) \
  MessageBoxA(NULL, std::to_string(message).c_str(), "Error", NULL);
#define error_coords(num1, num2)                                            \
  MessageBoxA(NULL,                                                         \
              (std::to_string(num1) + ", " + std::to_string(num2)).c_str(), \
              "Error", NULL);

// important constants
// WIDTH 155
// HEIGHT 42
#define SCREEN_WIDTH 300
#define SCREEN_HEIGHT 100
#define SCREEN_WIDTH_PIXELS 1280
#define SCREEN_HEIGHT_PIXELS 720
#define PI 3.14159f

// character width = 792/94 ish equal to 8.4255319148936170212765957446809
// character height = 790/46 ish equal to 17.173913043478260869565217391304
