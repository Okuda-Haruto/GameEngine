#include "Hat.h"
#include <ModelHolder/ModelHolder.h>
#include <Entity/Player/Player.h>
#include <numbers>

void Hat::Initialize(std::shared_ptr<DirectionalLight> directionalLight, Player* player) {
	player_ = player;

	object_ = std::make_unique<Object>();
	object_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Hat));
	object_->SetDirectionalLight(directionalLight);
	transform_.scale = { 0.0003f,0.0003f,0.0003f };
	transform_.rotate.x = std::numbers::pi_v<float> / 180 * -25;
	transform_.translate = { -0.0038f,0.0018f,0.01f };
	object_->SetTransform(transform_);
	animationTime_ = 0.0f;
}

void Hat::Update() {
	if (animationTime_ < kMaxAnimationTime) {
		animationTime_ += 1.0f / 60.0f;
		if (animationTime_ > kMaxAnimationTime) {
			animationTime_ -= kMaxAnimationTime;
		}
	}

	transform_.rotate.z = std::numbers::pi_v<float> / 180 * (-15 + 15 * cosf(std::numbers::pi_v<float> *2 * (animationTime_ / kMaxAnimationTime)));
	object_->SetTransform(transform_);
}

void Hat::Draw() {
	//体力(ハット)
	for (int32_t i = 0; i < player_->GetHP(); i++) {
		SRT transform = transform_;
		transform.translate.x += i * transform_.scale.x * 2;
		object_->SetTransform(transform);
		object_->Draw2D();
	}

}