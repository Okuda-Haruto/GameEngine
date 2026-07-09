#include "Colliders.h"
#include <Operation/Operation.h>
#include <Collision.h>

void Colliders::Initialize(Collider* collider) {
	collider_ = collider;
}

void Colliders::Update() {

	for (int i = 0; i < sphereColliders_.size(); i++) {
		sphereColliders_[i].colliderSphere.center = sphereColliders_[i].localSphere.center * *(sphereColliders_[i].parentMatrix.get());
		sphereColliders_[i].colliderSphere.radius = sphereColliders_[i].localSphere.radius;
	}

	for (int i = 0; i < obbColliders_.size(); i++) {
		obbColliders_[i].colliderOBB.center = obbColliders_[i].localOBB.center * *(obbColliders_[i].parentMatrix.get());
		obbColliders_[i].colliderOBB.orientations[0] = obbColliders_[i].localOBB.orientations[0];
		obbColliders_[i].colliderOBB.orientations[1] = obbColliders_[i].localOBB.orientations[1];
		obbColliders_[i].colliderOBB.orientations[2] = obbColliders_[i].localOBB.orientations[2];
		obbColliders_[i].colliderOBB.size = obbColliders_[i].localOBB.size;
	}
}

void Colliders::AddSphereCollider(Sphere localSphere, int8_t sourceId_, int8_t targetId_, std::shared_ptr<Matrix4x4> parentMatrix) {
	SphereCollider collider;
	
	collider.localSphere = localSphere;
	collider.sourceId = sourceId_;
	collider.targetId = targetId_;
	collider.parentMatrix = parentMatrix;

	sphereColliders_.push_back(collider);
}

void Colliders::AddOBBCollider(OBB localOBB, int8_t sourceId_, int8_t targetId_, std::shared_ptr<Matrix4x4> parentMatrix) {
	OBBCollider collider;

	collider.localOBB = localOBB;
	collider.sourceId = sourceId_;
	collider.targetId = targetId_;
	collider.parentMatrix = parentMatrix;

	obbColliders_.push_back(collider);
}


void Colliders::SetGrundCollider(Capsule localCapsule, int8_t sourceId_, int8_t targetId_) {
	CapsuleCollider collider;

	collider.localCapsule = localCapsule;
	collider.sourceId = sourceId_;
	collider.targetId = targetId_;

	grundCollider_ = collider;
}