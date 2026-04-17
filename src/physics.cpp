#include "../include/physics.h"
#include <math.h>

void applyFriction(float deltaTime, Player &player) {
  player.velocity.x *= (1.0f - player.friction * deltaTime);
  player.velocity.y *= (1.0f - player.friction * deltaTime);
}

void applyGravity(float deltaTime, Player &player, World &world, Camera3D &camera) {
  if (camera.position.y > player.height) {
    player.velocity.z -= world.gravity * deltaTime;
  }
}

void floorCheck(Player &player, Camera3D &camera) {
  if (camera.position.y <= player.height) {
    camera.position.y = player.height;
    player.velocity.z = 0;
  }
}

void speedCheck(Player &player) {
  float currentSpeed = Vector2Length((Vector2){player.velocity.x, player.velocity.y});
  if (currentSpeed > player.speed) {
    player.velocity.x = (player.velocity.x / currentSpeed) * player.speed;
    player.velocity.y = (player.velocity.y / currentSpeed) * player.speed;
  }
}

void applyJump(Player &player, Camera3D &camera) {
  bool onGround = camera.position.y <= player.height;
  if (player.jumpBufferTimer > 0 && onGround) {
    player.velocity.z = player.speed + 20.0f;
    player.jumpBufferTimer = 0;
  }
}

void bufferCountdown(float deltaTime, Player &player) {
  if (player.jumpBufferTimer > 0) {
    player.jumpBufferTimer -= deltaTime;
  }
}

int getCollidingBodies(Player &player, StaticBody allBodies[], int count, StaticBody *outColliding[]) {
  int found = 0;
  for (int i = 0; i < count; i++) {
    if (CheckCollisionBoxes(player.aabb, allBodies[i].aabb)) {
      outColliding[found++] = &allBodies[i];
    }
  }
  return found;
}

void resolveCollision(Player &player, Camera3D &camera, StaticBody &body) {
  float overlapX = fmin(player.aabb.max.x, body.aabb.max.x) - fmax(player.aabb.min.x, body.aabb.min.x);
  float overlapY = fmin(player.aabb.max.y, body.aabb.max.y) - fmax(player.aabb.min.y, body.aabb.min.y);
  float overlapZ = fmin(player.aabb.max.z, body.aabb.max.z) - fmax(player.aabb.min.z, body.aabb.min.z);

  if (overlapX < overlapY && overlapX < overlapZ) {
    if (player.aabb.min.x < body.aabb.min.x)
      camera.position.x -= overlapX;
    else
      camera.position.x += overlapX;
  } else if (overlapZ < overlapX && overlapZ < overlapY) {
    if (player.aabb.min.z < body.aabb.min.z)
      camera.position.z -= overlapZ;
    else
      camera.position.z += overlapZ;
  } else {
    if (player.aabb.min.y < body.aabb.min.y)
      camera.position.y -= overlapY;
    else
      camera.position.y += overlapY;
    player.velocity.z = 0;
  }

  player.updateAABB(camera.position);
}

int physicsProcess(float deltaTime, Player &player, World &world, Camera3D &camera, StaticBody allBodies[], int bodyCount) {
  applyFriction(deltaTime, player);
  applyGravity(deltaTime, player, world, camera);
  floorCheck(player, camera);
  speedCheck(player);
  applyJump(player, camera);
  bufferCountdown(deltaTime, player);

  player.updateAABB(camera.position);

  StaticBody *colliding[16];
  int hitCount = getCollidingBodies(player, allBodies, bodyCount, colliding);

  if (hitCount > 0) {
    for (int i = 0; i < hitCount; i++) {
      resolveCollision(player, camera, *colliding[i]);
    }
  }

  return hitCount;
}
