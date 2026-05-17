#include "Colliders.h"

void Colliders::Initialize(OBBCollider grundCollider) {
	grundCollider_ = grundCollider;
}

void Colliders::Update() {

}

void Colliders::AddSphereCollider(Sphere localSphere, int8_t sourceId_, int8_t targetId_) {
	SphereCollider collider;
	
	collider.localSphere_ = localSphere;
	collider.sourceId_ = sourceId_;
	collider.targetId_ = targetId_;

	sphereColliders_.push_back(collider);
}

void Colliders::AddOBBCollider(OBB localOBB, int8_t sourceId_, int8_t targetId_) {
	OBBCollider collider;

	collider.localOBB_ = localOBB;
	collider.sourceId_ = sourceId_;
	collider.targetId_ = targetId_;

	obbColliders_.push_back(collider);
}