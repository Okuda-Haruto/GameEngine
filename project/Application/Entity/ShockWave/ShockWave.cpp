#include "ShockWave.h"
#include "Operation/Operation.h"
#include <numbers>

weak_ptr<Camera> ShockWave::camera_;
weak_ptr<DirectionalLight> ShockWave::directionalLight_;
weak_ptr<PointLight> ShockWave::pointLight_;

void ShockWave::Initialize(SRT transform, float range, float maxLifeTime, CollisionID id, std::unique_ptr<PrimitiveCylinder> cylinder) {

	range_ = range;
	maxLifeTime_ = maxLifeTime;

	cylinder_ = move(cylinder);

	lifeTime_ = 0.0f;
	isDead = false;

	BaseEntity::Initialize(1.0f, id);
}

void ShockWave::Update() {

	if (lifeTime_ < maxLifeTime_) {
		lifeTime_ += 1.0f / 60.0f;
		if (lifeTime_ >= maxLifeTime_) {
			isDead = true;
		}
	}



	object_->SetTransform(transform_);
	BaseEntity::Update();
}

void ShockWave::Draw() {
	if (!isDead) {
		object_->Draw3D();
	}
}

void ShockWave::IsCollision(uint8_t targetId) {

	if (!(targetId & 0b0010) &&	//敵側ではない
		(targetId & 0b1000 ||	//衝突するもの
			targetId & CollisionID_Player_Attack)) {	//プレイヤー攻撃

		isDead = true;
	}
}