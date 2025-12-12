#include "PlayerBullet.h"
#include "Operation/Operation.h"
#include <numbers>

Camera*  PlayerBullet::camera_ = nullptr;
DirectionalLight* PlayerBullet::directionalLight_ = nullptr;
PointLight* PlayerBullet::pointLight_ = nullptr;

void PlayerBullet::Initialize(Vector3 translate, Vector3 rotate) {
	object_ = std::make_unique<Object>();
	object_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Bullet));
	object_->SetCamera(camera_);
	object_->SetDirectionalLight(directionalLight_);
	object_->SetPointLight(pointLight_);
	transform_ = {
		{1.0f,1.0f,1.0f},
		rotate,
		translate,
	};

	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotate);
	velocity_.translate = Normalize(rotateMatrix * Vector3{0.0f,0.0f,1.0f}) * speed_;

	lifeTime_ = 0.0f;
	isDead = false;

	InitializeCollider(1.0f, CollisionID_Player_Bullet);
}

void PlayerBullet::Update() {

	if (lifeTime_ < kMaxLifeTime_) {
		lifeTime_ += 1.0f / 60.0f;
		if (lifeTime_ >= kMaxLifeTime_) {
			isDead = true;
		}
	}
	if (!isDead) {
		transform_.rotate += velocity_.rotate;
		if (transform_.rotate.x > std::numbers::pi_v<float> *2)transform_.rotate.x -= std::numbers::pi_v<float> *2;
		if (transform_.rotate.y > std::numbers::pi_v<float> *2)transform_.rotate.y -= std::numbers::pi_v<float> *2;
		if (transform_.rotate.z > std::numbers::pi_v<float> *2)transform_.rotate.z -= std::numbers::pi_v<float> *2;
		transform_.translate += velocity_.translate;
	}

	object_->SetTransform(transform_);
	UpdateCollider();
}

void PlayerBullet::Draw() {
	if (!isDead) {
		object_->Draw3D();
	}
}

void PlayerBullet::IsCollision() {
	isDead = true;
}