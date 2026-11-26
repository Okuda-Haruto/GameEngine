#include "GameScene.h"
#include "GameEngine.h"
#include <numbers>
#include "SceneManager/SceneManager.h"
#include "../TitleScene/TitleScene.h"
#include <cmath>
#include "Collision.h"
#include "RadialBlurData.h"
#include "TextureManager/TextureManager.h"

GameScene::~GameScene() {

	ParticleManager::GetInstance()->Reset();

}

void GameScene::Initialize(std::shared_ptr<Input> input) {

	input_ = input;

	stage_ = std::make_unique<Stage>();
	stage_->Initialize(true, StageManager::GetInstance()->ReadStage("resources/Data/Stage/Stage.json"), input_);

	fadeSprite_ = std::make_unique<Sprite>();
	fadeSprite_->Initialize("resources/DebugResources/white2x2.png");
	fadeSprite_->SetSize({ 1280,720 });
	fadeSprite_->SetColor({ 0.0f,0.0f,0.0f,1.0f });

	dissolveData_.threshold = 0.0f;
	dissolveData_.edgeWidth = 0.03f;

	fade_ = Fade::FadeIn;
	fadeTime_ = 0.0f;

	boxFilterData_.scale = 5;
}

void GameScene::Update() {

	if (!AudioHolder::GetInstance()->GetAudio(AudioIndex::Battle_BGM).lock()->IsSoundPlayingWave()) {
		AudioHolder::GetInstance()->GetAudio(AudioIndex::Battle_BGM).lock()->SoundPlayWave();
	}
	if (AudioHolder::GetInstance()->GetAudio(AudioIndex::Title_BGM).lock()->IsSoundPlayingWave()) {
		AudioHolder::GetInstance()->GetAudio(AudioIndex::Title_BGM).lock()->SoundEndWave();
	}

	stage_->Update();
	if (stage_->IsEnd() && fade_ == Fade::None) {
		fade_ = Fade::FadeOut;
		fadeTime_ = 0.0f;
	}

	if (fadeTime_ < kMaxFadeTime) {
		fadeTime_ += 1.0f / 60.0f;
	}
	boxFilterData_.scale = int(30 * (1.0f - (fadeTime_ / kMaxFadeTime))) + 1;

	if (fade_ == Fade::FadeIn && fadeTime_ >= kMaxFadeTime) {
		fade_ = Fade::None;
	}
	if (fade_ == Fade::FadeOut && fadeTime_ >= kMaxFadeTime) {
		SceneManager::GetInstance()->ChangeScene(stage_->GetNextScene());
	}

	float a = 0.0f;
	if (fade_ == Fade::FadeIn) {
		a = 1.0f - fadeTime_ / kMaxFadeTime;
	} else if (fade_ == Fade::FadeOut) {
		a = fadeTime_ / kMaxFadeTime;
	}
	dissolveData_.threshold = a;
	fadeSprite_->Update();

	//描画
	GameEngine::RenderPreDraw("render");

	stage_->Draw();

	//フェード
	if (fade_ != Fade::None) {
		
	}

	GameEngine::RenderPostDraw();

	//アウトライン
	GameEngine::RenderPreDraw("Outline");

	GameEngine::DrawOutline("render", stage_->GetGameCamera().lock()->GetCamera().lock());

	GameEngine::RenderPostDraw();

	//セピアトーン
	GameEngine::RenderPreDraw("ColorChange");

	GameEngine::DrawScreen("Outline", ColorChange::COLORMODE_SEPIATONE, stage_->GetGameCamera().lock()->GetSepiaTone());

	stage_->DrawSprite();

	GameEngine::RenderPostDraw();

	//ぼかし
	GameEngine::RenderPreDraw("BoxFilter");

	GameEngine::DrawScreen("ColorChange", boxFilterData_);

	GameEngine::RenderPostDraw();

	//ビネット
	GameEngine::RenderPreDraw("Vignette");

	VignetteData vignetteData;
	vignetteData.vignetteIntensity = 16.0f;
	vignetteData.vignetteCurve = 0.05f;
	vignetteData.isUseRandom = 1;
	vignetteData.seed = GameEngine::randomFloat(1.0f,100.0f);

	GameEngine::DrawScreen("BoxFilter", vignetteData);

	GameEngine::RenderPostDraw();

	//ブラー
	GameEngine::RenderPreDraw("RadialBlur");

	GameEngine::DrawScreen("Vignette", stage_->GetGameCamera().lock()->GetRadialBlurData());

	GameEngine::RenderPostDraw();
}

void GameScene::Draw() {

	GameEngine::DrawScreen("RadialBlur", dissolveData_, TextureManager::GetInstance()->GetSrvIndex("resources/DebugResources/noise0.png"));
}