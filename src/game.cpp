#include "game.h"
#include "raymath.h"
#include <cmath>
#include <float.h>

void StaticBody::updateAABB() {
  BoundingBox modelAABB = GetModelBoundingBox(model);

  Matrix transform = MatrixScale(scale.x, scale.y, scale.z);
  transform = MatrixMultiply(transform, MatrixRotateY(rotation * DEG2RAD));
  transform = MatrixMultiply(transform, MatrixTranslate(position.x, position.y, position.z));

  Vector3 corners[8] = {
      {modelAABB.min.x, modelAABB.min.y, modelAABB.min.z},
      {modelAABB.min.x, modelAABB.min.y, modelAABB.max.z},
      {modelAABB.min.x, modelAABB.max.y, modelAABB.min.z},
      {modelAABB.min.x, modelAABB.max.y, modelAABB.max.z},
      {modelAABB.max.x, modelAABB.min.y, modelAABB.min.z},
      {modelAABB.max.x, modelAABB.min.y, modelAABB.max.z},
      {modelAABB.max.x, modelAABB.max.y, modelAABB.min.z},
      {modelAABB.max.x, modelAABB.max.y, modelAABB.max.z}};

  for (int i = 0; i < 8; i++)
    corners[i] = Vector3Transform(corners[i], transform);

  aabb.min = corners[0];
  aabb.max = corners[0];

  for (int i = 1; i < 8; i++) {
    aabb.min.x = fminf(aabb.min.x, corners[i].x);
    aabb.min.y = fminf(aabb.min.y, corners[i].y);
    aabb.min.z = fminf(aabb.min.z, corners[i].z);
    aabb.max.x = fmaxf(aabb.max.x, corners[i].x);
    aabb.max.y = fmaxf(aabb.max.y, corners[i].y);
    aabb.max.z = fmaxf(aabb.max.z, corners[i].z);
  }
}

void StaticBody::draw() {
  DrawModelEx(model, position, (Vector3){0, 1, 0}, rotation, scale, WHITE);
}

void Player::updateAABB() {
  float bodyHeight = col.height + 0.5f;
  col.aabb.min = (Vector3){position.x - col.width / 2, position.y, position.z - col.depth / 2};
  col.aabb.max = (Vector3){position.x + col.width / 2, position.y + bodyHeight, position.z + col.depth / 2};
}

void Player::UpdateModelOrientation(Model *model, Camera3D camera) {
  Vector3 dir = Vector3Subtract(camera.target, camera.position);
  dir = Vector3Normalize(dir);

  float yaw = atan2f(dir.x, dir.z);

  float horizontalDist = sqrtf(dir.x * dir.x + dir.z * dir.z);
  float pitch = -atan2f(dir.y, horizontalDist);

  Matrix rotation = MatrixMultiply(MatrixRotateX(pitch), MatrixRotateY(yaw));
  model->transform = rotation;
}

void Player::drawArms(Camera3D camera) {
  Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
  Vector3 right = Vector3Normalize(Vector3CrossProduct((Vector3){0, 1, 0}, forward));

  Vector3 baseOffset = Vector3Add(Vector3Scale(forward, vis.armConfig.dist),
                                  Vector3Scale((Vector3){0, 1, 0}, vis.armConfig.height));
  Vector3 leftArmPos = Vector3Add(camera.position,
                                  Vector3Add(baseOffset, Vector3Scale(right, -vis.armConfig.width)));
  Vector3 rightArmPos = Vector3Add(camera.position,
                                   Vector3Add(baseOffset, Vector3Scale(right, vis.armConfig.width)));

  UpdateModelOrientation(&vis.armModel, camera);

  DrawModelEx(vis.armModel, leftArmPos, (Vector3){0, 1, 0}, 0.0f, (Vector3){1, 1, 1}, RED);
  DrawModelEx(vis.armModel, rightArmPos, (Vector3){0, 1, 0}, 0.0f, (Vector3){1, 1, 1}, RED);
}
