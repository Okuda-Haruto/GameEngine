#include "ColliderObject.h"

void ColliderObject::Initialize(std::shared_ptr<Model> model, SRT transform) {
	object_->Initialize(model);
	object_->SetTransform(transform);

	collider_->GetOBB();
}

void ColliderObject::Update() {

}

void ColliderObject::Draw() {
	object_->Draw3D();
}