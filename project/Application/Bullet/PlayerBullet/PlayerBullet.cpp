#include "PlayerBullet.h"

void PlayerBullet::Initialize(ModelHolder* modelHolder, Vector3 translate, Vector3 rotate, BasePlayerBulletType* bulletType) {
	modelHolder_ = modelHolder;
	transform_ = {
		{1.0f,1.0f,1.0f},
		rotate,
		translate,
	};
	collision_.center = transform_.translate;
	collision_.radius = 1.0f * transform_.scale.x;

	velocity_ = {};

	ChangeBulletType(bulletType);
}

void PlayerBullet::Update() {

}

void PlayerBullet::Draw() {

}

void PlayerBullet::ChangeBulletType(BasePlayerBulletType* nextBulletType) {
	delete bulletType_;
	bulletType_ = nextBulletType;
	nextBulletType->Initialize();
}