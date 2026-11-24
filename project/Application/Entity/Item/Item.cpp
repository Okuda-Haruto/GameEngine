#include "Item.h"
#include "Entity/Player/Player.h"
#include "ModelManager/ModelManager.h"
#include "Operation/Operation.h"
#include <numbers>

std::weak_ptr<Camera> Item::camera_;
std::weak_ptr<DirectionalLight> Item::directionalLight_;
std::weak_ptr<PointLight> Item::pointLight_;

void Item::Initialize(SRT transform, Player* player) {
	player_ = player;

	object_ = std::make_unique<Object>();
	object_->Initialize(ModelManager::GetInstance()->GetModel("resources/Item","Item_HotDog.obj"));
	object_->SetCamera(camera_.lock());
	object_->SetDirectionalLight(directionalLight_.lock());
	object_->SetPointLight(pointLight_.lock());
	transform_ = transform;

	Matrix4x4 rotateMatrix = MakeRotateMatrix(transform_.rotate);
	velocity_ = Normalize(rotateMatrix * Vector3{ 0.0f,0.0f,1.0f }) * speed_;

	lifeTime_ = 0.0f;
	isDead = false;

	BaseEntity::Initialize(transform.scale.x, CollisionID_Item_Attack);
}

void Item::Update() {

	if (!isDead) {
		transform_.rotate.y += std::numbers::pi_v<float> / 90;

		if (transform_.rotate.x > std::numbers::pi_v<float> *2)transform_.rotate.x -= std::numbers::pi_v<float> *2;
		if (transform_.rotate.y > std::numbers::pi_v<float> *2)transform_.rotate.y -= std::numbers::pi_v<float> *2;
		if (transform_.rotate.z > std::numbers::pi_v<float> *2)transform_.rotate.z -= std::numbers::pi_v<float> *2;
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
		player_->AddHP(1);
	}
}