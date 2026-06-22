#ifndef PHYSICS_H
#define PHYSICS_H

#include "game.h"

int physicsProcess(float deltaTime, Player &player, World &world, Camera3D &camera);
void getCollidingBodies(Player &player, StaticBody allBodies[], int count);
void resolveCollision(Player &player, StaticBody &body);

#endif
