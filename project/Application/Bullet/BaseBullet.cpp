#include "BaseBullet.h"
#include <Operation/Operation.h>

void BaseBullet::Initialize(Vector3 size, CollisionID id) {
	colliderParent_ = std::shared_ptr<Matrix4x4>();
	*colliderParent_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	OBBCollider collider;
	collider.localOBB_.center = { 0.0f,0.0f,0.0f };
	collider.localOBB_.orientations[0] = { 1.0f,0.0f,0.0f };
	collider.localOBB_.orientations[1] = { 0.0f,1.0f,0.0f };
	collider.localOBB_.orientations[2] = { 0.0f,0.0f,1.0f };
	collider.localOBB_.size = size;
	collider.parentMatrix_ = colliderParent_;
	collider.sourceId_ = id;
	collider.targetId_ = id;
	colliders_ = std::make_unique<Colliders>();
	colliders_->Initialize(collider);
}

void BaseBullet::Update() {
	*colliderParent_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
}

void BaseBullet::Draw() {

}