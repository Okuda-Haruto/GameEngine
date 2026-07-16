#include "Bomb.h"
#include "Operation/Operation.h"
#include <numbers>

weak_ptr<Camera> Bomb::camera_;
weak_ptr<DirectionalLight> Bomb::directionalLight_;
weak_ptr<PointLight> Bomb::pointLight_;

void Bomb::Initialize(SRT transform, float range, float maxLifeTime, CollisionID id, std::shared_ptr<Model> model) {

	range_ = range;
	maxLifeTime_ = maxLifeTime;

	object_ = std::make_unique<Object>();
	object_->Initialize(model);
	object_->SetCamera(camera_.lock());
	object_->SetDirectionalLight(directionalLight_.lock());
	object_->SetPointLight(pointLight_.lock());
	object_->SetCubeTextureIndex(TextureManager::GetInstance()->GetSrvIndex("resources/BackGround/minedump_flats_1k.dds"));
	std::vector<Parts> parts = object_->GetParts();
	parts[0].material->enviromentCoefficient = 1.0f;
	object_->SetParts(parts[0], 0);
	transform_ = transform;

	lifeTime_ = 0.0f;
	isDead = false;

	BaseEntity::Initialize(1.0f, id);
}

void Bomb::Update() {

	if (lifeTime_ < maxLifeTime_) {
		lifeTime_ += 1.0f / 60.0f;
		if (lifeTime_ >= maxLifeTime_) {
			isDead = true;
		}
	}

	object_->SetTransform(transform_);
	BaseEntity::Update();
}

void Bomb::Draw() {
	if (!isDead) {
		object_->Draw3D();
	}
}

void Bomb::IsCollision(uint8_t targetId) {
	
}