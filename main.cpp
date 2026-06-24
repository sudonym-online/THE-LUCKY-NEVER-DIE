#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "src/game.h"
#include "src/player.h"
#include "src/input.h"
#include "src/physics.h"
#include "src/objects.h"
#include "src/debug.h"

Debug dbg;
World world;
Player player;
Font uiFont;

Camera3D camera = {0};
float mouseSensitivity = 0.15f;
bool debug = true;
Vector2 windowSize = {1280, 720};

void init() {
	InitWindow(windowSize.x, windowSize.y, "THE LUCKY NEVER DIE");

	uiFont = LoadFontEx("Assets/Fonts/Iosevka/IosevkaNerdFontMono-Regular.ttf", 256, nullptr, 0);
	SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR);
	player.visual.armModel = LoadModel("Assets/Arms.obj");

	player.position = (Vector3){0.0f, 0.0f, 4.0f};
	player.UpdateAABB();

	camera.position = (Vector3){player.position.x, player.position.y + player.collision.height, player.position.z};
	camera.target = (Vector3){0.0f, 2.0f, 0.0f};
	camera.up = (Vector3){0.0f, 1.0f, 0.0f};
	camera.fovy = 90.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	DisableCursor();
	SetTargetFPS(60);

	Objects::InitCache();

	dbg.Log("engine initialized [LOG]");

	int catId = Objects::Create("Cat");
	for (int i = 0; i < 5; i++) {
		Objects::Spawn(catId,
			{(float)GetRandomValue(-20, 20), 0.0f, (float)GetRandomValue(-20, 20)},
			{0.1f, 0.1f, 0.1f}, (float)GetRandomValue(0, 360));
	}
	dbg.Log("%d cat(s) spawned [LOG]", 5);

	int deagle = Objects::Create("Deagle");
	Objects::SetAttr(deagle, "ammo", "24");
	Objects::Spawn(deagle, {2.0f, 0.5f, 0.0f}, {0.4f, 0.4f, 0.4f});
	dbg.Log("spawned Deagle (id=%d) [LOG]", deagle);
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
		physicsProcess(deltaTime, player, world, camera);
		bool colliding = player.collision.bodyCount > 0;

		float speed = Vector2Length((Vector2){player.movement.velocity.x, player.movement.velocity.z});
		float targetFov = 60.0f + (speed * 1.5f);
		camera.fovy = Lerp(camera.fovy, targetFov, 5.0f * deltaTime);

		// ------------------- DRAWING -------------------
		BeginDrawing();
		ClearBackground(RAYWHITE);

		// ---- BOTTOM LAYER ----
		BeginMode3D(camera);
				DrawPlane((Vector3){0, 0, 0}, (Vector2){50, 50}, LIGHTGRAY);
				DrawGrid(0, 1.0f);

			for (int i = 0; i < Objects::objectInstanceCount; i++) {
					Objects::objectInstances[i].draw();
					if (debug) {
							DrawBoundingBox(Objects::objectInstances[i].aabb, GREEN);
					}
			}
				if (debug) {
						DrawBoundingBox(player.collision.aabb, RED);
				}
		EndMode3D();

		rlColorMask(false, false, false, false);
		rlClearScreenBuffers();
		rlColorMask(true, true, true, true);

		// ---- TOP LAYER ----
		BeginMode3D(camera);
				player.DrawArms(camera);
		EndMode3D();

		// ------------------- UI -------------------
		if (colliding) {
			DrawTextEx(uiFont, "COLLISION DETECTED", (Vector2){10, 10}, 32, 1, RED);
		} else {
			DrawTextEx(uiFont, "NO COLLISION", (Vector2){10, 10}, 32, 1, GREEN);
		}

		dbg.Draw(uiFont);

		EndDrawing();
	}

	// ------------------- CLEANUP -------------------
	UnloadFont(uiFont);
	UnloadModel(player.visual.armModel);
	Objects::UnloadAll();
	CloseWindow();
	return 0;
}
