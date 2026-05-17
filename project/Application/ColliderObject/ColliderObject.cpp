#include "ColliderObject.h"

void ColliderObject::Initialize(std::shared_ptr<Model> model, SRT transform) {
	object_ = std::make_unique<Object>();
	object_->Initialize(model);
	object_->SetTransform(transform);

	collider_ = std::make_shared<OBBCollider>();
	collider_->colliderOBB_.center = transform.translate;
	collider_->colliderOBB_.orientations[0] = { 1,0,0 };
	collider_->colliderOBB_.orientations[1] = { 0,1,0 };
	collider_->colliderOBB_.orientations[2] = { 0,0,1 };
	collider_->colliderOBB_.size = transform.scale;
}

void ColliderObject::Update() {

}

void ColliderObject::Draw() {
	object_->Draw3D();
}