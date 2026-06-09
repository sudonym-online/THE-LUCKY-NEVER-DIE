#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "src/game.h"
#include "src/input.h"
#include "src/physics.h"

World world;
StaticBody cats[5];
Model catModel;
Player player;

Camera3D camera = {0};
float mouseSensitivity = 0.15f;
bool debug = true;
Vector2 windowSize = {800, 450};

void init() {
  InitWindow(windowSize.x, windowSize.y, "THE LUCKY NEVER DIE");

  player.vis.armModel = LoadModel("Assets/Arms.obj");
  catModel = LoadModel("Assets/Mesh_Cat.obj");

  for (int i = 0; i < 5; i++) {
    cats[i].model = catModel;
    cats[i].position = (Vector3){(float)GetRandomValue(-20, 20), 0.0f, (float)GetRandomValue(-20, 20)};
    cats[i].rotation = (float)GetRandomValue(0, 360);
    cats[i].scale = (Vector3){0.1f, 0.1f, 0.1f};
    cats[i].updateAABB();
  }

  player.position = (Vector3){0.0f, 0.0f, 4.0f};
  player.updateAABB();

  camera.position = (Vector3){player.position.x, player.position.y + player.col.height, player.position.z};
  camera.target = (Vector3){0.0f, 2.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 90.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  DisableCursor();
  SetTargetFPS(60);
}

int main(void) {
  init();

  while (!WindowShouldClose()) {
    Vector2 deltaMouse = GetMouseDelta();
    float deltaTime = GetFrameTime();

    // ------------------- INPUT -------------------
    inputProcess(deltaTime, player, camera);

    // ------------------- MOVEMENT -------------------
    UpdateCameraPro(&camera,
                    (Vector3){0, 0, 0},
                    (Vector3){deltaMouse.x * mouseSensitivity,
                              deltaMouse.y * mouseSensitivity, 0.0f},
                    0.0f);

    // ------------------- PHYSICS & COLLISION -------------------
    physicsProcess(deltaTime, player, world, camera, cats, 5);
    bool colliding = player.col.bodyCount > 0;

    float speed = Vector2Length((Vector2){player.move.velocity.x, player.move.velocity.z});
    float targetFov = 60.0f + (speed * 1.5f);
    camera.fovy = Lerp(camera.fovy, targetFov, 5.0f * deltaTime);

    // ------------------- DRAWING -------------------
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // ---- BOTTOM LAYER ----
    BeginMode3D(camera);
        DrawPlane((Vector3){0, 0, 0}, (Vector2){50, 50}, LIGHTGRAY);
        DrawGrid(0, 1.0f);

        for (int i = 0; i < 5; i++) {
            cats[i].draw();
            if (debug) {
                DrawBoundingBox(cats[i].aabb, GREEN);
            }
        }
        if (debug) {
            DrawBoundingBox(player.col.aabb, RED);
        }
    EndMode3D();

    rlColorMask(false, false, false, false);
    rlClearScreenBuffers();
    rlColorMask(true, true, true, true);

    // ---- TOP LAYER ----
    BeginMode3D(camera);
        player.drawArms(camera);
    EndMode3D();

    // ------------------- UI -------------------
    if (colliding) {
      DrawText("COLLISION DETECTED", 10, 10, 20, RED);
    } else {
      DrawText("NO COLLISION", 10, 10, 20, GREEN);
    }

    EndDrawing();
  }

  // ------------------- CLEANUP -------------------
  UnloadModel(player.vis.armModel);
  UnloadModel(catModel);
  CloseWindow();
  return 0;
}
