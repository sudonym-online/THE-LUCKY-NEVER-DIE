#ifndef PHYSICS_H
#define PHYSICS_H

#include "game.h"
#include "player.h"

void physicsProcess(float deltaTime, Player &player, Camera3D &camera);
RayCollision raycastBox(Ray ray, int *outBody = nullptr);
RayCollision raycastMesh(Ray ray, int *outBody = nullptr);
float getBoost();

#endif
