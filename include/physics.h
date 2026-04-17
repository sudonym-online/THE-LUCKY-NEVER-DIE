#ifndef PHYSICS_H
#define PHYSICS_H

#include "game.h"

int physicsProcess(float deltaTime, Player &player, World &world, Camera3D &camera, StaticBody allBodies[], int bodyCount);
int getCollidingBodies(Player &player, StaticBody allBodies[], int count, StaticBody *outColliding[]);
void resolveCollision(Player &player, Camera3D &camera, StaticBody &body);

#endif
