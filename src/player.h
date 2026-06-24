#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

class Player {
public:
    struct Inventory {
        struct Item {
            int id;
            const char *name;
        };
        Item items[32];
        int count = 0;
    } inventory;

    struct Movement {
        Vector3 velocity = {0.0f, 0.0f, 0.0f};
        float speed = 50.0f;
        float acceleration = 320.0f;
        float friction = 3.5f;
    } movement;

    struct Collision {
        float height = 5.0f;
        float width = 1.0f;
        float depth = 1.0f;
        BoundingBox aabb;
        StaticBody *bodies[16];
        int bodyCount = 0;
        bool grounded = false;
    } collision;

    struct Jump {
        float bufferTime = 0.2f;
        float bufferTimer = 0.0f;
    } jump;

    struct Visuals {
        Model armModel;
        struct ArmConfig {
            float dist = 0.8f;
            float height = -2.8f;
            float width = 4.5f;
        } armConfig;
    } visual;

    Vector3 position = {0.0f, 0.0f, 0.0f};

    void updateAABB();
    void UpdateModelOrientation(Model *model, Camera3D camera);
    void drawArms(Camera3D camera);
};

#endif