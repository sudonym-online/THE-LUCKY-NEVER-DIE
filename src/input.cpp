#include "../include/input.h"

Vector2 getKeyVector() {
  return (Vector2){
      (float)IsKeyDown(KEY_W) - (float)IsKeyDown(KEY_S),
      (float)IsKeyDown(KEY_D) - (float)IsKeyDown(KEY_A),
  };
}

void inputProcess(float deltaTime, Player &player) {
    Vector2 keyVector = getKeyVector();

    // MOVEMENT INPUT
    if (Vector2Length(keyVector) > 0) {
      Vector2 inputDir = Vector2Normalize(keyVector);
      player.velocity.x += inputDir.x * player.acceleration * deltaTime;
      player.velocity.y += inputDir.y * player.acceleration * deltaTime;
    }

    // JUMP BUFFER INPUT
    if (IsKeyPressed(KEY_SPACE)) {
      player.jumpBufferTimer = player.jumpBufferTime;
    }
}
