
#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <float.h>

class Player {
  // TODO : separate camera from player position.
public:
  float speed = 50.0f;
  float acceleration = 520.0f;
  float friction = 8.0f;
  float height = 7.0f;
  float width = 1.0f;
  float depth = 1.0f;
  float jumpBufferTime = 0.2f;
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

  void UpdateModelOrientation(Model *model, Camera3D camera) {
    Vector3 dir = Vector3Subtract(camera.target, camera.position);
    dir = Vector3Normalize(dir);

    float yaw = atan2f(dir.x, dir.z);

    float horizontalDist = sqrtf(dir.x * dir.x + dir.z * dir.z);
    float pitch = -atan2f(dir.y, horizontalDist);

    Matrix rotation = MatrixMultiply(MatrixRotateX(pitch), MatrixRotateY(yaw));
    model->transform = rotation;
  }

  void drawArms(Camera3D camera) {
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct((Vector3){0, 1, 0}, forward));

    Vector3 baseOffset = Vector3Add(Vector3Scale(forward, armConfig.dist),
                                    Vector3Scale((Vector3){0, 1, 0}, armConfig.height));
    Vector3 leftArmPos = Vector3Add(camera.position,
                                    Vector3Add(baseOffset, Vector3Scale(right, -armConfig.width)));
    Vector3 rightArmPos = Vector3Add(camera.position,
                                     Vector3Add(baseOffset, Vector3Scale(right, armConfig.width)));

    UpdateModelOrientation(&armModel, camera);

    DrawModelEx(armModel, leftArmPos, (Vector3){0, 1, 0}, 0.0f, (Vector3){1, 1, 1}, RED);
    DrawModelEx(armModel, rightArmPos, (Vector3){0, 1, 0}, 0.0f, (Vector3){1, 1, 1}, RED);
  }
};

struct World {
  float gravity = 140.0f;
};

struct StaticBody {
  Model model;
  Vector3 position;
  float rotation;
  Vector3 scale;
  BoundingBox aabb;

  void updateAABB() {
    BoundingBox modelAABB = GetModelBoundingBox(model);
    aabb.min = (Vector3){
        modelAABB.min.x * scale.x + position.x,
        modelAABB.min.y * scale.y + position.y,
        modelAABB.min.z * scale.z + position.z};
    aabb.max = (Vector3){
        modelAABB.max.x * scale.x + position.x,
        modelAABB.max.y * scale.y + position.y,
        modelAABB.max.z * scale.z + position.z};
  }

  void draw() {
    DrawModelEx(model, position, (Vector3){0, 1, 0}, rotation, scale, WHITE);
  }
};

#endif
