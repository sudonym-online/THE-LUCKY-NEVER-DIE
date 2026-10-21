#include "objects.h"
#include "physics.h"
#include "raylib.h"
#include <cstdio>
#include <cstdlib>

static float numberOf(int id, const char *key) {
    const char *attr = Objects::Get(id, key);
    return attr ? atof(attr) : 0.0f;
}

static void deagleUse(int id, Vector3 origin, Vector3 direction) {
    int body = -1;
    RayCollision hit = raycastBox((Ray){origin, direction}, &body);
    if (hit.hit) printf("[Deagle] hit instance %d at %.1f\n", Objects::registry.bodies[body].instanceId, hit.distance);

    // 01 --- SPAWN PROJECTILE IF POSSIBLE

    Objects::Spawn();

    // 02 --- UPDATE AMMO COUNT

    // 03 --- TRIGGER RECOIL

}
OBJECT_FUNCTION(deagleUse);
