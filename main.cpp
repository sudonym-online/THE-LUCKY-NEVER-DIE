
#include "raylib.h"
#include "raymath.h"

struct Player {

} player;

// ------------------ HELPERS (MOVE TO A HEADER LIB LATER) -------------------

inline Vector2 getKeyVector() {
  return (Vector2){
      (float)IsKeyDown(KEY_W) - (float)IsKeyDown(KEY_S),
      (float)IsKeyDown(KEY_D) - (float)IsKeyDown(KEY_A),
  };
}

// ---------------------------------------------------------------------------

int main(void) {
  InitWindow(800, 450, "THE LUCKY NEVER DIE");

  Camera3D camera = {0};
  camera.position = (Vector3){0.0f, 2.0f, 4.0f}; // Start position
  camera.target = (Vector3){0.0f, 2.0f, 0.0f};   // Look at center
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};       // Up direction
  camera.fovy = 60.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  float moveSpeed = 0.1f;
  float mouseSensitivity = 0.15f;

  DisableCursor(); // Locks mouse to center for FPS look
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    Vector2 mouseDelta = GetMouseDelta();
    Vector2 keyVector = getKeyVector();

    UpdateCameraPro(
        &camera,
        (Vector3){keyVector.x * moveSpeed, keyVector.y * moveSpeed, 0.0f},
        (Vector3){mouseDelta.x * mouseSensitivity,
                  mouseDelta.y * mouseSensitivity, 0.0f},
        0.0f // Zoom
    );

    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode3D(camera);
    DrawPlane((Vector3){0, 0, 0}, (Vector2){50, 50}, LIGHTGRAY);
    DrawGrid(20, 1.0f);
    DrawCube((Vector3){0, 1, -5}, 2, 2, 2, RED); // Target object
    EndMode3D();

    DrawText("WASD: Move | Mouse: Look", 10, 10, 20, DARKGRAY);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
