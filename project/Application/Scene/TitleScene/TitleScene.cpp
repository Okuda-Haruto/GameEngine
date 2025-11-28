#include "TitleScene.h"
#include "GameEngine.h"
#include <numbers>
#include "../GameScene/GameScene.h"

TitleScene::~TitleScene() {

}

void TitleScene::Initialize(ModelHolder* modelHolder, SpriteManager* spriteManager) {
	modelHolder_ = modelHolder;

	titleSprite_ = std::make_unique<Sprite>();
	titleSprite_->Initialize("resources/Title.png", spriteManager);

	fadeSprite_ = std::make_unique<Sprite>();
	fadeSprite_->Initialize("resources/DebugResources/white2x2.png", spriteManager);
	fadeSprite_->SetSize({ 1280,720 });
	fadeSprite_->SetColor({ 0.0f,0.0f,0.0f,1.0f });

	fade_ = Fade::FadeIn;
	fadeTime_ = 0.0f;
	isfinished_ = false;
}

void TitleScene::Update() {
	Keybord key = GameEngine::GetKeybord();
	Pad pad = GameEngine::GetPad();

	if (fadeTime_ < kMaxFadeTime) {
		fadeTime_ += 1.0f / 60.0f;
	}

	if (fade_ == Fade::FadeIn && fadeTime_ >= kMaxFadeTime) {
		fade_ = Fade::None;
	}
	if (fade_ == Fade::FadeOut&& fadeTime_ >= kMaxFadeTime) {
		isfinished_ = true;
	}

	if ((key.trigger[DIK_SPACE] || key.trigger[DIK_C] || pad.Button[PAD_BUTTON_Y].trigger) && fade_ == Fade::None) {
		fade_ = Fade::FadeOut;
		fadeTime_ = 0.0f;
		nextScene_ = new GameScene;
	}

	float a = 0.0f;
	if (fade_ == Fade::FadeIn) {
		a = 1.0f - fadeTime_ / kMaxFadeTime;
	} else if (fade_ == Fade::FadeOut) {
		a = fadeTime_ / kMaxFadeTime;
	}
	fadeSprite_->SetColor({ 0.0f,0.0f,0.0f,a });
	fadeSprite_->Update();
	titleSprite_->Update();
}

void TitleScene::Draw() {
	titleSprite_->Draw2D();
	if (fade_ != Fade::None) {
		fadeSprite_->Draw2D();
	}
}