#include "ColliderObject.h"
#include <Operation/Operation.h>

void ColliderObject::Initialize(std::shared_ptr<Model> model, SRT transform) {
	transform_ = transform;

	colliderParent_ = std::make_shared<Matrix4x4>();
	*colliderParent_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	object_ = std::make_unique<Object>();
	object_->Initialize(model);
	object_->SetTransform(transform);

	colliders_ = std::make_shared<Colliders>();
	colliders_->Initialize(this);

	OBB obb;
	obb.center = { 0,0,0 };
	obb.orientations[0] = { 1,0,0 };
	obb.orientations[1] = { 0,1,0 };
	obb.orientations[2] = { 0,0,1 };
	obb.size = transform.scale;

	colliders_->AddOBBCollider(obb, CollisionID_Anything_Body, CollisionID_Anything_Body, colliderParent_);
	colliders_->Update();
}

void ColliderObject::Update() {
	*colliderParent_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	colliders_->Update();
}

void ColliderObject::Draw() {
	object_->Draw3D();
}