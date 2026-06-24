#include "game.h"
#include "raymath.h"
#include <cmath>
#include <float.h>

void StaticBody::UpdateAABB() {
	BoundingBox modelAABB = GetModelBoundingBox(model);

	Matrix transform = MatrixScale(scale.x, scale.y, scale.z);
	transform = MatrixMultiply(transform, MatrixRotateY(rotation * DEG2RAD));
	transform = MatrixMultiply(transform, MatrixTranslate(position.x, position.y, position.z));

	Vector3 corners[8] = {
			{modelAABB.min.x, modelAABB.min.y, modelAABB.min.z},
			{modelAABB.min.x, modelAABB.min.y, modelAABB.max.z},
			{modelAABB.min.x, modelAABB.max.y, modelAABB.min.z},
			{modelAABB.min.x, modelAABB.max.y, modelAABB.max.z},
			{modelAABB.max.x, modelAABB.min.y, modelAABB.min.z},
			{modelAABB.max.x, modelAABB.min.y, modelAABB.max.z},
			{modelAABB.max.x, modelAABB.max.y, modelAABB.min.z},
			{modelAABB.max.x, modelAABB.max.y, modelAABB.max.z}};

	for (int i = 0; i < 8; i++)
		corners[i] = Vector3Transform(corners[i], transform);

	aabb.min = corners[0];
	aabb.max = corners[0];

	for (int i = 1; i < 8; i++) {
		aabb.min.x = fminf(aabb.min.x, corners[i].x);
		aabb.min.y = fminf(aabb.min.y, corners[i].y);
		aabb.min.z = fminf(aabb.min.z, corners[i].z);
		aabb.max.x = fmaxf(aabb.max.x, corners[i].x);
		aabb.max.y = fmaxf(aabb.max.y, corners[i].y);
		aabb.max.z = fmaxf(aabb.max.z, corners[i].z);
	}
}

void StaticBody::draw() {
	DrawModelEx(model, position, (Vector3){0, 1, 0}, rotation, scale, WHITE);
}