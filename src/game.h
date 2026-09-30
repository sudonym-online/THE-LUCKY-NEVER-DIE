#ifndef GAME_H
#define GAME_H

#include "raylib.h"

struct Triangle {
    Vector3             v0, v1, v2;
    Vector3             normal;
};

namespace Objects { struct ObjectRegistry; }

struct StaticBody {
    BoundingBox         aabb;
    Color               tint =              WHITE;
    Model               model;
    Vector3             position;
    Vector3             scale;
    float               rotation;
    int                 triOffset;
    int                 triCount;
    int                 instanceId =        0;

    Matrix Transform();
    void UpdateAABB();
    void ExtractTriangles(Objects::ObjectRegistry &reg);
    void draw();
};

#endif
