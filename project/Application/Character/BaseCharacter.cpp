#include "BaseCharacter.h"
#include <Operation/Operation.h>

void BaseCharacter::Initialize(float radius, CollisionID id) {
	colliderParent_ = std::make_shared<Matrix4x4>();
	*colliderParent_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	colliders_ = std::make_unique<Colliders>();
	colliders_->Initialize(this);

	Sphere sphere;
	sphere.center = { 0.0f,0.0f,0.0f };
	sphere.radius = radius;

	colliders_->AddSphereCollider(sphere, id, id, colliderParent_);
	colliders_->Update();
}

void BaseCharacter::Update() {
	*colliderParent_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	colliders_->Update();
}

void BaseCharacter::Draw() {

}