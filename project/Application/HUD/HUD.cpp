#include "HUD.h"
#include <ModelHolder/ModelHolder.h>
#include <Character/Player/Player.h>
#include <numbers>

void HUD::Initialize(std::shared_ptr<DirectionalLight> directionalLight, Player* player) {
	player_ = player;

	cylinder_ = std::make_unique<Object>();
	cylinder_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Cylinder));
	cylinder_->SetDirectionalLight(directionalLight);
	cylinderTransform_.scale = { 0.0005f, 0.0005f, 0.0005f };
	cylinderTransform_.rotate.x = std::numbers::pi_v<float> / 180 * 9;
	cylinderTransform_.rotate.y = std::numbers::pi_v<float> / 180 * -15;
	cylinderTransform_.translate = { -0.0032f,-0.0015f,0.01f };
	cylinder_->SetTransform(cylinderTransform_);

	hat_ = std::make_unique<Object>();
	hat_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Hat));
	hat_->SetDirectionalLight(directionalLight);
	hatTransform_.scale = { 0.0003f,0.0003f,0.0003f };
	hatTransform_.rotate.x = std::numbers::pi_v<float> / 180 * -25;
	hatTransform_.translate = { -0.0038f,0.0018f,0.01f };
	hat_->SetTransform(hatTransform_);
	animationTime_ = 0.0f;

	sprite_[0] = make_unique<Sprite>();
	sprite_[0]->Initialize("resources/Sprite/LT.png");
	sprite_[0]->SetPosition(Vector2{ 990,720 - 148 });
	sprite_[0]->SetSize(Vector2{ 128,128 });
	sprite_[1] = make_unique<Sprite>();
	sprite_[1]->Initialize("resources/Sprite/RT.png");
	sprite_[1]->SetPosition(Vector2{ 1060,720 - 148 });
	sprite_[1]->SetSize(Vector2{ 128,128 });
	sprite_[2] = make_unique<Sprite>();
	sprite_[2]->Initialize("resources/Sprite/B.png");
	sprite_[2]->SetPosition(Vector2{ 1130,720 - 148 });
	sprite_[2]->SetSize(Vector2{ 128,128 });
	sprite_[3] = make_unique<Sprite>();
	sprite_[3]->Initialize("resources/Sprite/Reload_UI.png");
	sprite_[3]->SetPosition(Vector2{ 1200,720 - 148 });
	sprite_[3]->SetSize(Vector2{ 128,128 });
}

void HUD::Update() {
	if (animationTime_ < kMaxAnimationTime) {
		animationTime_ += 1.0f / 60.0f;
		if (animationTime_ > kMaxAnimationTime) {
			animationTime_ -= kMaxAnimationTime;
		}
	}

	hatTransform_.rotate.z = std::numbers::pi_v<float> / 180 * (-15 + 15 * cosf(std::numbers::pi_v<float> *2 * (animationTime_ / kMaxAnimationTime)));
	hat_->SetTransform(hatTransform_);
}

void HUD::Draw() {
	//シリンダー
	int32_t remainingRounds = player_->GetRemainingRounds();
	std::vector<Parts> parts = cylinder_->GetParts();
	for (int32_t i = 1; i <= 6; i++) {
		if (remainingRounds >= i) {
			parts[i].material->color = { 1.0f,1.0f,1.0f,1.0f };
		} else {
			parts[i].material->color = { 1.0f,1.0f,1.0f,0.0f };
		}
		cylinder_->SetParts(parts[i], i);
	}
	cylinder_->Draw2D();

	//体力(ハット)
	for (int32_t i = 0; i < player_->GetHP(); i++) {
		SRT transform = hatTransform_;
		transform.translate.x += i * hatTransform_.scale.x * 2;
		hat_->SetTransform(transform);
		hat_->Draw2D();
	}

	//操作説明
	for (auto& sprite : sprite_) {
		sprite->Draw2D();
	}
}