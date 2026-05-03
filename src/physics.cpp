#include "physics.h"
#include <math.h>

void applyFriction(float deltaTime, Player &player) {
  player.velocity.x *= (1.0f - player.friction * deltaTime);
  player.velocity.z *= (1.0f - player.friction * deltaTime);
}

void applyGravity(float deltaTime, Player &player, World &world) {
  player.velocity.y -= world.gravity * deltaTime;
}

void floorCheck(Player &player) {
  if (player.position.y <= 0.0f) {
    player.position.y = 0.0f;
    player.isGrounded = true;
    if (player.velocity.y < 0)
      player.velocity.y = 0;
  }
}

void speedCheck(Player &player) {
  float currentSpeed = sqrtf(player.velocity.x * player.velocity.x + player.velocity.z * player.velocity.z);
  if (currentSpeed > player.speed) {
    player.velocity.x = (player.velocity.x / currentSpeed) * player.speed;
    player.velocity.z = (player.velocity.z / currentSpeed) * player.speed;
  }
}

void applyJump(Player &player) {
  if (player.jumpBufferTimer > 0 && player.isGrounded) {
    player.velocity.y = 55.0f;
    player.jumpBufferTimer = 0;
  }
}

void bufferCountdown(float deltaTime, Player &player) {
  if (player.jumpBufferTimer > 0) {
    player.jumpBufferTimer -= deltaTime;
  }
}

void getCollidingBodies(Player &player, StaticBody allBodies[], int count) {
  player.collidingCount = 0;
  for (int i = 0; i < count; i++) {
    if (CheckCollisionBoxes(player.aabb, allBodies[i].aabb)) {
      if (player.collidingCount < 16) {
        player.collidingBodies[player.collidingCount++] = &allBodies[i];
      }
    }
  }
}

void resolveCollision(Player &player, StaticBody &body) {
  float overlapX = fmin(player.aabb.max.x, body.aabb.max.x) - fmax(player.aabb.min.x, body.aabb.min.x);
  float overlapY = fmin(player.aabb.max.y, body.aabb.max.y) - fmax(player.aabb.min.y, body.aabb.min.y);
  float overlapZ = fmin(player.aabb.max.z, body.aabb.max.z) - fmax(player.aabb.min.z, body.aabb.min.z);

  if (overlapX < overlapY && overlapX < overlapZ) {
    if (player.aabb.min.x < body.aabb.min.x) {
      player.position.x -= overlapX;
    } else {
      player.position.x += overlapX;
    }
    player.velocity.x = 0;
  } else if (overlapZ < overlapX && overlapZ < overlapY) {
    if (player.aabb.min.z < body.aabb.min.z) {
      player.position.z -= overlapZ;
    } else {
      player.position.z += overlapZ;
    }
    player.velocity.z = 0;
  } else {
    if (player.aabb.min.y < body.aabb.min.y) {
      player.position.y -= overlapY;
      if (player.velocity.y > 0)
        player.velocity.y = 0;
    } else {
      player.position.y += overlapY;
      if (player.velocity.y < 0)
        player.velocity.y = 0;
      player.isGrounded = true;
    }
  }

  player.updateAABB();
}

int physicsProcess(float deltaTime, Player &player, World &world, Camera3D &camera, StaticBody allBodies[], int bodyCount) {
  player.isGrounded = false;

  applyFriction(deltaTime, player);
  applyGravity(deltaTime, player, world);

  player.position.x += player.velocity.x * deltaTime;
  player.position.y += player.velocity.y * deltaTime;
  player.position.z += player.velocity.z * deltaTime;

  floorCheck(player);
  speedCheck(player);

  player.updateAABB();

  getCollidingBodies(player, allBodies, bodyCount);

  if (player.collidingCount > 0) {
    for (int i = 0; i < player.collidingCount; i++) {
      resolveCollision(player, *player.collidingBodies[i]);
    }
  }

  applyJump(player);
  bufferCountdown(deltaTime, player);

  Vector3 newCamPos = (Vector3){player.position.x, player.position.y + player.height, player.position.z};
  Vector3 moveDiff = Vector3Subtract(newCamPos, camera.position);
  camera.position = newCamPos;
  camera.target = Vector3Add(camera.target, moveDiff);

  return player.collidingCount;
}
