#include "player.h"
#include "raymath.h"
#include <cmath>
#include <float.h>

void Player::updateAABB() {
    float bodyHeight = player.collision.height + 0.5f;
    player.collision.aabb.min = (Vector3){player.position.x - player.collision.width / 2, player.position.y, player.position.z - player.collision.depth / 2};
    player.collision.aabb.max = (Vector3){player.position.x + player.collision.width / 2, player.position.y + bodyHeight, player.position.z + player.collision.depth / 2};
}

void Player::UpdateModelOrientation(Model *model, Camera3D camera) {
    Vector3 dir = Vector3Subtract(camera.target, camera.position);
    dir = Vector3Normalize(dir);

    float yaw = atan2f(dir.x, dir.z);

    float horizontalDist = sqrtf(dir.x * dir.x + dir.z * dir.z);
    float pitch = -atan2f(dir.y, horizontalDist);

    Matrix rotation = MatrixMultiply(MatrixRotateX(pitch), MatrixRotateY(yaw));
    model->transform = rotation;
}

void Player::drawArms(Camera3D camera) {
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct((Vector3){0, 1, 0}, forward));

    Vector3 baseOffset = Vector3Add(Vector3Scale(forward, visual.armConfig.dist),
        Vector3Scale((Vector3){0, 1, 0}, visual.armConfig.height));
    Vector3 leftArmPos = Vector3Add(camera.position,
        Vector3Add(baseOffset, Vector3Scale(right, -visual.armConfig.width)));
    Vector3 rightArmPos = Vector3Add(camera.position,
        Vector3Add(baseOffset, Vector3Scale(right, visual.armConfig.width)));

    UpdateModelOrientation(&visual.armModel, camera);

    DrawModelEx(visual.armModel, leftArmPos, (Vector3){0, 1, 0}, 0.0f, (Vector3){1, 1, 1}, RED);
    DrawModelEx(visual.armModel, rightArmPos, (Vector3){0, 1, 0}, 0.0f, (Vector3){1, 1, 1}, RED);
}