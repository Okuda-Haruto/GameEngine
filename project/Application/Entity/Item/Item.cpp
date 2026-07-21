#include "Item.h"
#include <ModelManager/ModelManager.h>
#include "Operation/Operation.h"
#include <numbers>

weak_ptr<Camera> Item::camera_;
weak_ptr<DirectionalLight> Item::directionalLight_;
weak_ptr<PointLight> Item::pointLight_;

void Item::Initialize(SRT transform) {
	object_ = std::make_unique<Object>();
	object_->Initialize(ModelManager::GetInstance()->GetModel("resources/DebugResources/sphere","sphere.obj"));
	object_->SetCamera(camera_.lock());
	object_->SetDirectionalLight(directionalLight_.lock());
	object_->SetPointLight(pointLight_.lock());
	object_->SetCubeTextureIndex(TextureManager::GetInstance()->GetSrvIndex("resources/BackGround/minedump_flats_1k.dds"));
	std::vector<Parts> parts = object_->GetParts();
	parts[0].material->enviromentCoefficient = 1.0f;
	object_->SetParts(parts[0], 0);
	transform_ = transform;

	Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);
	velocity_ = Normalize(rotateMatrix * Vector3{ 0.0f,0.0f,1.0f }) * speed_;

	lifeTime_ = 0.0f;
	isDead = false;

	BaseEntity::Initialize(1.0f, CollisionID_Item_Attack);
}

void Item::Update() {

	if (lifeTime_ < kMaxLifeTime_) {
		lifeTime_ += 1.0f / 60.0f;
		if (lifeTime_ >= kMaxLifeTime_) {
			isDead = true;
		}
	}
	if (!isDead) {
		if (transform_.rotate.x > std::numbers::pi_v<float> *2)transform_.rotate.x -= std::numbers::pi_v<float> *2;
		if (transform_.rotate.y > std::numbers::pi_v<float> *2)transform_.rotate.y -= std::numbers::pi_v<float> *2;
		if (transform_.rotate.z > std::numbers::pi_v<float> *2)transform_.rotate.z -= std::numbers::pi_v<float> *2;
		transform_.translate += velocity_;
	}

	object_->SetTransform(transform_);
	BaseEntity::Update();
}

void Item::Draw() {
	if (!isDead) {
		object_->Draw3D();
	}
}

void Item::IsCollision(uint8_t targetId) {

	if (!(targetId & 0b0010) &&	//敵側ではない
		(targetId & 0b1000 ||	//衝突するもの
			targetId & CollisionID_Player_Attack)) {	//プレイヤー攻撃

		isDead = true;
	}
}