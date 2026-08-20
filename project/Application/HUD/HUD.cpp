#include "HUD.h"
#include <ModelHolder/ModelHolder.h>
#include <Entity/Player/Player.h>
#include <numbers>

void HUD::Initialize(bool isTutorial, std::shared_ptr<Input> input, std::shared_ptr<DirectionalLight> directionalLight, Player* player) {
	player_ = player;
	input_ = input;

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
	sprite_[0]->SetPosition(Vector2{ 1280 - 148 * 4,720 - 148 });
	sprite_[0]->SetSize(Vector2{ 128,128 });
	sprite_[1] = make_unique<Sprite>();
	sprite_[1]->Initialize("resources/Sprite/RT.png");
	sprite_[1]->SetPosition(Vector2{ 1280 - 148 * 3,720 - 148 });
	sprite_[1]->SetSize(Vector2{ 128,128 });
	sprite_[2] = make_unique<Sprite>();
	sprite_[2]->Initialize("resources/Sprite/B.png");
	sprite_[2]->SetPosition(Vector2{ 1280 - 148 * 2,720 - 148 });
	sprite_[2]->SetSize(Vector2{ 128,128 });
	sprite_[3] = make_unique<Sprite>();
	sprite_[3]->Initialize("resources/Sprite/Reload_UI.png");
	sprite_[3]->SetPosition(Vector2{ 1280 - 148,720 - 148 });
	sprite_[3]->SetSize(Vector2{ 128,128 });

	isTutorial_ = isTutorial;
	tutorialNum_ = 0;
	tutorialSprite_ = make_unique<Sprite>();
	tutorialSprite_->Initialize("resources/Sprite/tutorial.png");
	tutorialSprite_->SetAnchorPoint({ 0.5f,0.5f });
	tutorialSprite_->SetPosition(Vector2{ 1280 / 2,128 + 20 });
	tutorialSprite_->SetSize(Vector2{ 417 * 2,64 * 2 });
	tutorialSprite_->SetTextureSize({ 417,64 });
}

void HUD::Update() {
	Pad pad = input_->GetPad();

	if (animationTime_ < kMaxAnimationTime) {
		animationTime_ += 1.0f / 60.0f;
		if (animationTime_ > kMaxAnimationTime) {
			animationTime_ -= kMaxAnimationTime;
		}
	}

	for (int i = 0; i <= 3; i++) {
		sprite_[i]->Update();
	}
	tutorialSprite_->Update();

	hatTransform_.rotate.z = std::numbers::pi_v<float> / 180 * (-15 + 15 * cosf(std::numbers::pi_v<float> *2 * (animationTime_ / kMaxAnimationTime)));
	hat_->SetTransform(hatTransform_);

	if (isTutorial_) {
		switch (tutorialNum_)
		{
		case 0:
			if (pad.Button[PAD_BUTTON_RT].trigger) {
				tutorialNum_++;
			}
			break;
		case 1:
			if (pad.Button[PAD_BUTTON_B].trigger) {
				tutorialNum_++;
			}
			break;
		case 2:
			if (pad.Button[PAD_BUTTON_LT].trigger) {
				tutorialNum_++;
			}
			break;
		default:
			break;
		}
		tutorialSprite_->SetTextureLeftTop({ 0.0f,64.0f * tutorialNum_ });
	}
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

}

void HUD::DrawSprite() {
	//操作説明
	for (auto& sprite : sprite_) {
		sprite->Draw2D();
	}
	if (isTutorial_) {
		tutorialSprite_->Draw2D();
	}
}