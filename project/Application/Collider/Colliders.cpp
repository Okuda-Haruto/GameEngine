#define NOMINMAX
#include "Colliders.h"
#include <Operation/Operation.h>
#include <Collision.h>
#include <algorithm>
#include <PrimitiveManager/PrimitiveManager.h>

void Colliders::Initialize(Collider* collider) {
	collider_ = collider;
}

void Colliders::Update() {

	for (int i = 0; i < sphereColliders_.size(); i++) {
		sphereColliders_[i].colliderSphere.center = sphereColliders_[i].localSphere.center * *(sphereColliders_[i].parentMatrix.get());
		float sx = Length(Vector3(sphereColliders_[i].parentMatrix.get()->m[0][0], sphereColliders_[i].parentMatrix.get()->m[0][1], sphereColliders_[i].parentMatrix.get()->m[0][2]));
		float sy = Length(Vector3(sphereColliders_[i].parentMatrix.get()->m[1][0], sphereColliders_[i].parentMatrix.get()->m[1][1], sphereColliders_[i].parentMatrix.get()->m[1][2]));
		float sz = Length(Vector3(sphereColliders_[i].parentMatrix.get()->m[2][0], sphereColliders_[i].parentMatrix.get()->m[2][1], sphereColliders_[i].parentMatrix.get()->m[2][2]));

		float scale = std::max({ sx, sy, sz });

		sphereColliders_[i].colliderSphere.radius = sphereColliders_[i].localSphere.radius * scale;
#ifdef USE_IMGUI
		PrimitiveManager::GetInstance()->AddSphere(sphereColliders_[i].colliderSphere);
#endif // USE_IMGUI
	}

	for (int i = 0; i < obbColliders_.size(); i++) {
		obbColliders_[i].colliderOBB.center = obbColliders_[i].localOBB.center * *(obbColliders_[i].parentMatrix.get());
		obbColliders_[i].colliderOBB.orientations[0] = obbColliders_[i].localOBB.orientations[0];
		obbColliders_[i].colliderOBB.orientations[1] = obbColliders_[i].localOBB.orientations[1];
		obbColliders_[i].colliderOBB.orientations[2] = obbColliders_[i].localOBB.orientations[2];
		obbColliders_[i].colliderOBB.size = obbColliders_[i].localOBB.size;
#ifdef USE_IMGUI
		PrimitiveManager::GetInstance()->AddOBB(obbColliders_[i].colliderOBB);
#endif // USE_IMGUI
	}
	if (grundCollider_.parentMatrix) {
		grundCollider_.colliderSphere.center = grundCollider_.localSphere.center * *(grundCollider_.parentMatrix.get());
		grundCollider_.colliderSphere.radius = grundCollider_.localSphere.radius;
	}
}

void Colliders::AddSphereCollider(Sphere localSphere, int8_t sourceId, int8_t targetId, std::shared_ptr<Matrix4x4> parentMatrix) {
	SphereCollider collider;
	
	collider.localSphere = localSphere;
	collider.sourceId = sourceId;
	collider.targetId = targetId;
	collider.parentMatrix = parentMatrix;

	sphereColliders_.push_back(collider);
}

void Colliders::AddOBBCollider(OBB localOBB, int8_t sourceId, int8_t targetId, std::shared_ptr<Matrix4x4> parentMatrix) {
	OBBCollider collider;

	collider.localOBB = localOBB;
	collider.sourceId = sourceId;
	collider.targetId = targetId;
	collider.parentMatrix = parentMatrix;

	obbColliders_.push_back(collider);
}


void Colliders::SetGrundCollider(Sphere localSphere, int8_t sourceId, int8_t targetId, std::shared_ptr<Matrix4x4> parentMatrix) {
	SphereCollider collider;

	collider.localSphere = localSphere;
	collider.sourceId = sourceId;
	collider.targetId = targetId;
	collider.parentMatrix = parentMatrix;

	grundCollider_ = collider;
}