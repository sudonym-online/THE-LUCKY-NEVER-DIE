#include "raylib.h"
#include "raymath.h"
#include <float.h>

class World {
public:
  float gravity = 75.0f;
};

World world;

class StaticBody {
public:
  Model model;
  Vector3 position = {0, 0, 0};
  Vector3 scale = {1, 1, 1};
  BoundingBox aabb;

  void updateAABB() {
    BoundingBox box = GetModelBoundingBox(model);

    Vector3 corners[8] = {
        {box.min.x, box.min.y, box.min.z},
        {box.max.x, box.min.y, box.min.z},
        {box.min.x, box.max.y, box.min.z},
        {box.max.x, box.max.y, box.min.z},
        {box.min.x, box.min.y, box.max.z},
        {box.max.x, box.min.y, box.max.z},
        {box.min.x, box.max.y, box.max.z},
        {box.max.x, box.max.y, box.max.z}};

    for (int i = 0; i < 8; i++) {
      corners[i].x *= scale.x;
      corners[i].y *= scale.y;
      corners[i].z *= scale.z;
      corners[i] = Vector3Add(corners[i], position);
    }

    Vector3 newMin = {FLT_MAX, FLT_MAX, FLT_MAX};
    Vector3 newMax = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

    for (int i = 0; i < 8; i++) {
      if (corners[i].x < newMin.x)
        newMin.x = corners[i].x;
      if (corners[i].y < newMin.y)
        newMin.y = corners[i].y;
      if (corners[i].z < newMin.z)
        newMin.z = corners[i].z;
      if (corners[i].x > newMax.x)
        newMax.x = corners[i].x;
      if (corners[i].y > newMax.y)
        newMax.y = corners[i].y;
      if (corners[i].z > newMax.z)
        newMax.z = corners[i].z;
    }

    aabb = (BoundingBox){newMin, newMax};
  }

  void draw() {
    DrawModelEx(model, position, (Vector3){0, 1, 0}, 0, scale, WHITE);
  }
};

class Player {
public:
  float speed = 25.0f;
  float acceleration = 200.0f;
  float friction = 5.0f;
  float height = 0.8f;
  float width = 0.5f;
  float depth = 0.5f;
  float jumpBufferTime = 0.20f;
  float jumpBufferTimer = 0.0f;

  Vector3 velocity = {0.0f, 0.0f, 0.0f};
  Model armModel;
  BoundingBox aabb;

  struct ArmConfig {
    float dist = 0.8f;
    float height = -2.8f;
    float width = 4.5f;
  } armConfig;

  void updateAABB(Vector3 position) {
    aabb.min = (Vector3){position.x - width / 2, position.y - height, position.z - depth / 2};
    aabb.max = (Vector3){position.x + width / 2, position.y, position.z + depth / 2};
  }

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

StaticBody cat;

Player player;

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

  player.armModel = LoadModel("Assets/Arms.obj");
  cat.model = LoadModel("Assets/Mesh_Cat.obj");
  cat.position = (Vector3){8.0f, 3.0f, 6.0f};
  cat.scale = (Vector3){0.1f, 0.1f, 0.1f};
  cat.updateAABB();

  camera.position = (Vector3){0.0f, 2.0f, 4.0f};
  camera.target = (Vector3){0.0f, 2.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 210.0f;
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
    camera.fovy = Lerp(camera.fovy, targetFov, 2.0f * deltaTime);


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

    player.updateAABB(camera.position);

    DrawPlane((Vector3){0, 0, 0}, (Vector2){50, 50}, LIGHTGRAY);
    DrawGrid(20, 1.0f);

    cat.draw();
    DrawBoundingBox(cat.aabb, GREEN);
    DrawBoundingBox(player.aabb, RED);

    EndMode3D();

    EndDrawing();
  }
  UnloadModel(player.armModel);
  UnloadModel(cat.model);
  CloseWindow();
  return 0;
}
