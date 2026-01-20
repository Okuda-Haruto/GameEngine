#include "TitleScene.h"
#include "GameEngine.h"
#include <numbers>
#include <SceneManager/SceneManager.h>
#include "../GameScene/GameScene.h"

TitleScene::~TitleScene() {

}

void TitleScene::Initialize(shared_ptr<Input> input) {
	input_ = input;

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
	skydome_->Initialize();
	skydome_->SetCamera(camera_);
	ground_ = std::make_unique<Ground>();
	ground_->Initialize();
	ground_->SetDirectionalLight(directionalLight_);
	ground_->SetCamera(camera_);
	fence_ = std::make_unique<Fence>();
	fence_->Initialize(camera_, directionalLight_, nullptr);
	player_ = std::make_unique<Object>();
	player_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Player));
	player_->SetDirectionalLight(directionalLight_);
	player_->SetCamera(camera_);
	SRT PlayerTransform{};
	PlayerTransform.scale = { 1,1,1 };
	PlayerTransform.rotate = { 0,std::numbers::pi_v<float>,0 };
	PlayerTransform.translate = { 0,1,1.5f };
	player_->SetTransform(PlayerTransform);


	titleSprite_ = std::make_unique<Sprite>();
	titleSprite_->Initialize("resources/Title/GunKid_Title.png");
	titleSprite_->SetAnchorPoint(Vector2{ 0.5f,0.5f });
	titleSprite_->SetPosition(Vector2{ 640.0f,200.0f });
	press_B_Start_Sprite_ = std::make_unique<Sprite>();
	press_B_Start_Sprite_->Initialize("resources/Title/Press_B_Start.png");
	press_B_Start_Sprite_->SetAnchorPoint(Vector2{ 0.5f,0.5f });
	press_B_Start_Sprite_->SetPosition(Vector2{ 640.0f,500.0f });
	credit_Sprite_ = std::make_unique<Sprite>();
	credit_Sprite_->Initialize("resources/Title/Credit.png");
	credit_Sprite_->SetAnchorPoint(Vector2{ 1.0f,1.0f });
	credit_Sprite_->SetPosition(Vector2{ 1280 - 10,720.0f - 10 });

	fadeSprite_ = std::make_unique<Sprite>();
	fadeSprite_->Initialize("resources/DebugResources/white2x2.png");
	fadeSprite_->SetSize({ 1280,720 });
	fadeSprite_->SetColor({ 0.0f,0.0f,0.0f,1.0f });

	fade_ = Fade::FadeIn;
	fadeTime_ = 0.0f;
}

void TitleScene::Update() {
	Keybord key = input_->GetKeyBord();
	Pad pad = input_->GetPad(0);

	if (!AudioHolder::GetInstance()->GetAudio(AudioIndex::Title_BGM).lock()->IsSoundPlayingWave()) {
		AudioHolder::GetInstance()->GetAudio(AudioIndex::Title_BGM).lock()->SoundPlayWave();
	}
	if (AudioHolder::GetInstance()->GetAudio(AudioIndex::Battle_BGM).lock()->IsSoundPlayingWave()) {
		AudioHolder::GetInstance()->GetAudio(AudioIndex::Battle_BGM).lock()->SoundEndWave();
	}

	animationTime += 1.0f / 60.0f;
	if (animationTime > 60.0f)animationTime -= 60.0f;

	if (fadeTime_ < kMaxFadeTime) {
		fadeTime_ += 1.0f / 60.0f;
	}

	if (fade_ == Fade::FadeIn && fadeTime_ >= kMaxFadeTime) {
		fade_ = Fade::None;
	}
	if (fade_ == Fade::FadeOut&& fadeTime_ >= kMaxFadeTime) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}

	if ((key.trigger[DIK_SPACE] || key.trigger[DIK_C] || pad.Button[PAD_BUTTON_B].trigger) && fade_ == Fade::None) {
		fade_ = Fade::FadeOut;
		fadeTime_ = 0.0f;
	}

	float a = 0.0f;
	if (fade_ == Fade::FadeIn) {
		a = 1.0f - fadeTime_ / kMaxFadeTime;
	} else if (fade_ == Fade::FadeOut) {
		a = fadeTime_ / kMaxFadeTime;
	}

	press_B_Start_Sprite_->SetPosition(Vector2{ 640.0f,500.0f + 15.0f * cosf(std::numbers::pi_v<float> / 2 * animationTime)});

	fadeSprite_->SetColor({ 0.0f,0.0f,0.0f,a });
	fadeSprite_->Update();
	titleSprite_->Update();
	credit_Sprite_->Update();
	press_B_Start_Sprite_->Update();
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
	player_->Draw3D();

	titleSprite_->Draw2D();
	press_B_Start_Sprite_->Draw2D();
	credit_Sprite_->Draw2D();
	if (fade_ != Fade::None) {
		fadeSprite_->Draw2D();
	}
}