#include "physics.h"
#include "raymath.h"
#include <math.h>

void applyFriction(float deltaTime, Player &player) {
	player.movement.velocity.x *= (1.0f - player.movement.friction * deltaTime);
	player.movement.velocity.z *= (1.0f - player.movement.friction * deltaTime);
}

void applyGravity(float deltaTime, Player &player, World &world) {
	player.movement.velocity.y -= world.gravity * deltaTime;
}

void floorCheck(Player &player) {
	if (player.position.y <= 0.0f) {
		player.position.y = 0.0f;
		player.collision.grounded = true;
		if (player.movement.velocity.y < 0)
			player.movement.velocity.y = 0;
	}
}

void speedCheck(Player &player) {
	float currentSpeed = sqrtf(player.movement.velocity.x * player.movement.velocity.x + player.movement.velocity.z * player.movement.velocity.z);
	if (currentSpeed > player.movement.speed) {
		player.movement.velocity.x = (player.movement.velocity.x / currentSpeed) * player.movement.speed;
		player.movement.velocity.z = (player.movement.velocity.z / currentSpeed) * player.movement.speed;
	}
}

void applyJump(Player &player) {
	if (player.jump.bufferTimer > 0 && player.collision.grounded) {
		player.movement.velocity.y = 55.0f;
		player.jump.bufferTimer = 0;
	}
}

void bufferCountdown(float deltaTime, Player &player) {
	if (player.jump.bufferTimer > 0) {
		player.jump.bufferTimer -= deltaTime;
	}
}

void getCollidingBodies(Player &player, StaticBody allBodies[], int count) {
	player.collision.bodyCount = 0;
	for (int i = 0; i < count; i++) {
		if (CheckCollisionBoxes(player.collision.aabb, allBodies[i].aabb)) {
			if (player.collision.bodyCount < 16) {
				player.collision.bodies[player.collision.bodyCount++] = &allBodies[i];
			}
		}
	}
}

void resolveCollision(Player &player, StaticBody &body) {
	float overlapX = fmin(player.collision.aabb.max.x, body.aabb.max.x) - fmax(player.collision.aabb.min.x, body.aabb.min.x);
	float overlapY = fmin(player.collision.aabb.max.y, body.aabb.max.y) - fmax(player.collision.aabb.min.y, body.aabb.min.y);
	float overlapZ = fmin(player.collision.aabb.max.z, body.aabb.max.z) - fmax(player.collision.aabb.min.z, body.aabb.min.z);

	if (overlapX < overlapY && overlapX < overlapZ) {
		if (player.collision.aabb.min.x < body.aabb.min.x) {
			player.position.x -= overlapX;
		} else {
			player.position.x += overlapX;
		}
		player.movement.velocity.x = 0;
	} else if (overlapZ < overlapX && overlapZ < overlapY) {
		if (player.collision.aabb.min.z < body.aabb.min.z) {
			player.position.z -= overlapZ;
		} else {
			player.position.z += overlapZ;
		}
		player.movement.velocity.z = 0;
	} else {
		if (player.collision.aabb.min.y < body.aabb.min.y) {
			player.position.y -= overlapY;
			if (player.movement.velocity.y > 0)
				player.movement.velocity.y = 0;
		} else {
			player.position.y += overlapY;
			if (player.movement.velocity.y < 0)
				player.movement.velocity.y = 0;
			player.collision.grounded = true;
		}
	}

	player.updateAABB();
}

int physicsProcess(float deltaTime, Player &player, World &world, Camera3D &camera, StaticBody allBodies[], int bodyCount) {
	player.collision.grounded = false;

	applyFriction(deltaTime, player);
	applyGravity(deltaTime, player, world);

	player.position.x += player.movement.velocity.x * deltaTime;
	player.position.y += player.movement.velocity.y * deltaTime;
	player.position.z += player.movement.velocity.z * deltaTime;

	floorCheck(player);
	speedCheck(player);

	player.updateAABB();

	getCollidingBodies(player, allBodies, bodyCount);

	if (player.collision.bodyCount > 0) {
		for (int i = 0; i < player.collision.bodyCount; i++) {
			resolveCollision(player, *player.collision.bodies[i]);
		}
	}

	applyJump(player);
	bufferCountdown(deltaTime, player);

	Vector3 newCamPos = (Vector3){player.position.x, player.position.y + player.collision.height, player.position.z};
	Vector3 moveDiff = Vector3Subtract(newCamPos, camera.position);
	camera.position = newCamPos;
	camera.target = Vector3Add(camera.target, moveDiff);

	return player.collision.bodyCount;
}
