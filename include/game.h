#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "raymath.h"
#include <float.h>

class World {
public:
  float gravity = 110.0f;
};

class StaticBody {
public:
  Model model;
  Vector3 position = {0, 0, 0};
  Vector3 scale = {1, 1, 1};
  float rotation = 0.0f;
  BoundingBox aabb;

  void updateAABB() {
    BoundingBox box = GetModelBoundingBox(model);

    Vector3 corners[8] = {
        {box.min.x, box.min.y, box.min.z},
        {box.max.x, box.min.y, box.min.z},
        {box.min.x, box.max.y, box.min.z},
        {box.max.x, box.max.y, box.min.z},
        {box.min.x, box.min.y, box.max.z},
        {box.max.x, box.min.y, box.max.z},
        {box.min.x, box.max.y, box.max.z},
        {box.max.x, box.max.y, box.max.z}};

    for (int i = 0; i < 8; i++) {
      corners[i].x *= scale.x;
      corners[i].y *= scale.y;
      corners[i].z *= scale.z;
      
      corners[i] = Vector3RotateByAxisAngle(corners[i], (Vector3){0, 1, 0}, rotation * DEG2RAD);
      corners[i] = Vector3Add(corners[i], position);
    }

    Vector3 newMin = {FLT_MAX, FLT_MAX, FLT_MAX};
    Vector3 newMax = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

    for (int i = 0; i < 8; i++) {
      if (corners[i].x < newMin.x) newMin.x = corners[i].x;
      if (corners[i].y < newMin.y) newMin.y = corners[i].y;
      if (corners[i].z < newMin.z) newMin.z = corners[i].z;
      if (corners[i].x > newMax.x) newMax.x = corners[i].x;
      if (corners[i].y > newMax.y) newMax.y = corners[i].y;
      if (corners[i].z > newMax.z) newMax.z = corners[i].z;
    }

    aabb = (BoundingBox){newMin, newMax};
  }

  void draw() {
    DrawModelEx(model, position, (Vector3){0, 1, 0}, rotation, scale, WHITE);
  }
};

class Player {
public:
  float speed = 55.0f;
  float acceleration = 240.0f;
  float friction = 5.0f;
  float height = 5.8f;
  float width = 1.5f;
  float depth = 1.5f;
  float jumpBufferTime = 0.20f;
  float jumpBufferTimer = 0.0f;

  Vector3 velocity = {0.0f, 0.0f, 0.0f};
  Model armModel;
  BoundingBox aabb;

  struct ArmConfig {
    float dist = 0.8f;
    float height = -2.8f;
    float width = 4.5f;
  } armConfig;

  void updateAABB(Vector3 position) {
    aabb.min = (Vector3){position.x - width / 2, position.y - height, position.z - depth / 2};
    aabb.max = (Vector3){position.x + width / 2, position.y, position.z + depth / 2};
  }

  void drawArms(Camera3D camera) {
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct({0, 1, 0}, forward));

    Vector3 baseOffset = Vector3Add(Vector3Scale(forward, armConfig.dist), Vector3Scale({0, 1, 0}, armConfig.height));

    Vector3 leftArmPos = Vector3Add(camera.position, Vector3Add(baseOffset, Vector3Scale(right, -armConfig.width)));
    Vector3 rightArmPos = Vector3Add(camera.position, Vector3Add(baseOffset, Vector3Scale(right, armConfig.width)));

    float armAngle = atan2f(forward.x, forward.z);

    DrawModelEx(armModel, leftArmPos, {0, 1, 0}, armAngle * RAD2DEG, {1, 1, 1}, RED);
    DrawModelEx(armModel, rightArmPos, {0, 1, 0}, armAngle * RAD2DEG, {-1, 1, 1}, RED);
  }
};

#endif
