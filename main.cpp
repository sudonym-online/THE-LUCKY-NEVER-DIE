
#include "raylib.h"
#include "raymath.h"

struct Player {
  float speed = 50.0f;
  float acceleration = 50.0f;
  float friction = 10.0f;

  Vector3 velocity = {0.0f, 0.0f, 0.0f};
} player;

// ------------------ HELPERS (MOVE TO A HEADER LIB LATER) -------------------

Camera3D camera = {0};
float mouseSensitivity = 0.15f;

inline Vector2 getKeyVector() {
  return (Vector2){
      (float)IsKeyDown(KEY_W) - (float)IsKeyDown(KEY_S),
      (float)IsKeyDown(KEY_D) - (float)IsKeyDown(KEY_A),
  };
}

void init() {
  InitWindow(800, 450, "THE LUCKY NEVER DIE");

  camera.position = (Vector3){0.0f, 2.0f, 4.0f};
  camera.target = (Vector3){0.0f, 2.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 60.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  DisableCursor();
  SetTargetFPS(60);
}

// ---------------------------------------------------------------------------

int main(void) {

  init();

  while (!WindowShouldClose()) {
    Vector2 deltaMouse = GetMouseDelta();
    Vector2 keyVector = getKeyVector();
    float deltaTime = GetFrameTime();

    if (Vector2Length(keyVector) > 0) {
      player.velocity.x += keyVector.x * player.acceleration * deltaTime;
      player.velocity.y += keyVector.y * player.acceleration * deltaTime;
    }

    player.velocity.x *= (1.0f - player.friction * deltaTime);
    player.velocity.y *= (1.0f - player.friction * deltaTime);

    float speed = Vector2Length((Vector2){player.velocity.x, player.velocity.y});
    float targetFov = 60.0f + (speed * 5.0f);
    camera.fovy = Lerp(camera.fovy, targetFov, 8.0f * deltaTime);

    UpdateCameraPro(&camera,
                    (Vector3){player.velocity.x * deltaTime,
                              player.velocity.y * deltaTime,
                              0.0f},
                    (Vector3){deltaMouse.x * mouseSensitivity,
                              deltaMouse.y * mouseSensitivity, 0.0f},
                    0.0f);

    BeginDrawing();



    ClearBackground(RAYWHITE);
    BeginMode3D(camera);
    DrawPlane((Vector3){0, 0, 0}, (Vector2){50, 50}, LIGHTGRAY);
    DrawGrid(20, 1.0f);
    DrawCube((Vector3){0, 1, -5}, 2, 2, 2, RED);



    EndMode3D();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
