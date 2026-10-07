#include "player.h"
#include "objects.h"
#include "raymath.h"
#include <cmath>
#include <cstdio>

inline bool Player::isValid(int itemId) { return itemId > 0; }

static Vector3 scaleOf(int itemId) {
    Vector3 scale = {1.0f, 1.0f, 1.0f};
    const char *attr = Objects::Get(itemId, "scale");
    if (attr) sscanf(attr, "%f,%f,%f", &scale.x, &scale.y, &scale.z);
    return scale;
}

void Player::UpdateAABB() {
    float bodyHeight = collision.height + 0.5f;
    collision.aabb.min = (Vector3){position.x - collision.width / 2, position.y, position.z - collision.depth / 2};
    collision.aabb.max = (Vector3){position.x + collision.width / 2, position.y + bodyHeight, position.z + collision.depth / 2};
}

void Player::UpdateModelOrientation(Model *model, Camera3D camera) {
    Vector3 dir = Vector3Normalize(Vector3Subtract(camera.target, camera.position));

    yaw = atan2f(dir.x, dir.z);

    float horizontalDist = sqrtf(dir.x * dir.x + dir.z * dir.z);
    pitch = -atan2f(dir.y, horizontalDist);

    Matrix rotation = MatrixMultiply(MatrixRotateX(pitch), MatrixRotateY(yaw));
    model->transform = rotation;
}

void Player::DrawArms(Camera3D camera) {
    forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    right = Vector3Normalize(Vector3CrossProduct((Vector3){0, 1, 0}, forward));

    Vector3 baseOffset  = Vector3Add(Vector3Scale(forward, visual.armConfig.dist), Vector3Scale((Vector3){0, 1, 0}, visual.armConfig.height));
    Vector3 leftArmPos  = Vector3Add(camera.position, Vector3Add(baseOffset, Vector3Scale(right, -visual.armConfig.width))); 
    Vector3 rightArmPos = Vector3Add(camera.position, Vector3Add(baseOffset, Vector3Scale(right, visual.armConfig.width)));

    // ARMS
    UpdateModelOrientation(&visual.armModel, camera);

    DrawModelEx(visual.armModel, leftArmPos, (Vector3){0, 1, 0}, 0.0f, (Vector3){1, 1, 1}, RED); // TODO: get a actual left arm model.
    DrawModelEx(visual.armModel, rightArmPos, (Vector3){0, 1, 0}, 0.0f, (Vector3){1, 1, 1}, RED); 

    // HOLDING

    if(inventory.hand == 0) return;
    Vector3 handGripOffset = Vector3Add(Vector3Scale(forward, visual.heldModelConfig.dist), Vector3Scale(right, visual.heldModelConfig.side));
    handGripPosition = Vector3Add(camera.position, Vector3Add(baseOffset, handGripOffset));

    UpdateModelOrientation(&visual.heldModel, camera);
    DrawModelEx(visual.heldModel, handGripPosition, (Vector3){0, 1, 0}, 0.0f, visual.heldModelScale, RED);
}

void Player::Stash(int itemId) {
    if (!isValid(itemId)) return;
    for (int i = 0; i < MAX_INVENTORY_SIZE; i++)
        if (inventory.items[i] == itemId) return;

    for (int i = 0; i < MAX_INVENTORY_SIZE; i++) {
        if (inventory.items[i] == 0) {
            inventory.items[i] = itemId;
            break;
        }
    }
    Objects::Despawn(itemId);
}

bool Player::Unstash(int itemId) {
    if (!isValid(itemId)) return false;

    bool found = false;
    for (int i = 0; i < MAX_INVENTORY_SIZE; i++) {
        if (inventory.items[i] == itemId) {
            inventory.items[i] = 0;
            found = true;
            break;
        }
    }
    if (!found) return false;

    Vector3 dropPos = Vector3Add(position, Vector3Scale(forward, 3.0f));
    Objects::Spawn(itemId, dropPos, scaleOf(itemId), 0.0f);
    return true;
}

void Player::Hold(int itemId) {
    if (itemId == -1) {
        if (inventory.hand != 0) {
            UnloadModel(visual.heldModel);
            inventory.hand = 0;
        }
        return;
    }

    if (!isValid(itemId) || inventory.hand == itemId) return;

    const char *modelPath = Objects::Get(itemId, "model");
    if (!modelPath) return; 

    inventory.hand = itemId;
    visual.heldModel = LoadModel(modelPath);

    visual.heldModelScale = scaleOf(itemId);
}

void Player::Use(bool down) {

}