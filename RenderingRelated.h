#pragma once
#include <iostream>

#include "Scribbler.h"
#include "math.h"

typedef struct Vec2 {
  float x, y;
  Vec2 operator*(int t) { return Vec2{x * t, y * t}; }
  Vec2 operator-(Vec2 v) { return Vec2{x - v.x, y - v.y}; }
  Vec2 operator+(Vec2 v) { return Vec2{x + v.x, y + v.y}; }
} Ray, Point;

typedef struct LineSegment {
  Point point;
  Vec2 vec;
} Wall, RayFromPoint;

float evaluateDistanceBetweenRayAndWall(const LineSegment& v,  // Casted Ray
                                        const Wall& w          // Target Wall
) {
  // math for intersection between line segments

  const float& V_x = v.vec.x;
  const float& V_y = v.vec.y;
  const float& C_x = v.point.x;
  const float& C_y = v.point.y;

  const float& W_x = w.vec.x;
  const float& W_y = w.vec.y;
  const float& D_x = w.point.x;
  const float& D_y = w.point.y;

  float r = float(V_y * D_x + V_x * C_y - V_y * C_x - V_x * D_y) /
            float(V_x * W_y - V_y * W_x);
  float t = float(W_y * C_x + W_x * D_y - W_y * D_x - W_x * C_y) /
            float(W_x * V_y - W_y * V_x);

  if (r < 0 || r > 1) {
    return -1;
  }
  if (t < 0) {
    return -1;
  }

  float tPx, tPy;
  tPx = V_x * t;
  tPy = V_y * t;
  float distance = std::sqrt(tPx * tPx + tPy * tPy);

  return distance;
}

void updateDistances(
    float* distances,        // distances from playerPos along vectors in Rays
    const Point& playerPos,  // player position. origin point of rays
    Vec2* Rays,              // vectors from player position
    size_t amountOfArrays,
    Wall* activeWalls,  // visible walls(line segments)
    size_t amountOfActiveWalls) {
  // create buffer for line segment struct
  RayFromPoint rayFromPointBuf{playerPos};

  // current distance
  float evaluatedDistance;

  // loop through rays in ray array
  for (int i = 0; i < amountOfArrays; i++) {
    // resets distances
    distances[i] = -1.0f;

    // set buffer to current ray
    rayFromPointBuf.vec = Rays[i];

    // loop through active walls
    for (int j = 0; j < amountOfActiveWalls; j++) {
      // calculate distance between buffer with current ray-vector and current
      // active wall
      evaluatedDistance =
          evaluateDistanceBetweenRayAndWall(rayFromPointBuf, activeWalls[j]);

      // round up distances under 1 unit and over 0 units
      if (evaluatedDistance > 0 && evaluatedDistance < 1) evaluatedDistance = 1;

      // disqualify 0-value
      if (evaluatedDistance == 0) evaluatedDistance = -1.0f;

      // update distance if the new distance is shorter i.e. is closer to player
      if (((evaluatedDistance < distances[i]) && evaluatedDistance > 0) ||
          distances[i] == -1.0f)
        distances[i] = evaluatedDistance;
    }
  }
}

void generateRayArray(
    Vec2* rays,  // array of vectors/rays
    const size_t amount,
    const float rotation_rad  // rotation of vectors in radians
) {
  /*
      (screen plane)
  -3_______________3
           |1
           |
           |
           |
           |
           0
      (ray-source)
  */

  // rotation matrix
  float cos = (float)std::cos(rotation_rad);
  float sin = (float)std::sin(rotation_rad);
  Vec2 xHat{cos, -sin};
  Vec2 yHat{sin, cos};

  // starting vector in the left of the screen
  float buffer_startRay_x = -4.0f;
  float buffer_startRay_y = 4.0f;

  // apply rotation matrix
  float startRay_x = buffer_startRay_x * xHat.x + buffer_startRay_y * xHat.y;
  float startRay_y = buffer_startRay_x * yHat.x + buffer_startRay_y * yHat.y;

  // distance between endpoints of rays
  float xIncrement = 6.0f / ((float)amount - 1);

  // 69 funny hehe xd
  Vec2 buffer{69, 69};

  // loop through array of rays
  for (int i = 0; i < amount; i++) {
    // set current ray vector
    buffer.x = startRay_x + (xHat.x * xIncrement * i);
    buffer.y = startRay_y + ((-xHat.y) * xIncrement * i);
    rays[i] = buffer;
  }
}

Vec2* initAndGenerateRayArray(
    const size_t amount,      // amount of rays
    const float rotation_rad  // rotation of vectors in radians
) {
  // allocate rays array on heap
  Vec2* rays = (Vec2*)malloc(sizeof(Vec2) * amount);

  generateRayArray(rays, amount, rotation_rad);

  return rays;
}

void draw3DRenderColumns(
    Scribbler* const s,  // drawing object
    const float* const
        distances,  // distances from playerPos along vectors in Rays
    const size_t amountOfRays) {
  // width of a screen column
  const uint32_t colWidth = (uint32_t)(SCREEN_WIDTH / amountOfRays);

  // remainder. For imperfect amount of columns
  const uint32_t remainderColWidth =
      colWidth + (uint32_t)(SCREEN_WIDTH % amountOfRays);

  // midpoint of screen on y axis
  constexpr uint32_t screenMidPointY = uint32_t(SCREEN_HEIGHT * 0.5);

  // offset y value from midpoint of screen, to center objects to be rendered
  uint32_t offsetY;

  // draw first imperfect column if wall is visible
  if (distances[0] >= 1.0f) {
    // set offset value
    offsetY =
        screenMidPointY - uint32_t(float(SCREEN_HEIGHT) / distances[0] * 0.5f);

    // draw column based on distances
    s->drawRectangle_color(0, offsetY, remainderColWidth,
                           uint32_t(SCREEN_HEIGHT / distances[0]),
                           Scribbler::color::WHITE_BACKGROUND_BLACK_TEXT);
  }
  // loop through rays
  for (int i = 1; i < amountOfRays; i++) {
    // draw if wall is visible
    if (distances[i] >= 1.0f) {
      // set offset value
      offsetY = screenMidPointY -
                uint32_t(float(SCREEN_HEIGHT) / distances[i] * 0.5f);

      // draw column based on distances
      if (distances[i] < 5) {
        s->drawRectangle_colorAndChar(
            remainderColWidth + (i - 1) * colWidth, offsetY, remainderColWidth,
            uint32_t(SCREEN_HEIGHT / distances[i]),
            Scribbler::color::WHITE_BACKGROUND_BLACK_TEXT, L'.');
      } else if (distances[i] < 10) {
        s->drawRectangle_colorAndChar(
            remainderColWidth + (i - 1) * colWidth, offsetY, remainderColWidth,
            uint32_t(SCREEN_HEIGHT / distances[i]),
            Scribbler::color::WHITE_BACKGROUND_BLACK_TEXT, L'+');
      } else {
        s->drawRectangle_colorAndChar(
            remainderColWidth + (i - 1) * colWidth, offsetY, remainderColWidth,
            uint32_t(SCREEN_HEIGHT / distances[i]),
            Scribbler::color::GRAY_BACKGROUND_BLACK_TEXT, L'#');
      }
    }
  }
}
