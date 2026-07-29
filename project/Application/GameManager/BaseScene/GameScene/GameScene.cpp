#include "GameScene.h"
#include "GameEngine.h"
#include <numbers>
#include <SceneManager/SceneManager.h>
#include "../TitleScene/TitleScene.h"
#include <cmath>
#include <Collision.h>
#include <RadialBlurData.h>
#include <TextureManager/TextureManager.h>

GameScene::~GameScene() {

	ParticleManager::GetInstance()->Reset();

}

void GameScene::Initialize(shared_ptr<Input> input) {

	input_ = input;

	//パーティクル
	/*ParticleManager::GetInstance()->CreateParticleGroup("particle_1", "resources/Particle/sand.png");
	particle_ = std::make_unique<ParticleEmitter>("particle_1");
	editor_ = std::make_unique<ParticleEditor>();
	editor_->Initialize(particle_.get());

	Emitter emitter_;
	emitter_.count = 2;
	emitter_.lifeTime = 3.0f;
	emitter_.frequency = 0.1f;
	emitter_.frequencyTime = 0.0f;
	emitter_.spawnRange.min = { -5.0f,-0.5f,-50.0f };
	emitter_.spawnRange.max = { 5.0f,0.5f,50.0f };
	emitter_.angleBase = { -1.0f,0.0f,0.0f };
	emitter_.angleRange = { 0.0f,0.0f,0.1f };	//方向範囲
	emitter_.speedBase = 0.7f;	//基礎速度
	emitter_.speedRange = 0.4f;	//速度範囲
	emitter_.beforeColor = { 1.0f,1.0f,1.0f,1.0f };
	emitter_.afterColor = { 1.0f,1.0f,1.0f,0.0f };
	emitter_.rotateVelocity = 0.0f;
	emitter_.rotateRate = std::numbers::pi_v<float>;
	editor_->SetEmitter(emitter_);
	AccelerationField field_;
	field_.area.min = { -50.0f,-10.0f,-75.0f };
	field_.area.max = { 10.0f,10.0f,75.0f };
	field_.acceleration = {};
	field_.acceleration.translate = { -0.005f,0.0f,0.00f };
	editor_->SetField(field_);
	SRT emitterTransform;
	emitterTransform.scale = { 10.0f, 10.0f, 10.0f };
	emitterTransform.rotate = { 0, 0, 0 };
	emitterTransform.translate = { 50.0f, 0.0f, 0.0f };
	editor_->SetTransform(emitterTransform);

	//パーティクル
	ParticleManager::GetInstance()->CreateParticleGroup("particle_2", "resources/Particle/particle.png");
	particle_2 = std::make_unique<ParticleEmitter>("particle_2");
	editor_2 = std::make_unique<ParticleEditor>();
	editor_2->Initialize(particle_2.get());

	Emitter emitter_2;
	emitter_2.count = 16;
	emitter_2.lifeTime = 0.5f;
	emitter_2.frequency = 0.0f;
	emitter_2.frequencyTime = 0.0f;
	emitter_2.spawnRange.min = { 0.0f,0.0f,0.0f };
	emitter_2.spawnRange.max = { 0.0f,0.0f,0.0f };
	emitter_2.angleBase = { 0.0f,0.0f,1.0f };
	emitter_2.angleRange = { 0.1f,0.1f,0.1f };	//方向範囲
	emitter_2.speedBase = 0.1f;	//基礎速度
	emitter_2.speedRange = 0.05f;	//速度範囲
	emitter_2.beforeColor = { 240.0f / 256.0f,240.0f / 256.0f,240.0f / 256.0f,1.0f };
	emitter_2.afterColor =  {  40.0f / 256.0f, 40.0f / 256.0f, 40.0f / 256.0f,0.0f };
	editor_2->SetEmitter(emitter_2);
	AccelerationField field_2;
	field_2.area.min = { 0.0f,0.0f,0.0f };
	field_2.area.max = { 1.0f,1.0f,1.0f };
	field_2.acceleration = {};
	editor_2->SetField(field_2);
	SRT emitterTransform_2;
	emitterTransform_2.scale = { 1.0f,1.0f,1.0f };
	emitterTransform_2.rotate = { 0.0f,0.0f,0.0f };
	emitterTransform_2.translate = { 0.0f,0.0f,0.0f };
	editor_2->SetTransform(emitterTransform_2);

	//パーティクル
	ParticleManager::GetInstance()->CreateParticleGroup("particle_3", "resources/Particle/particle.png");
	particle_3 = std::make_unique<ParticleEmitter>("particle_3");
	editor_3 = std::make_unique<ParticleEditor>();
	editor_3->Initialize(particle_3.get());

	Emitter emitter_3;
	emitter_3.count = 16;
	emitter_3.lifeTime = 0.5f;
	emitter_3.frequency = 0.0f;
	emitter_3.frequencyTime = 0.0f;
	emitter_3.spawnRange.min = { 0.0f,0.0f,0.0f };
	emitter_3.spawnRange.max = { 0.0f,0.0f,0.0f };
	emitter_3.beforeColor = { 1.0f,1.0f,0.6f,1.0f };
	emitter_3.afterColor = { 0.6f,0.2f,0.2f,0.0f };
	emitter_3.rotateVelocity = 0.0f;
	emitter_3.rotateRate = std::numbers::pi_v<float>;
	editor_3->SetEmitter(emitter_3);
	AccelerationField field_3;
	field_3.area.min = { 0.0f,0.0f,0.0f };
	field_3.area.max = { 1.0f,1.0f,1.0f };
	field_3.acceleration = {};
	editor_3->SetField(field_3);
	SRT emitterTransform_3;
	emitterTransform_3.scale = { 2.0f,0.1f,0.1f };
	emitterTransform_3.rotate = { 0.0f,0.0f,0.0f };
	emitterTransform_3.translate = { 0.0f,0.0f,0.0f };
	editor_3->SetTransform(emitterTransform_3);

	//パーティクル
	ParticleManager::GetInstance()->CreateParticleGroup("particle_4", "resources/Particle/sand.png");
	particle_4 = std::make_unique<ParticleEmitter>("particle_4");
	editor_4 = std::make_unique<ParticleEditor>();
	editor_4->Initialize(particle_4.get());

	Emitter emitter_4;
	emitter_4.count = 1;
	emitter_4.lifeTime = 0.2f;
	emitter_4.frequency = 0.0f;
	emitter_4.frequencyTime = 0.0f;
	emitter_4.spawnRange.min = { 0.0f,0.0f,0.0f };
	emitter_4.spawnRange.max = { 0.0f,0.0f,0.0f };
	emitter_4.angleBase = { 0.0f,0.0f,1.0f };
	emitter_4.angleRange = { 0.2f,0.2f,0.2f };	//方向範囲
	emitter_4.speedBase = 0.2f;	//基礎速度
	emitter_4.speedRange = 0.1f;	//速度範囲
	emitter_4.beforeColor = { 1.0f,1.0f,1.0f,1.0f };
	emitter_4.afterColor = { 1.0f,1.0f,1.0f,0.0f };
	editor_4->SetEmitter(emitter_4);
	AccelerationField field_4;
	field_4.area.min = { 0.0f,0.0f,0.0f };
	field_4.area.max = { 1.0f,1.0f,1.0f };
	field_4.acceleration = {};
	editor_4->SetField(field_4);
	SRT emitterTransform_4;
	emitterTransform_4.scale = { 3.0f,3.0f,3.0f };
	emitterTransform_4.rotate = { 0.0f,0.0f,0.0f };
	emitterTransform_4.translate = { 0.0f,0.0f,0.0f };
	editor_4->SetTransform(emitterTransform_4);*/
  
  //ring_ = std::make_unique<PrimitiveRing>();
	//ring_->Initialize(TextureManager::GetInstance()->GetSrvIndex("resources/Particle/gradationLine.png"), gameCamera_->GetCamera(), GameEngine::GetDirectXCommon());
	//ringTransform_ = {};
	//ringTransform_.scale = { 4,4,4 };
	//ringMaterial_.color = { 1.0f,1.0f,1.0f,0.0f };
	//ringMaterial_.uvTransform = MakeTranslateMatrix({ 1.0f,1.0f,1.0f });


	stage_ = std::make_unique<Stage>();
	stage_->Initialize(StageManager::GetInstance()->ReadStage("resources/Data/Stage/Stage.json"), input_);

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
		if (stage_->IsClear()) {
			SceneManager::GetInstance()->ChangeScene("Clear");
		} else {
			SceneManager::GetInstance()->ChangeScene("GameOver");
		}
	}

	float a = 0.0f;
	if (fade_ == Fade::FadeIn) {
		a = 1.0f - fadeTime_ / kMaxFadeTime;
	} else if (fade_ == Fade::FadeOut) {
		a = fadeTime_ / kMaxFadeTime;
	}
	dissolveData_.threshold = a;
	fadeSprite_->Update();


	/*editor_->Update();
	editor_2->Update();
	editor_3->Update();
	editor_4->Update();

	Matrix4x4 rotateMatrix = MakeRotateYMatrix(player_->GetTransform()->rotate.y);
	pointLightElement_.position = player_->GetTransform()->translate + rotateMatrix * Vector3(0.0f, 0.0f, 1.0f);
	pointLight_->SetPointLightElement(pointLightElement_);

	SRT transform = editor_2->GetTransform();
	Emitter emitter = editor_2->GetEmitter();
	transform.translate = pointLightElement_.position;
	emitter.angleBase = Normalize(rotateMatrix * Vector3(0.0f, 0.0f, 0.8f) + Vector3(0.0f, 1.0f, 0.0f));
	editor_2->SetTransform(transform);
	editor_2->SetEmitter(emitter);

	transform = editor_3->GetTransform();
	transform.translate = player_->GetTransform()->translate;
	transform.scale = { 1.0f,5.0f,5.0f };
	editor_3->SetTransform(transform);

	transform = editor_4->GetTransform();
	transform.translate = player_->GetTransform()->translate;
	transform.translate.y = 0.0f;
	editor_4->SetTransform(transform);

	ringTransform_.translate = boss_->GetTransform()->translate + Vector3{ 0,0,1 } * MakeRotateYMatrix(boss_->GetTransform()->rotate.y);
	if (ringMaterial_.color.w > 0.0f) {
		ringMaterial_.color.w -= 2.0f / 60.0f;
		if (ringMaterial_.color.w < 0.0f) {
			ringMaterial_.color.w = 0.0f;
		}
	}*/

	//セピア調にする範囲
	GameEngine::RenderPreDraw("render");

	stage_->Draw();

	//フェード
	if (fade_ != Fade::None) {
		
	}

	GameEngine::RenderPostDraw();

	GameEngine::RenderPreDraw("Outline");

	GameEngine::DrawOutline("render", stage_->GetGameCamera()->GetCamera());

	GameEngine::RenderPostDraw();

	GameEngine::RenderPreDraw("ColorChange");

	GameEngine::DrawScreen("Outline", ColorChange::COLORMODE_SEPIATONE, stage_->GetGameCamera()->GetSepiaTone());

	GameEngine::RenderPostDraw();

	GameEngine::RenderPreDraw("BoxFilter");

	GameEngine::DrawScreen("ColorChange", boxFilterData_);

	GameEngine::RenderPostDraw();

	GameEngine::RenderPreDraw("Vignette");

	VignetteData vignettedata;
	vignettedata.vignetteIntensity = 16.0f;
	vignettedata.vignetteCurve = 0.05f;
	vignettedata.isUseRandom = 1;
	vignettedata.seed = GameEngine::randomFloat(1.0f,100.0f);

	GameEngine::DrawScreen("BoxFilter", vignettedata);

	GameEngine::RenderPostDraw();

	GameEngine::RenderPreDraw("RadialBlur");

	RadialBlurData radialBlurdata;
	radialBlurdata.center = { 0.5,0.5f };
	radialBlurdata.blurWidth = 0.01f * (1.0f - stage_->GetPlayer()->GetDodgeActiveTimeRate());

	GameEngine::DrawScreen("Vignette", radialBlurdata);

	GameEngine::RenderPostDraw();
}

void GameScene::Draw() {

	GameEngine::DrawScreen("RadialBlur", dissolveData_, TextureManager::GetInstance()->GetSrvIndex("resources/DebugResources/noise0.png"));

	stage_->DrawSprite();
}