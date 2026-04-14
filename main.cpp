#include "raylib.h"
#include "raymath.h"

class World {
public:
  float gravity = 75.0f;
};

World world;
class Player {
public:
  float speed = 25.0f;
  float acceleration = 200.0f;
  float friction = 5.0f;
  float height = 1.8f;
  float jumpBufferTime = 0.20f;
  float jumpBufferTimer = 0.0f;

  Vector3 velocity = {0.0f, 0.0f, 0.0f};
  Model armModel;

  struct ArmConfig {
    float dist = 0.8f;
    float height = -2.8f;
    float width = 4.5f;
  } armConfig;

  void drawArms(Camera3D camera) {
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct({0, 1, 0}, forward));

    Vector3 baseOffset = Vector3Add(Vector3Scale(forward, armConfig.dist), Vector3Scale({0, 1, 0}, armConfig.height));

    Vector3 leftArmPos = Vector3Add(camera.position, Vector3Add(baseOffset, Vector3Scale(right, -armConfig.width)));
    Vector3 rightArmPos = Vector3Add(camera.position, Vector3Add(baseOffset, Vector3Scale(right, armConfig.width)));

    float armAngle = atan2f(forward.x, forward.z);

    DrawModelEx(armModel, leftArmPos, {0, 1, 0}, armAngle * RAD2DEG, {1, 1, 1}, RED);
    DrawModelEx(armModel, rightArmPos, {0, 1, 0}, armAngle * RAD2DEG, {-1, 1, 1}, RED);
  }
};

Model catModel;
Vector3 catPos = {8.0f, 3.0f, 6.0f};

Player player;

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

  // load arm 3d model
  player.armModel = LoadModel("Assets/Arms.obj");
  catModel = LoadModel("Assets/Mesh_Cat.obj");

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

    // ------------------- INPUT -------------------

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

    // ------------------- PHYSICS -------------------

    // FRICTION
    player.velocity.x *= (1.0f - player.friction * deltaTime);
    player.velocity.y *= (1.0f - player.friction * deltaTime);

    // GRAVITY
    if (camera.position.y > player.height) {
      player.velocity.z -= world.gravity * deltaTime;
    }

    // FLOOR COLLISION
    if (camera.position.y <= player.height) {
      camera.position.y = player.height;
      player.velocity.z = 0;
    }

    // SPEED CLAMP
    float currentSpeed = Vector2Length((Vector2){player.velocity.x, player.velocity.y});
    if (currentSpeed > player.speed) {
      player.velocity.x = (player.velocity.x / currentSpeed) * player.speed;
      player.velocity.y = (player.velocity.y / currentSpeed) * player.speed;
    }

    // JUMP EXECUTION
    bool onGround = camera.position.y <= player.height;
    if (player.jumpBufferTimer > 0 && onGround) {
      player.velocity.z = player.speed + 20.0f;
      player.jumpBufferTimer = 0;
    }

    // BUFFER COUNTDOWN
    if (player.jumpBufferTimer > 0) {
      player.jumpBufferTimer -= deltaTime;
    }

    float speed = Vector2Length((Vector2){player.velocity.x, player.velocity.y});
    float targetFov = 60.0f + (speed * 1.5f);
    camera.fovy = Lerp(camera.fovy, targetFov, 4.0f * deltaTime);


    UpdateCameraPro(&camera,
                    (Vector3){
                        player.velocity.x * deltaTime,
                        player.velocity.y * deltaTime,
                        player.velocity.z * deltaTime},
                    (Vector3){deltaMouse.x * mouseSensitivity,
                              deltaMouse.y * mouseSensitivity, 0.0f},
                    0.0f);

    BeginDrawing();



    ClearBackground(RAYWHITE);
    BeginMode3D(camera);

    player.drawArms(camera);

    DrawPlane((Vector3){0, 0, 0}, (Vector2){50, 50}, LIGHTGRAY);
    DrawGrid(20, 1.0f);
    DrawModel(catModel, catPos, 0.1f, WHITE);
    BoundingBox catBox = GetModelBoundingBox(catModel);
    catBox.min = Vector3Add(Vector3Scale(catBox.min, 0.1f), catPos);
    catBox.max = Vector3Add(Vector3Scale(catBox.max, 0.1f), catPos);
    DrawBoundingBox(catBox, GREEN);


    EndMode3D();

    EndDrawing();
  }
  UnloadModel(player.armModel);
  UnloadModel(catModel);
  CloseWindow();
  return 0;
}
