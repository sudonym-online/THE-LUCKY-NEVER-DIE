#include "physics.h"
#include "raymath.h"
#include <math.h>

void applyFriction(float deltaTime, Player &player) {
  player.move.velocity.x *= (1.0f - player.move.friction * deltaTime);
  player.move.velocity.z *= (1.0f - player.move.friction * deltaTime);
}

void applyGravity(float deltaTime, Player &player, World &world) {
  player.move.velocity.y -= world.gravity * deltaTime;
}

void floorCheck(Player &player) {
  if (player.position.y <= 0.0f) {
    player.position.y = 0.0f;
    player.col.grounded = true;
    if (player.move.velocity.y < 0)
      player.move.velocity.y = 0;
  }
}

void speedCheck(Player &player) {
  float currentSpeed = sqrtf(player.move.velocity.x * player.move.velocity.x + player.move.velocity.z * player.move.velocity.z);
  if (currentSpeed > player.move.speed) {
    player.move.velocity.x = (player.move.velocity.x / currentSpeed) * player.move.speed;
    player.move.velocity.z = (player.move.velocity.z / currentSpeed) * player.move.speed;
  }
}

void applyJump(Player &player) {
  if (player.jump.bufferTimer > 0 && player.col.grounded) {
    player.move.velocity.y = 55.0f;
    player.jump.bufferTimer = 0;
  }
}

void bufferCountdown(float deltaTime, Player &player) {
  if (player.jump.bufferTimer > 0) {
    player.jump.bufferTimer -= deltaTime;
  }
}

void getCollidingBodies(Player &player, StaticBody allBodies[], int count) {
  player.col.bodyCount = 0;
  for (int i = 0; i < count; i++) {
    if (CheckCollisionBoxes(player.col.aabb, allBodies[i].aabb)) {
      if (player.col.bodyCount < 16) {
        player.col.bodies[player.col.bodyCount++] = &allBodies[i];
      }
    }
  }
}

void resolveCollision(Player &player, StaticBody &body) {
  float overlapX = fmin(player.col.aabb.max.x, body.aabb.max.x) - fmax(player.col.aabb.min.x, body.aabb.min.x);
  float overlapY = fmin(player.col.aabb.max.y, body.aabb.max.y) - fmax(player.col.aabb.min.y, body.aabb.min.y);
  float overlapZ = fmin(player.col.aabb.max.z, body.aabb.max.z) - fmax(player.col.aabb.min.z, body.aabb.min.z);

  if (overlapX < overlapY && overlapX < overlapZ) {
    if (player.col.aabb.min.x < body.aabb.min.x) {
      player.position.x -= overlapX;
    } else {
      player.position.x += overlapX;
    }
    player.move.velocity.x = 0;
  } else if (overlapZ < overlapX && overlapZ < overlapY) {
    if (player.col.aabb.min.z < body.aabb.min.z) {
      player.position.z -= overlapZ;
    } else {
      player.position.z += overlapZ;
    }
    player.move.velocity.z = 0;
  } else {
    if (player.col.aabb.min.y < body.aabb.min.y) {
      player.position.y -= overlapY;
      if (player.move.velocity.y > 0)
        player.move.velocity.y = 0;
    } else {
      player.position.y += overlapY;
      if (player.move.velocity.y < 0)
        player.move.velocity.y = 0;
      player.col.grounded = true;
    }
  }

  player.updateAABB();
}

int physicsProcess(float deltaTime, Player &player, World &world, Camera3D &camera, StaticBody allBodies[], int bodyCount) {
  player.col.grounded = false;

  applyFriction(deltaTime, player);
  applyGravity(deltaTime, player, world);

  player.position.x += player.move.velocity.x * deltaTime;
  player.position.y += player.move.velocity.y * deltaTime;
  player.position.z += player.move.velocity.z * deltaTime;

  floorCheck(player);
  speedCheck(player);

  player.updateAABB();

  getCollidingBodies(player, allBodies, bodyCount);

  if (player.col.bodyCount > 0) {
    for (int i = 0; i < player.col.bodyCount; i++) {
      resolveCollision(player, *player.col.bodies[i]);
    }
  }

  applyJump(player);
  bufferCountdown(deltaTime, player);

  Vector3 newCamPos = (Vector3){player.position.x, player.position.y + player.col.height, player.position.z};
  Vector3 moveDiff = Vector3Subtract(newCamPos, camera.position);
  camera.position = newCamPos;
  camera.target = Vector3Add(camera.target, moveDiff);

  return player.col.bodyCount;
}
