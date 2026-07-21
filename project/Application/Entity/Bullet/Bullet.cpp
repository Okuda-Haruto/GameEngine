#include "Bullet.h"
#include "Operation/Operation.h"
#include <numbers>

weak_ptr<Camera> Bullet::camera_;
weak_ptr<DirectionalLight> Bullet::directionalLight_;
weak_ptr<PointLight> Bullet::pointLight_;

void Bullet::Initialize(SRT transform, float speed, CollisionID id, Bullet::BulletMove bulletMove, std::shared_ptr<Model> model, std::unique_ptr<BaseEvent> event) {
	transform_ = transform;
	speed_ = speed;
	bulletMove_ = bulletMove;
	deadEvent_ = move(event);

	object_ = std::make_unique<Object>();
	object_->Initialize(model);
	object_->SetCamera(camera_.lock());
	object_->SetDirectionalLight(directionalLight_.lock());
	object_->SetPointLight(pointLight_.lock());
	object_->SetCubeTextureIndex(TextureManager::GetInstance()->GetSrvIndex("resources/BackGround/minedump_flats_1k.dds"));
	std::vector<Parts> parts = object_->GetParts();
	parts[0].material->enviromentCoefficient = 1.0f;
	object_->SetParts(parts[0], 0);

	Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);
	velocity_ = Normalize(rotateMatrix * Vector3{ 0.0f,0.0f,1.0f }) * speed_;

	lifeTime_ = 0.0f;
	isDead_ = false;

	BaseEntity::Initialize(1.0f, id);
}

void Bullet::Update() {

	if (lifeTime_ < kMaxLifeTime_) {
		lifeTime_ += 1.0f / 60.0f;
		if (lifeTime_ >= kMaxLifeTime_) {
			isDead_ = true;
		}
	}

	if (transform_.rotate.x > std::numbers::pi_v<float> *2)transform_.rotate.x -= std::numbers::pi_v<float> *2;
	if (transform_.rotate.y > std::numbers::pi_v<float> *2)transform_.rotate.y -= std::numbers::pi_v<float> *2;
	if (transform_.rotate.z > std::numbers::pi_v<float> *2)transform_.rotate.z -= std::numbers::pi_v<float> *2;

	if (!isDead_) {
		switch (bulletMove_)
		{
		case Bullet::BulletMove::NormalBullet:
			transform_.translate += velocity_;
			break;
		case Bullet::BulletMove::FallingBullet:
			velocity_.y -= 0.1f;
			transform_.translate += velocity_;
			break;
		case Bullet::BulletMove::HomingBullet:
			break;
		default:
			break;
		}
	}

	object_->SetTransform(transform_);
	BaseEntity::Update();
}

void Bullet::Draw() {
	if (!isDead_) {
		object_->Draw3D();
	}
}

void Bullet::IsCollision(uint8_t targetId) {

	isDead_ = true;

	if (isDead_ && deadEvent_) {
		deadEvent_->Play(transform_);
	}
}

void Bullet::IsCollisionGround(OBB obb) {

	isDead_ = true;

}