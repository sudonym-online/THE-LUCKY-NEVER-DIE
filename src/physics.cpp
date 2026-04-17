#include "../include/physics.h"

void applyFriction(float deltaTime, Player &player) {
    player.velocity.x *= (1.0f - player.friction * deltaTime);
    player.velocity.y *= (1.0f - player.friction * deltaTime);
}

void applyGravity(float deltaTime, Player &player, World &world, Camera3D &camera) {
    if (camera.position.y > player.height) {
        player.velocity.z -= world.gravity * deltaTime;
    }
}

void floorCheck(Player &player, Camera3D &camera) {
    if (camera.position.y <= player.height) {
        camera.position.y = player.height;
        player.velocity.z = 0;
    }
}

void speedCheck(Player &player) {
    float currentSpeed = Vector2Length((Vector2){player.velocity.x, player.velocity.y});
    if (currentSpeed > player.speed) {
        player.velocity.x = (player.velocity.x / currentSpeed) * player.speed;
        player.velocity.y = (player.velocity.y / currentSpeed) * player.speed;
    }
}

void applyJump(Player &player, Camera3D &camera) {
    bool onGround = camera.position.y <= player.height;
    if (player.jumpBufferTimer > 0 && onGround) {
        player.velocity.z = player.speed + 20.0f;
        player.jumpBufferTimer = 0;
    }
}

void bufferCountdown(float deltaTime, Player &player) {
    if (player.jumpBufferTimer > 0) {
        player.jumpBufferTimer -= deltaTime;
    }
}

void physicsProcess(float deltaTime, Player &player, World &world, Camera3D &camera) {
    applyFriction(deltaTime, player);
    applyGravity(deltaTime, player, world, camera);
    floorCheck(player, camera);
    speedCheck(player);
    applyJump(player, camera);
    bufferCountdown(deltaTime, player);
}
