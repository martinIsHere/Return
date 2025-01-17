#include <iostream>

#include "Macros.h"
#include "RenderingRelated.h"
#include "Scribbler.h"

// TODO : add text functionality in corner
// TODO : add window resize event -> check if new window buffer is bigger
//        -> recreate the currently used window buffer.
// TODO : draw farther objects/walls in darker shade.
// TODO : mapbuilder (HIGH-PRIORITY)

int main() {
  // drawing object
  Scribbler* s = new Scribbler();

  // player position in characters
  Point playerPos{15, 15};

  // rays shooting from player position
  Vec2* arrayOfRays = nullptr;
  size_t amountOfRays = SCREEN_WIDTH;

  // array of distances from walls
  float distances[SCREEN_WIDTH];

  // index of vector pointing forwards from player in the distances array,
  // middle of camera.
  uint16_t indexToForwardDirVector = uint16_t(SCREEN_WIDTH / 2);

  float cameraAngle = PI * 3 / 2 - 0.6;

  // array of rays created on the heap
  arrayOfRays = initAndGenerateRayArray(amountOfRays, cameraAngle);

  // array of visible walls
  size_t amountOfActiveWalls = 5;
  Wall activeWalls[5] = {
      Wall{Vec2{10, -1}, Vec2{0, 10}},
      Wall{Vec2{-1, -1}, Vec2{SCREEN_WIDTH + 1, 0}},
      Wall{Vec2{-1, -1}, Vec2{0, SCREEN_HEIGHT + 1}},
      Wall{Vec2{SCREEN_WIDTH, -1}, Vec2{0, SCREEN_HEIGHT + 1}},
      Wall{Vec2{-1, SCREEN_HEIGHT}, Vec2{SCREEN_WIDTH + 1, 0}}};

  bool programOngoing = true;

  while (programOngoing) {
    // update distances from walls
    updateDistances(distances, playerPos, arrayOfRays, amountOfRays,
                    activeWalls, amountOfActiveWalls);

    // exit key
    if (GetAsyncKeyState(VK_ESCAPE) >> 15) programOngoing = false;

    // movement keys
    // TODO - CLEAN DISHIT UP
    if (GetAsyncKeyState(VK_RIGHT) >> 15) {
      playerPos.y += 0.2f * (-arrayOfRays[indexToForwardDirVector].x);
      playerPos.x += 0.2f * (arrayOfRays[indexToForwardDirVector].y);
    }
    if (GetAsyncKeyState(VK_LEFT) >> 15) {
      playerPos.y -= 0.2f * (-arrayOfRays[indexToForwardDirVector].x);
      playerPos.x -= 0.2f * (arrayOfRays[indexToForwardDirVector].y);
    }
    if (GetAsyncKeyState(VK_UP) >> 15) {
      playerPos.y += 0.2f * arrayOfRays[indexToForwardDirVector].y;
      playerPos.x += 0.2f * arrayOfRays[indexToForwardDirVector].x;
    }
    if (GetAsyncKeyState(VK_DOWN) >> 15) {
      playerPos.y -= 0.2f * arrayOfRays[indexToForwardDirVector].y;
      playerPos.x -= 0.2f * arrayOfRays[indexToForwardDirVector].x;
    }
    if (GetAsyncKeyState('A') >> 15) {
      cameraAngle += 0.1f;
      generateRayArray(arrayOfRays, amountOfRays, cameraAngle);
    }
    if (GetAsyncKeyState('D') >> 15) {
      cameraAngle -= 0.1f;
      generateRayArray(arrayOfRays, amountOfRays, cameraAngle);
    }

    // correct boundary crossing
    if (playerPos.x < 0)
      playerPos.x = 0;

    else if (playerPos.x > SCREEN_WIDTH - 1)
      playerPos.x = SCREEN_WIDTH - 1;

    if (playerPos.y < 0)
      playerPos.y = 0;

    else if (playerPos.y > SCREEN_HEIGHT - 1)
      playerPos.y = SCREEN_HEIGHT - 1;

    // clear
    s->clearScreen();

    s->drawRectangle_color(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT * 0.5,
                           Scribbler::color::BLUE);
    s->drawRectangle_color(0, SCREEN_HEIGHT * 0.5, SCREEN_WIDTH,
                           SCREEN_HEIGHT * 0.5 + 1, Scribbler::color::BLACK);

    // 2.5d
    draw3DRenderColumns(s, distances, amountOfRays);

    // vectors
    s->drawRectangle_color((uint32_t)playerPos.x, (uint32_t)playerPos.y, 1, 1,
                           Scribbler::color::RED);

    // debug key
    if (GetAsyncKeyState('K') >> 15 || 1) {
      // error_number(distances[1]);
      std::string sampleMessage = "                ";
      s->writeText(0U, 0U, sampleMessage.size(), sampleMessage);
      sampleMessage = std::to_string(s->getScreenWidth());
      s->writeText(0U, 0U, sampleMessage.size(), sampleMessage);
    }

    // forward screen buffer
    s->drawToConsole();

    // 100 ms delay
    Sleep(100);
  }
  free(arrayOfRays);

  return 0;
}
