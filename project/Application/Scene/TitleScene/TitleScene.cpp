#include "TitleScene.h"
#include "GameEngine.h"
#include <numbers>
#include "../GameScene/GameScene.h"

TitleScene::~TitleScene() {

}

void TitleScene::Initialize(ModelHolder* modelHolder, SpriteManager* spriteManager) {
	modelHolder_ = modelHolder;

	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Initialize(GameEngine::GetDirectXCommon());
	directionalLightElement_.color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	directionalLightElement_.direction = Normalize(Vector3{ 0.0f,-1.0f,1.0f });
	directionalLightElement_.intensity = 1.0f;
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);

	//メインカメラ
	camera_ = std::make_unique<Camera>();
	camera_->Initialize(GameEngine::GetDirectXCommon());

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(modelHolder_);
	skydome_->SetCamera(camera_.get());
	ground_ = std::make_unique<Ground>();
	ground_->Initialize(modelHolder_);
	ground_->SetDirectionalLight(directionalLight_.get());
	ground_->SetCamera(camera_.get());
	fence_ = std::make_unique<Fence>();
	fence_->Initialize(modelHolder_, camera_.get(), directionalLight_.get(), nullptr);;


	titleSprite_ = std::make_unique<Sprite>();
	titleSprite_->Initialize("resources/Title/GunKid_Title.png", spriteManager);
	titleSprite_->SetAnchorPoint(Vector2{ 0.5f,0.5f });
	titleSprite_->SetPosition(Vector2{ 640.0f,200.0f });
	pless_B_Start_Sprite_ = std::make_unique<Sprite>();
	pless_B_Start_Sprite_->Initialize("resources/Title/Pless_B_Start.png", spriteManager);
	pless_B_Start_Sprite_->SetAnchorPoint(Vector2{ 0.5f,0.5f });
	pless_B_Start_Sprite_->SetPosition(Vector2{ 640.0f,500.0f });

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

	if ((key.trigger[DIK_SPACE] || key.trigger[DIK_C] || pad.Button[PAD_BUTTON_B].trigger) && fade_ == Fade::None) {
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
	pless_B_Start_Sprite_->Update();
	SRT transform = {
		{0,0,0},
		{0,0,0},
		{0,2,-20}
	};
	camera_->Update(transform);
}

void TitleScene::Draw() {
	skydome_->Draw();
	ground_->Draw();
	fence_->Draw();

	titleSprite_->Draw2D();
	pless_B_Start_Sprite_->Draw2D();
	if (fade_ != Fade::None) {
		fadeSprite_->Draw2D();
	}
}