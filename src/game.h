#ifndef GAME_H
#define GAME_H

#include "raylib.h"

struct StaticBody {
    Model model;
    Vector3 position;
    float rotation;
    Vector3 scale;
    BoundingBox aabb;

    void UpdateAABB();
    void draw();
};

struct Item {
    int id;
    const char *name;
};

enum { MAX_ITEMS = 32 };

struct World {
    float gravity = 120.0f;
};

#endif
