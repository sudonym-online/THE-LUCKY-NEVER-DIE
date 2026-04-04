
#include "raylib.h"
#include "raymath.h" // Required for vector math if needed later

struct MyStruct {
  
};

int main(void) {
  InitWindow(800, 450, "raylib - Explicit WASD Movement");

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

    UpdateCameraPro(&camera,
                    (Vector3){
                        (IsKeyDown(KEY_W) - IsKeyDown(KEY_S)) * moveSpeed,
                        (IsKeyDown(KEY_D) - IsKeyDown(KEY_A)) * moveSpeed,
                        0.0f // Up/Down
                    },
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
