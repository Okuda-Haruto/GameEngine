#include "Colliders.h"
#include <Operation/Operation.h>
#include <Collision.h>

void Colliders::Initialize(Collider* collider) {
	collider_ = collider;
}

void Colliders::Update() {

	for (int i = 0; i < sphereColliders_.size(); i++) {
		sphereColliders_[i].colliderSphere_.center = sphereColliders_[i].localSphere_.center * *(sphereColliders_[i].parentMatrix_.get());
		sphereColliders_[i].colliderSphere_.radius = sphereColliders_[i].localSphere_.radius;
	}

	for (int i = 0; i < obbColliders_.size(); i++) {
		obbColliders_[i].colliderOBB_.center = obbColliders_[i].localOBB_.center * *(obbColliders_[i].parentMatrix_.get());
		obbColliders_[i].colliderOBB_.orientations[0] = obbColliders_[i].localOBB_.orientations[0];
		obbColliders_[i].colliderOBB_.orientations[1] = obbColliders_[i].localOBB_.orientations[1];
		obbColliders_[i].colliderOBB_.orientations[2] = obbColliders_[i].localOBB_.orientations[2];
		obbColliders_[i].colliderOBB_.size = obbColliders_[i].localOBB_.size;
	}
}

void Colliders::AddSphereCollider(Sphere localSphere, int8_t sourceId_, int8_t targetId_, std::shared_ptr<Matrix4x4> parentMatrix) {
	SphereCollider collider;
	
	collider.localSphere_ = localSphere;
	collider.sourceId_ = sourceId_;
	collider.targetId_ = targetId_;
	collider.parentMatrix_ = parentMatrix;

	sphereColliders_.push_back(collider);
}

void Colliders::AddOBBCollider(OBB localOBB, int8_t sourceId_, int8_t targetId_, std::shared_ptr<Matrix4x4> parentMatrix) {
	OBBCollider collider;

	collider.localOBB_ = localOBB;
	collider.sourceId_ = sourceId_;
	collider.targetId_ = targetId_;
	collider.parentMatrix_ = parentMatrix;

	obbColliders_.push_back(collider);
}


void Colliders::SetGrundCollider(OBB localOBB, int8_t sourceId_, int8_t targetId_) {
	OBBCollider collider;

	collider.localOBB_ = localOBB;
	collider.sourceId_ = sourceId_;
	collider.targetId_ = targetId_;

	grundCollider_ = collider;
}