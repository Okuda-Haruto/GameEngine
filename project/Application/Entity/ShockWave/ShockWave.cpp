#include "ShockWave.h"
#include "Operation/Operation.h"
#include <numbers>
#include <Easing.h>

weak_ptr<Camera> ShockWave::camera_;
weak_ptr<DirectionalLight> ShockWave::directionalLight_;
weak_ptr<PointLight> ShockWave::pointLight_;

void ShockWave::Initialize(SRT transform, float range, float maxLifeTime, CollisionID id, std::unique_ptr<PrimitiveCylinder> cylinder) {

	range_ = range;
	maxLifeTime_ = maxLifeTime;
	transform_ = transform;


	cylinder_ = move(cylinder);

	lifeTime_ = 0.0f;
	isDead = false;

	cylinderMaterial_.color = { 1.0f,1.0f,1.0f,1.0f };
	cylinderMaterial_.uvTransform = MakeIdentity4x4();

	BaseEntity::Initialize(1.0f, id);
}

void ShockWave::Update() {

	if (lifeTime_ < maxLifeTime_) {
		lifeTime_ += 1.0f / 60.0f;
		if (lifeTime_ >= maxLifeTime_) {
			isDead = true;
		}
	}


	float scale = Easing::EaseOut(1.0f,range_,lifeTime_ / maxLifeTime_);
	transform_.scale = { scale ,3.0f,scale };

	BaseEntity::Update();
}

void ShockWave::Draw() {
	if (!isDead) {
		cylinder_->Draw(transform_, cylinderMaterial_);
	}
}

void ShockWave::IsCollision(uint8_t targetId) {

}