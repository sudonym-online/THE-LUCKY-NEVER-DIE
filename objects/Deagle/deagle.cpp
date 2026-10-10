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
    float ammo = numberOf(id, "ammo");
    if (ammo <= 0 || GetTime() < numberOf(id, "nextFire")) return;

    char buf[32];
    snprintf(buf, sizeof(buf), "%d", (int)ammo - 1);
    Objects::Set(id, "ammo", buf);
    snprintf(buf, sizeof(buf), "%.3f", GetTime() + numberOf(id, "fireRate"));
    Objects::Set(id, "nextFire", buf);

    int body = -1;
    RayCollision hit = raycastBox((Ray){origin, direction}, &body);
    if (hit.hit) printf("[Deagle] hit instance %d at %.1f\n", Objects::registry.bodies[body].instanceId, hit.distance);
}
OBJECT_FUNCTION(deagleUse);
