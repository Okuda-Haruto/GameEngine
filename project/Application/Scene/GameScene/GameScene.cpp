#include "GameScene.h"
#include "GameEngine.h"
#include <numbers>
#include <SceneManager/SceneManager.h>
#include "../TitleScene/TitleScene.h"
#include <cmath>
#include <Collision.h>

GameScene::~GameScene() {
	playerBullet_.clear();
	bossBullet_.clear();
	ParticleManager::GetInstance()->Reset();

#ifdef USE_IMGUI
	SaveBackGround("resources/CSV/BackGround.csv");
#endif
}

void GameScene::Initialize(shared_ptr<Input> input) {

	input_ = input;

	//パーティクル
	ParticleManager::GetInstance()->CreateParticleGroup("particle_1", "resources/Particle/Sand.png");
	particle_ = std::make_unique<ParticleEmitter>("particle_1");
	editor_ = std::make_unique<ParticleEditor>();
	editor_->Initialize(particle_.get());

	Emitter emitter_;
	emitter_.count = 2;
	emitter_.lifeTime = 3.0f;
	emitter_.frequency = 0.1f;
	emitter_.frequencyTime = 0.0f;
	emitter_.transform.scale = { 10.0f,10.0f,10.0f };
	emitter_.transform.translate = { 50.0f,0.0f,0.0f };
	emitter_.spawnRange.min = { -5.0f,-0.5f,-50.0f };
	emitter_.spawnRange.max = { 5.0f,0.5f,50.0f };
	emitter_.angleBase = { -1.0f,0.0f,0.0f };
	emitter_.angleRange = { 0.0f,0.0f,0.1f };	//方向範囲
	emitter_.speedBase = 0.7f;	//基礎速度
	emitter_.speedRange = 0.4f;	//速度範囲
	emitter_.beforeColor = { 1.0f,1.0f,1.0f,1.0f };
	emitter_.afterColor = { 1.0f,1.0f,1.0f,0.0f };
	editor_->SetEmitter(emitter_);
	AccelerationField field_;
	field_.area.min = { -50.0f,-10.0f,-75.0f };
	field_.area.max = { 10.0f,10.0f,75.0f };
	field_.acceleration = {};
	field_.acceleration.translate = { 0.005f,0.0f,0.00f };
	editor_->SetField(field_);

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
	emitter_2.transform.scale = { 1.0f,1.0f,1.0f };
	emitter_2.transform.translate = { 0.0f,0.0f,0.0f };
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

	//パーティクル
	ParticleManager::GetInstance()->CreateParticleGroup("particle_3", "resources/Particle/particle.png");
	particle_3 = std::make_unique<ParticleEmitter>("particle_3");
	editor_3 = std::make_unique<ParticleEditor>();
	editor_3->Initialize(particle_3.get());

	Emitter emitter_3;
	emitter_3.count = 32;
	emitter_3.lifeTime = 0.5f;
	emitter_3.frequency = 0.0f;
	emitter_3.frequencyTime = 0.0f;
	emitter_3.transform.scale = { 2.0f,2.0f,2.0f };
	emitter_3.transform.translate = { 0.0f,0.0f,0.0f };
	emitter_3.spawnRange.min = { 0.0f,0.0f,0.0f };
	emitter_3.spawnRange.max = { 0.0f,0.0f,0.0f };
	emitter_3.angleBase = { 0.0f,0.0f,1.0f };
	emitter_3.angleRange = { 1.0f,1.0f,1.0f };	//方向範囲
	emitter_3.speedBase = 0.3f;	//基礎速度
	emitter_3.speedRange = 0.1f;	//速度範囲
	emitter_3.beforeColor = { 1.0f,0.8f,0.6f,1.0f };
	emitter_3.afterColor = { 1.0f,0.2f,0.2f,0.0f };
	editor_3->SetEmitter(emitter_3);
	AccelerationField field_3;
	field_3.area.min = { 0.0f,0.0f,0.0f };
	field_3.area.max = { 1.0f,1.0f,1.0f };
	field_3.acceleration = {};
	editor_3->SetField(field_3);

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
	emitter_4.transform.scale = { 3.0f,3.0f,3.0f };
	emitter_4.transform.translate = { 0.0f,0.0f,0.0f };
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

	//メインカメラ
	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Initialize(input_);
	gameCamera_->SetOffset(Vector3{ 0.0f,6.0f,-60.0f });
	gameCamera_->SetRotate(Vector3{ std::numbers::pi_v<float> / 180 * 10,0.0f,0.0f });
	SRT event{};
	event.translate = { 0.0f,2.0f,-30.0f };
	gameCamera_->SetEventTransform(event);
	gameCamera_->SetIsEvent(true);
	PlayerBullet::SetCamera(gameCamera_->GetCamera());
	BossBullet::SetCamera(gameCamera_->GetCamera());

	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Initialize(GameEngine::GetDirectXCommon());
	directionalLightElement_.color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	directionalLightElement_.direction = Normalize(Vector3{ 0.0f,-1.0f,1.0f });
	directionalLightElement_.intensity = 1.0f;
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);
	PlayerBullet::SetDirectionalLight(directionalLight_);
	BossBullet::SetDirectionalLight(directionalLight_);

	pointLight_ = std::make_unique<PointLight>();
	pointLight_->Initialize(GameEngine::GetDirectXCommon());
	pointLightElement_.color = Vector4{ 1.0f,0.8f,0.6f,1.0f };
	pointLightElement_.intensity = 0.0f;
	pointLightElement_.radius = 4.0f;
	pointLightElement_.position = {};
	pointLightElement_.decay = 1.0f;
	pointLight_->SetPointLightElement(pointLightElement_);
	PlayerBullet::SetPointLight(pointLight_);
	BossBullet::SetPointLight(pointLight_);

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize(this,gameCamera_.get(), input_, particle_4.get());
	gameCamera_->SetPlayer(player_->GetTransform());
	player_->SetCameraTransform(gameCamera_->GetTransform());
	player_->SetCamera(gameCamera_->GetCamera());
	player_->SetDirectionalLight(directionalLight_);
	player_->SetPointLight(pointLight_);

	//ボス
	boss_ = std::make_unique<Boss>();
	boss_->Initialize(this,gameCamera_.get(), particle_3.get(), player_.get(), 60.0f);
	gameCamera_->SetTarget(boss_->GetTransform());
	player_->SetBossTransform(boss_->GetTransform());
	boss_->SetCamera(gameCamera_->GetCamera());
	boss_->SetDirectionalLight(directionalLight_);
	boss_->SetPointLight(pointLight_);

	colliderObject_ = std::make_unique<ColliderObject>();
	colliderObject_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Tumbleweed), SRT{{2,2,2},{0,0,0},{20,2,0}});

	cylinder_ = std::make_unique<Object>();
	cylinder_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Cylinder));
	cylinder_->SetDirectionalLight(directionalLight_);
	cylinderTransform_.scale = { 0.0005f, 0.0005f, 0.0005f };
	cylinderTransform_.rotate.x = std::numbers::pi_v<float> / 180 * 9;
	cylinderTransform_.rotate.y = std::numbers::pi_v<float> / 180 * -15;
	cylinderTransform_.translate = { -0.0032f,-0.0015f,0.01f };
	cylinder_->SetTransform(cylinderTransform_);

	hat_ = std::make_unique<Object>();
	hat_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Hat));
	hat_->SetDirectionalLight(directionalLight_);
	hatTransform_.scale = { 0.0003f,0.0003f,0.0003f };
	hatTransform_.rotate.x = std::numbers::pi_v<float> / 180 * -25;
	hatTransform_.translate = { -0.0038f,0.0018f,0.01f };
	hat_->SetTransform(hatTransform_);
	animationTime_ = 0.0f;

	sprite_[0] = make_unique<Sprite>();
	sprite_[0]->Initialize("resources/Sprite/LT.png");
	sprite_[0]->SetPosition(Vector2{ 990,720 - 84 });
	sprite_[0]->SetSize(Vector2{ 64,64 });
	sprite_[1] = make_unique<Sprite>();
	sprite_[1]->Initialize("resources/Sprite/RT.png");
	sprite_[1]->SetPosition(Vector2{ 1060,720 - 84 });
	sprite_[1]->SetSize(Vector2{ 64,64 });
	sprite_[2] = make_unique<Sprite>();
	sprite_[2]->Initialize("resources/Sprite/B.png");
	sprite_[2]->SetPosition(Vector2{ 1130,720 - 84 });
	sprite_[2]->SetSize(Vector2{ 64,64 });
	sprite_[3] = make_unique<Sprite>();
	sprite_[3]->Initialize("resources/Sprite/Reload_UI.png");
	sprite_[3]->SetPosition(Vector2{ 1200,720 - 84 });
	sprite_[3]->SetSize(Vector2{ 64,64 });

	//背景
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(gameCamera_->GetCamera());
	ground_ = std::make_unique<Ground>();
	ground_->Initialize();
	ground_->SetCamera(gameCamera_->GetCamera());
	ground_->SetDirectionalLight(directionalLight_);
	ground_->SetPointLight(pointLight_);
	backGrouds_ = LoadBackGround("resources/CSV/BackGround.csv");
	tumbleweed_ = std::make_unique<Tumbleweed>();
	tumbleweed_->Initialize(gameCamera_->GetCamera(),directionalLight_,pointLight_,particle_4.get());

	fence_ = std::make_unique<Fence>();
	fence_->Initialize(gameCamera_->GetCamera(),directionalLight_,pointLight_);

	fadeSprite_ = std::make_unique<Sprite>();
	fadeSprite_->Initialize("resources/DebugResources/white2x2.png");
	fadeSprite_->SetSize({ 1280,720 });
	fadeSprite_->SetColor({ 0.0f,0.0f,0.0f,1.0f });

	fade_ = Fade::FadeIn;
	fadeTime_ = 0.0f;

	isClear_ = false;
}

void GameScene::Update() {
	Keybord key = input_->GetKeyBord();

	if (!AudioHolder::GetInstance()->GetAudio(AudioIndex::Battle_BGM).lock()->IsSoundPlayingWave()) {
		AudioHolder::GetInstance()->GetAudio(AudioIndex::Battle_BGM).lock()->SoundPlayWave();
	}
	if (AudioHolder::GetInstance()->GetAudio(AudioIndex::Title_BGM).lock()->IsSoundPlayingWave()) {
		AudioHolder::GetInstance()->GetAudio(AudioIndex::Title_BGM).lock()->SoundEndWave();
	}


	if (animationTime_ < kMaxAnimationTime) {
		animationTime_ += 1.0f / 60.0f;
		if (animationTime_ > kMaxAnimationTime) {
			animationTime_ -= kMaxAnimationTime;
		}
	}

	if (fadeTime_ < kMaxFadeTime) {
		fadeTime_ += 1.0f / 60.0f;
	}

	if (fade_ == Fade::FadeIn && fadeTime_ >= kMaxFadeTime) {
		fade_ = Fade::None;
	}
	if (fade_ == Fade::FadeOut && fadeTime_ >= kMaxFadeTime) {
		if (isClear_) {
			SceneManager::GetInstance()->ChangeScene("Clear");
		} else {
			SceneManager::GetInstance()->ChangeScene("GameOver");
		}
	}

	if (!boss_->IsStartAnimation()) {
		gameCamera_->SetIsEvent(false);
		if (fade_ == Fade::None) {
			player_->Update();
		}
	}
	if (!isClear_) {
		gameCamera_->SetMoveVelocity(player_->GetMove().x);
		gameCamera_->SetIsTargeted(player_->GetIsTargeted());
		gameCamera_->Update();

		if (boss_->IsDead() && fade_ == Fade::None) {
			isClear_ = true;
			fade_ = Fade::FadeOut;
			fadeTime_ = 0.0f;
		}
		if (player_->IsDead() && fade_ == Fade::None) {
			isClear_ = false;
			fade_ = Fade::FadeOut;
			fadeTime_ = 0.0f;
		}
		if (pointLightElement_.intensity > 0.0f) {
			pointLightElement_.intensity -= 0.05f;
			if (pointLightElement_.intensity < 0.0f)pointLightElement_.intensity = 0.0f;
			Matrix4x4 rotateMatrix = MakeRotateYMatrix(player_->GetTransform()->rotate.y);
			pointLightElement_.position = player_->GetTransform()->translate + rotateMatrix * Vector3(0.0f, 0.0f, 1.0f);
			pointLight_->SetPointLightElement(pointLightElement_);
		}

		for (auto& bullet : playerBullet_) {
			bullet->Update();
		}

		for (auto& bullet : bossBullet_) {
			bullet->Update();
		}

		boss_->Update();

		Collision();

		std::erase_if(playerBullet_, [](const auto& bullet) {
			return bullet->IsDead();
			});

		std::erase_if(bossBullet_, [](const auto& bullet) {
			return bullet->IsDead();
			});
	}




	float a = 0.0f;
	if (fade_ == Fade::FadeIn) {
		a = 1.0f - fadeTime_ / kMaxFadeTime;
	} else if (fade_ == Fade::FadeOut) {
		a = fadeTime_ / kMaxFadeTime;
	}
	fadeSprite_->SetColor({ 0.0f,0.0f,0.0f,a });
	fadeSprite_->Update();



	hatTransform_.rotate.z = std::numbers::pi_v<float> / 180 * (-15 + 15 * cosf(std::numbers::pi_v<float> *2 * (animationTime_ / kMaxAnimationTime)));
	hat_->SetTransform(hatTransform_);

	for (auto& sprite : sprite_) {
		sprite->Update();
	}

	tumbleweed_->Update();

	editor_->Update();
	editor_2->Update();
	editor_3->Update();
	editor_4->Update();

	Matrix4x4 rotateMatrix = MakeRotateYMatrix(player_->GetTransform()->rotate.y);
	pointLightElement_.position = player_->GetTransform()->translate + rotateMatrix * Vector3(0.0f, 0.0f, 1.0f);
	pointLight_->SetPointLightElement(pointLightElement_);

	Emitter emitter = editor_2->GetEmitter();
	emitter.transform.translate = pointLightElement_.position;
	emitter.angleBase = Normalize(rotateMatrix * Vector3(0.0f, 0.0f, 0.8f) + Vector3(0.0f, 1.0f, 0.0f));
	editor_2->SetEmitter(emitter);

	emitter = editor_3->GetEmitter();
	emitter.transform.translate = boss_->GetTransform()->translate;
	editor_3->SetEmitter(emitter);

	emitter = editor_4->GetEmitter();
	emitter.transform.translate = player_->GetTransform()->translate;
	emitter.transform.translate.y = 0.0f;
	editor_4->SetEmitter(emitter);


#ifdef USE_IMGUI
	ImGui::Begin("操作方法");
	ImGui::Text("矢印キー：移動");
	ImGui::Text("X：発射");
	ImGui::Text("C：回避");
	ImGui::Text("LShift：注目");
	ImGui::Text("攻撃と回避をしていないならリロード");
	ImGui::End();

	ImGui::Begin("背景オブジェクト");
	static int cursor = 0;
	ImGui::SliderInt("オブジェクト番号", &cursor, 0, int(backGrouds_.size() - 1));
	SRT transform = { {1,1,1},{0,0,0},{0,0,0} };
	if (ImGui::Button("追加")) {

		std::unique_ptr<BackGround> newBG = std::make_unique<BackGround>();
		newBG->Initialize(0);
		newBG->SetTransform(transform);
		newBG->SetDirectionalLight(directionalLight_);
		newBG->SetPointLight(pointLight_);
		backGrouds_.push_back(move(newBG));

		cursor = int(backGrouds_.size() - 1);
	}
	int index = int(backGrouds_[cursor]->GetIndex());
	transform = backGrouds_[cursor]->GetTransform();
	transform.rotate = transform.rotate / std::numbers::pi_v<float> * 180;

	ImGui::SliderInt("使用するモデル番号", &index, 0, 7);
	ImGui::DragFloat3("Scale", &transform.scale.x,0.1f);
	ImGui::DragFloat3("Rotate", &transform.rotate.x, 1.0f);
	ImGui::DragFloat3("Translate", &transform.translate.x, 0.1f);

	transform.rotate = transform.rotate * std::numbers::pi_v<float> / 180;
	backGrouds_[cursor]->SetIndex(index);
	backGrouds_[cursor]->SetTransform(transform);
	ImGui::End();
#endif

	colliderObject_->Update();

	//セピア調にする範囲
	GameEngine::RenderPreDraw("render");

	//背景
	skydome_->Draw();
	ground_->Draw();

	//ボス
	boss_->Draw();

	//弾
	for (auto& bullet : playerBullet_) {
		bullet->Draw();
	}
	for (auto& bullet : bossBullet_) {
		bullet->Draw();
	}

	//プレイヤー
	player_->Draw();

	//障害物
	colliderObject_->Draw();

	//背景オブジェクト
	for (auto& BG : backGrouds_) {
		BG->Draw();
	}
	tumbleweed_->Draw();
	fence_->Draw();

	//パーティクル
	editor_->Draw();
	editor_2->Draw();
	editor_3->Draw();
	editor_4->Draw();

	GameEngine::RenderPostDraw();

}

void GameScene::Draw() {
	
	GameEngine::DrawScreen("render", ColorChange::COLORMODE_SEPIATONE, gameCamera_->GetSepiaTone());

	//シリンダー
	int32_t remainingRounds = player_->GetRemainingRounds();
	std::vector<Parts> parts = cylinder_->GetParts();
	for (int32_t i = 1; i <= 6; i++) {
		if (remainingRounds >= i) {
			parts[i].material->color = { 1.0f,1.0f,1.0f,1.0f };
		}
		else {
			parts[i].material->color = { 1.0f,1.0f,1.0f,0.0f };
		}
		cylinder_->SetParts(parts[i], i);
	}
	cylinder_->Draw2D();

	//HUD
	for (auto& sprite : sprite_) {
		sprite->Draw2D();
	}

	//体力(ハット)
	for (int32_t i = 0; i < player_->GetHP();i++) {
		SRT transform = hatTransform_;
		transform.translate.x += i * hatTransform_.scale.x * 2;
		hat_->SetTransform(transform);
		hat_->Draw2D();
	}

	//フェード
	if (fade_ != Fade::None) {
		fadeSprite_->Draw2D();
	}
}

void GameScene::Collision() {
	std::list<BaseCharacter*> characterList;
	characterList.push_back(player_.get());
	characterList.push_back(boss_.get());

	std::list<Colliders*> colliders;
	colliders.push_back(player_.get()->GetColliders());
	colliders.push_back(boss_.get()->GetColliders());
	for (auto& bullet : playerBullet_) {
		colliders.push_back(bullet.get()->GetColliders());
	}
	for (auto& bullet : bossBullet_) {
		colliders.push_back(bullet.get()->GetColliders());
	}

	//障害物コライダー
	std::vector<OBBCollider> obbCollider = colliderObject_->GetCollider()->GetOBBColliders();

	for (std::list<Colliders*>::iterator iteratorA = colliders.begin();
		iteratorA != colliders.end(); iteratorA++) {

		//球接触判定
		std::vector<SphereCollider> sphereColliderA = (*iteratorA)->GetSphereColliders();

		for (std::list<Colliders*>::iterator iteratorB = iteratorA;
			iteratorB != colliders.end(); iteratorB++) {

			if (iteratorA == iteratorB)continue;

			//球接触判定
			std::vector<SphereCollider> sphereColliderB = (*iteratorB)->GetSphereColliders();

			if (sphereColliderA.size() > 0 && sphereColliderB.size() > 0) {
				for (int a = 0; a < sphereColliderA.size(); a++) {
					for (int b = 0; b < sphereColliderB.size(); b++) {

						//同一のグループの属するなら抜ける
						if (sphereColliderA[a].sourceId_ & 0b01 && sphereColliderB[b].targetId_ & 0b01 ||
							sphereColliderA[a].sourceId_ & 0b10 && sphereColliderB[b].targetId_ & 0b10 ||
							sphereColliderA[a].sourceId_ & 0b00 && sphereColliderB[b].targetId_ & 0b00) {
							continue;
						}

						// A->B
						if (!(sphereColliderA[a].sourceId_ & 0b100) && sphereColliderB[b].targetId_ & 0b100) {
							if (IsCollision(sphereColliderA[a].colliderSphere_, sphereColliderB[b].colliderSphere_)) {
								(*iteratorB)->IsCollision(sphereColliderA[a].sourceId_);
							}
						}
						// B->A
						if (!(sphereColliderB[b].sourceId_ & 0b100) && sphereColliderA[a].targetId_ & 0b100) {
							if (IsCollision(sphereColliderA[a].colliderSphere_, sphereColliderB[b].colliderSphere_)) {
								(*iteratorA)->IsCollision(sphereColliderB[b].sourceId_);
							}
						}


					}
				}
			}
		}


		for (int a = 0; a < sphereColliderA.size(); a++) {
			for (int i = 0; i < obbCollider.size(); i++) {
				if (IsCollision(obbCollider[i].colliderOBB_, (sphereColliderA[a].colliderSphere_))) {
					(*iteratorA)->IsCollision(0b100);
				}
			}

		}
	}
}

void GameScene::AddPlayerBullet(Vector3 translate, Vector3 rotate) {
	unique_ptr<PlayerBullet> newBullet = make_unique<PlayerBullet>();
	newBullet->Initialize(translate, rotate);
	playerBullet_.push_back(move(newBullet));

	pointLightElement_.intensity = 1.0f;

	particle_2->Emit();

	AudioHolder::GetInstance()->GetAudio(AudioIndex::Shot_SE).lock()->SoundPlayWave();
}

void GameScene::AddBossBullet(Vector3 translate, Vector3 rotate) {
	unique_ptr<BossBullet> newBullet = make_unique<BossBullet>();
	newBullet->Initialize(translate, rotate);
	bossBullet_.push_back(move(newBullet));

	AudioHolder::GetInstance()->GetAudio(AudioIndex::Shot_SE).lock()->SoundPlayWave();
}

std::vector<std::unique_ptr<BackGround>> GameScene::LoadBackGround(std::string csvfile) {
	// ファイルを開く
	std::ifstream file;
	file.open(csvfile);
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	std::stringstream LoadEnemyCommands_;
	LoadEnemyCommands_ << file.rdbuf();

	// ファイルを閉じる
	file.close();

	std::vector<std::unique_ptr<BackGround>> backGrouds;

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(LoadEnemyCommands_, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(line_stream, word, ':');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// 無視する
			continue;
		}

		// OBJECTコマンド
		if (word.find("OBJECT") == 0) {

			std::unique_ptr<BackGround> newBG = std::make_unique<BackGround>();
			SRT transform;

			//Index
			getline(line_stream, word, ',');
			int index = (uint32_t)std::atoi(word.c_str());

			while (line_stream && (line_stream.peek() == ',' || line_stream.peek() == '{')) { //','と'{'をスキップ
				line_stream.ignore();
			}

			//Scale
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());
			getline(line_stream, word, '}');
			float z = (float)std::atof(word.c_str());

			transform.scale = { x, y, z };

			while (line_stream && line_stream.peek() == '}') { //'}'をスキップ
				line_stream.ignore();
			}

			while (line_stream && (line_stream.peek() == ',' || line_stream.peek() == '{')) { //','と'{'をスキップ
				line_stream.ignore();
			}

			//Rotate
			getline(line_stream, word, ',');
			x = (float)std::atof(word.c_str());
			getline(line_stream, word, ',');
			y = (float)std::atof(word.c_str());
			getline(line_stream, word, '}');
			z = (float)std::atof(word.c_str());

			//わかりにくいので度数法で書くこと
			transform.rotate = { x * std::numbers::pi_v<float> / 180, y * std::numbers::pi_v<float> / 180, z * std::numbers::pi_v<float> / 180 };

			while (line_stream && line_stream.peek() == '}') { //'}'をスキップ
				line_stream.ignore();
			}

			while (line_stream && (line_stream.peek() == ',' || line_stream.peek() == '{')) { //','と'{'をスキップ
				line_stream.ignore();
			}

			//Translate
			getline(line_stream, word, ',');
			x = (float)std::atof(word.c_str());
			getline(line_stream, word, ',');
			y = (float)std::atof(word.c_str());
			getline(line_stream, word, '}');
			z = (float)std::atof(word.c_str());

			while (line_stream && line_stream.peek() == '}') { //'}'をスキップ
				line_stream.ignore();
			}

			transform.translate = { x, y, z };

			newBG->Initialize(index);
			newBG->SetTransform(transform);
			newBG->SetDirectionalLight(directionalLight_);
			newBG->SetPointLight(pointLight_);

			backGrouds.push_back(move(newBG));
		}
	}

	return backGrouds;
}

void GameScene::SaveBackGround(std::string csvfile) {
	// ファイルを開く
	std::ofstream file;
	file.open(csvfile);	//上書きモード
	assert(file.is_open());

	file << "//コマンド:パーツ番号,{Scale},{Rotate},{Translate}\n\n";

	for (int i = 0; i < backGrouds_.size(); i++) {

		uint32_t index = backGrouds_[i]->GetIndex();
		SRT Transform = backGrouds_[i]->GetTransform();

		file << "OBJECT:" << index << 
			",{" << Transform.scale.x << "," << Transform.scale.y << "," << Transform.scale.z <<
			"},{" << Transform.rotate.x / std::numbers::pi_v<float> * 180 << "," << Transform.rotate.y / std::numbers::pi_v<float> * 180 << "," << Transform.rotate.z / std::numbers::pi_v<float> * 180 <<
			"},{" << Transform.translate.x << "," << Transform.translate.y << "," << Transform.translate.z << "},\n";

	}

	// ファイルを閉じる
	file.close();
}