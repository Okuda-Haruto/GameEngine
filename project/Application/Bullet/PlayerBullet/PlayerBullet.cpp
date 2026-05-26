#include "PlayerBullet.h"
#include "Operation/Operation.h"
#include <numbers>

weak_ptr<Camera>  PlayerBullet::camera_;
weak_ptr<DirectionalLight> PlayerBullet::directionalLight_;
weak_ptr<PointLight> PlayerBullet::pointLight_;

void PlayerBullet::Initialize(Vector3 translate, Vector3 rotate) {
	object_ = std::make_unique<Object>();
	object_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Bullet));
	object_->SetCamera(camera_.lock());
	object_->SetDirectionalLight(directionalLight_.lock());
	object_->SetPointLight(pointLight_.lock());
	object_->SetCubeTextureIndex(TextureManager::GetInstance()->GetSrvIndex("resources/BackGround/minedump_flats_1k.dds"));
	std::vector<Parts> parts = object_->GetParts();
	parts[0].material->enviromentCoefficient = 1.0f;
	object_->SetParts(parts[0], 0);

	transform_ = {
		{1.0f,1.0f,1.0f},
		rotate,
		translate,
	};

	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotate);
	velocity_.translate = Normalize(rotateMatrix * Vector3{0.0f,0.0f,1.0f}) * speed_;

	lifeTime_ = 0.0f;
	isDead = false;

	BaseBullet::Initialize(1.0f,CollisionID_Player_Attack);
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

	BaseBullet::Update();
}

void PlayerBullet::Draw() {
	if (!isDead) {
		object_->Draw3D();
	}
}

void PlayerBullet::IsCollision(uint8_t targetId) {

	if (!(targetId & 0b0001) &&	//プレイヤー側ではない
		(targetId & 0b1000 ||	//衝突するもの
			targetId & CollisionID_Enemy_Attack)) {	//敵攻撃

		isDead = true;
	}
}

void PlayerBullet::IsCollisionGround(OBB obb) {

	isDead = true;

}