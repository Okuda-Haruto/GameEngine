#include "GameScene.h"
#include "GameEngine.h"
#include <numbers>
#include <SceneManager/SceneManager.h>
#include "../TitleScene/TitleScene.h"

GameScene::~GameScene() {
	for (PlayerBullet* bullet : playerBullet_) {
		delete bullet;
		bullet = nullptr;
	}
	playerBullet_.clear();
	for (BossBullet* bullet : bossBullet_) {
		delete bullet;
		bullet = nullptr;
	}
	bossBullet_.clear();

	for (Sprite* sprite : sprite_) {
		delete sprite;
	}
	ParticleManager::GetInstance()->Reset();
}

void GameScene::Initialize() {
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
	emitter_.spawnRange.max = { 5.0f,10.0f,50.0f };
	emitter_.angleBase = { -1.0f,0.0f,0.0f };
	emitter_.angleRange = { 0.0f,0.1f,0.1f };	//方向範囲
	emitter_.speedBase = 0.7f;	//基礎速度
	emitter_.speedRange = 0.4f;	//速度範囲
	emitter_.beforeColor = { 1.0f,1.0f,1.0f,68.0f / 256.0f };
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
	emitter_2.beforeColor = { 0.1f,0.1f,0.1f,1.0f };
	emitter_2.afterColor = { 0.0f,0.0f,0.0f,0.0f };
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
	emitter_3.afterColor = { 1.0f,0.0f,0.0f,0.0f };
	editor_3->SetEmitter(emitter_3);
	AccelerationField field_3;
	field_3.area.min = { 0.0f,0.0f,0.0f };
	field_3.area.max = { 1.0f,1.0f,1.0f };
	field_3.acceleration = {};
	editor_3->SetField(field_3);

	//パーティクル
	ParticleManager::GetInstance()->CreateParticleGroup("particle_4", "resources/Particle/Sand.png");
	particle_4 = std::make_unique<ParticleEmitter>("particle_4");
	editor_4 = std::make_unique<ParticleEditor>();
	editor_4->Initialize(particle_4.get());

	Emitter emitter_4;
	emitter_4.count = 1;
	emitter_4.lifeTime = 0.2f;
	emitter_4.frequency = 0.0f;
	emitter_4.frequencyTime = 0.0f;
	emitter_4.transform.scale = { 2.0f,2.0f,2.0f };
	emitter_4.transform.translate = { 0.0f,0.0f,0.0f };
	emitter_4.spawnRange.min = { 0.0f,0.0f,0.0f };
	emitter_4.spawnRange.max = { 0.0f,0.0f,0.0f };
	emitter_4.angleBase = { 0.0f,0.0f,1.0f };
	emitter_4.angleRange = { 0.2f,0.2f,0.2f };	//方向範囲
	emitter_4.speedBase = 0.2f;	//基礎速度
	emitter_4.speedRange = 0.1f;	//速度範囲
	emitter_4.beforeColor = { 0.4f,0.4f,0.4f,0.4f };
	emitter_4.afterColor = { 0.0f,0.0f,0.0f,0.0f };
	editor_4->SetEmitter(emitter_4);
	AccelerationField field_4;
	field_4.area.min = { 0.0f,0.0f,0.0f };
	field_4.area.max = { 1.0f,1.0f,1.0f };
	field_4.acceleration = {};
	editor_4->SetField(field_4);

	//メインカメラ
	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Initialize();
	gameCamera_->SetOffset(Vector3{ 0.0f,6.0f,-60.0f });
	gameCamera_->SetRotate(Vector3{ std::numbers::pi_v<float> / 180 * 10,0.0f,0.0f });
	PlayerBullet::SetCamera(gameCamera_->GetCamera());
	BossBullet::SetCamera(gameCamera_->GetCamera());

	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Initialize(GameEngine::GetDirectXCommon());
	directionalLightElement_.color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	directionalLightElement_.direction = Normalize(Vector3{ 0.0f,-1.0f,1.0f });
	directionalLightElement_.intensity = 1.0f;
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);
	PlayerBullet::SetDirectionalLight(directionalLight_.get());
	BossBullet::SetDirectionalLight(directionalLight_.get());

	pointLight_ = std::make_unique<PointLight>();
	pointLight_->Initialize(GameEngine::GetDirectXCommon());
	pointLightElement_.color = Vector4{ 1.0f,0.8f,0.6f,1.0f };
	pointLightElement_.intensity = 0.0f;
	pointLightElement_.radius = 4.0f;
	pointLightElement_.position = {};
	pointLightElement_.decay = 1.0f;
	pointLight_->SetPointLightElement(pointLightElement_);
	PlayerBullet::SetPointLight(pointLight_.get());
	BossBullet::SetPointLight(pointLight_.get());

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize(this,gameCamera_.get(), particle_4.get());
	gameCamera_->SetPlayer(player_->GetTransform());
	player_->SetCameraTransform(gameCamera_->GetTransform());
	player_->SetCamera(gameCamera_->GetCamera());
	player_->SetDirectionalLight(directionalLight_.get());
	player_->SetPointLight(pointLight_.get());

	//ボス
	boss_ = std::make_unique<Boss>();
	boss_->Initialize(this,gameCamera_.get(), particle_3.get(), player_.get(), 30.0f);
	gameCamera_->SetTarget(boss_->GetTransform());
	player_->SetBossTransform(boss_->GetTransform());
	boss_->SetCamera(gameCamera_->GetCamera());
	boss_->SetDirectionalLight(directionalLight_.get());
	boss_->SetPointLight(pointLight_.get());

	cylinder_ = std::make_unique<Object>();
	cylinder_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Cylinder));
	cylinder_->SetDirectionalLight(directionalLight_.get());
	cylinderTransform_.scale = { 0.0005f, 0.0005f, 0.0005f };
	cylinderTransform_.rotate.x = std::numbers::pi_v<float> / 180 * 9;
	cylinderTransform_.rotate.y = std::numbers::pi_v<float> / 180 * -15;
	cylinderTransform_.translate = { -0.0032f,-0.0015f,0.01f };
	cylinder_->SetTransform(cylinderTransform_);

	hat_ = std::make_unique<Object>();
	hat_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Hat));
	hat_->SetDirectionalLight(directionalLight_.get());
	hatTransform_.scale = { 0.0003f,0.0003f,0.0003f };
	hatTransform_.rotate.x = std::numbers::pi_v<float> / 180 * -25;
	hatTransform_.translate = { -0.0038f,0.0018f,0.01f };
	hat_->SetTransform(hatTransform_);
	animationTime_ = 0.0f;

	sprite_[0] = new Sprite;
	sprite_[0]->Initialize("resources/Sprite/LT.png");
	sprite_[0]->SetPosition(Vector2{ 200,720 - 84 });
	sprite_[0]->SetSize(Vector2{ 64,64 });
	sprite_[1] = new Sprite;
	sprite_[1]->Initialize("resources/Sprite/RT.png");
	sprite_[1]->SetPosition(Vector2{ 270,720 - 84 });
	sprite_[1]->SetSize(Vector2{ 64,64 });
	sprite_[2] = new Sprite;
	sprite_[2]->Initialize("resources/Sprite/B.png");
	sprite_[2]->SetPosition(Vector2{ 340,720 - 84 });
	sprite_[2]->SetSize(Vector2{ 64,64 });
	sprite_[3] = new Sprite;
	sprite_[3]->Initialize("resources/Sprite/Reload_UI.png");
	sprite_[3]->SetPosition(Vector2{ 410,720 - 84 });
	sprite_[3]->SetSize(Vector2{ 64,64 });

	//背景
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();
	skydome_->SetCamera(gameCamera_->GetCamera());
	ground_ = std::make_unique<Ground>();
	ground_->Initialize();
	ground_->SetCamera(gameCamera_->GetCamera());
	ground_->SetDirectionalLight(directionalLight_.get());
	ground_->SetPointLight(pointLight_.get());

	fence_ = std::make_unique<Fence>();
	fence_->Initialize(gameCamera_->GetCamera(),directionalLight_.get(),pointLight_.get());

	fadeSprite_ = std::make_unique<Sprite>();
	fadeSprite_->Initialize("resources/DebugResources/white2x2.png");
	fadeSprite_->SetSize({ 1280,720 });
	fadeSprite_->SetColor({ 0.0f,0.0f,0.0f,1.0f });

	fade_ = Fade::FadeIn;
	fadeTime_ = 0.0f;
}

void GameScene::Update() {
	Keybord key = GameEngine::GetKeybord();

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
		SceneManager::GetInstance()->ChangeScene("Title");
	}

	if (fade_ == Fade::None) {
		player_->Update();
	}

	gameCamera_->SetMoveVelocity(player_->GetMove().x);
	gameCamera_->SetIsTargeted(player_->GetIsTargeted());
	gameCamera_->Update();

	if ((player_->IsDead() || boss_->IsDead()) && fade_ == Fade::None) {
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

	for (PlayerBullet* bullet : playerBullet_) {
		bullet->Update();
	}

	for (BossBullet* bullet : bossBullet_) {
		bullet->Update();
	}

	boss_->Update();

	Collision();

	// デスフラグの立った弾の削除
	playerBullet_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			bullet = nullptr;
			return true;
		}
		return false;
	});

	bossBullet_.remove_if([](BossBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			bullet = nullptr;
			return true;
		}
		return false;
		});



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

	for (Sprite* sprite : sprite_) {
		sprite->Update();
	}

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
#endif
}

void GameScene::Draw() {
	//背景
	skydome_->Draw();
	ground_->Draw();
	fence_->Draw();

	//プレイヤー
	boss_->Draw();

	editor_->Draw();
	editor_2->Draw();
	editor_3->Draw();
	editor_4->Draw();

	for (PlayerBullet* bullet : playerBullet_) {
		bullet->Draw();
	}

	for (BossBullet* bullet : bossBullet_) {
		bullet->Draw();
	}

	player_->Draw();

	int32_t remainingRounds = player_->GetRemainingRounds();
	std::vector<Parts> parts = cylinder_->GetParts();
	for (int32_t i = 1; i <= 6;i++) {
		if (remainingRounds >= i) {
			parts[i].material->color = { 1.0f,1.0f,1.0f,1.0f };
		} else {
			parts[i].material->color = { 1.0f,1.0f,1.0f,0.0f };
		}
		cylinder_->SetParts(parts[i], i);
	}
	cylinder_->Draw2D();

	for (Sprite* sprite : sprite_) {
		sprite->Draw2D();
	}

	for (int32_t i = 0; i < player_->GetHP();i++) {
		SRT transform = hatTransform_;
		transform.translate.x += i * hatTransform_.scale.x * 2;
		hat_->SetTransform(transform);
		hat_->Draw2D();
	}

	if (fade_ != Fade::None) {
		fadeSprite_->Draw2D();
	}
}

void GameScene::Collision() {
	std::list<BaseBullet*> bulletList;
	for (std::list<PlayerBullet*>::iterator iterator = playerBullet_.begin();
		iterator != playerBullet_.end(); iterator++) {
		bulletList.push_back(*iterator);
	}
	for (std::list<BossBullet*>::iterator iterator = bossBullet_.begin();
		iterator != bossBullet_.end(); iterator++) {
		bulletList.push_back(*iterator);
	}

	std::list<BaseCharacter*> characterList;
	characterList.push_back(player_.get());
	characterList.push_back(boss_.get());

	std::list<Collider*> collider;
	collider.push_back(player_.get());
	collider.push_back(boss_.get());
	for (BaseBullet* bullet : playerBullet_) {
		collider.push_back(bullet);
	}
	for (BaseBullet* bullet : bossBullet_) {
		collider.push_back(bullet);
	}

	for (std::list<Collider*>::iterator iteratorA = collider.begin();
		iteratorA != collider.end(); iteratorA++) {
		for (std::list<Collider*>::iterator iteratorB = iteratorA;
			iteratorB != collider.end(); iteratorB++) {

			if (iteratorA == iteratorB)continue;

			uint8_t idA = (*iteratorA)->GetID(), idB = (*iteratorB)->GetID();

			//プレイヤー側と敵側の場合
			if ((idA & 0b01 && idB & 0b10) ||
				(idA & 0b10 && idB & 0b01)) {
				
				if ((*iteratorA)->GetInvincible() || (*iteratorB)->GetInvincible())continue;

				if (Length((*iteratorA)->GetSphare().center - (*iteratorB)->GetSphare().center) <=
					((*iteratorA)->GetSphare().radius + (*iteratorB)->GetSphare().radius)) {
					(*iteratorA)->IsCollision();
					(*iteratorB)->IsCollision();
				}
			//プレイヤー側とアイテムの場合
			} else if (idA & 0b01 && idB & 0b00) {
				//プレイヤーがアイテムを取得
				if (idA == CollisionID_Player_Character && idB == CollisionID_Item_Bullet) {
					if (Length((*iteratorA)->GetSphare().center - (*iteratorB)->GetSphare().center) <=
						((*iteratorA)->GetSphare().radius + (*iteratorB)->GetSphare().radius)) {
						(*iteratorB)->IsCollision();
					}
				}
				//プレイヤー弾がボックスを破壊
				if (idA == CollisionID_Player_Bullet && idB == CollisionID_Item_Character) {
					if (Length((*iteratorA)->GetSphare().center - (*iteratorB)->GetSphare().center) <=
						((*iteratorA)->GetSphare().radius + (*iteratorB)->GetSphare().radius)) {
						(*iteratorA)->IsCollision();
						(*iteratorB)->IsCollision();
					}
				}
			} else if (idA & 0b00 && idB & 0b01) {
				//プレイヤーがアイテムを取得
				if (idA == CollisionID_Item_Bullet && idB == CollisionID_Player_Character) {
					if (Length((*iteratorA)->GetSphare().center - (*iteratorB)->GetSphare().center) <=
						((*iteratorA)->GetSphare().radius + (*iteratorB)->GetSphare().radius)) {
						(*iteratorA)->IsCollision();
					}
				}
				//プレイヤー弾がボックスを破壊
				if (idA == CollisionID_Item_Character && idB == CollisionID_Player_Bullet) {
					if (Length((*iteratorA)->GetSphare().center - (*iteratorB)->GetSphare().center) <=
						((*iteratorA)->GetSphare().radius + (*iteratorB)->GetSphare().radius)) {
						(*iteratorA)->IsCollision();
						(*iteratorB)->IsCollision();
					}
				}
			}

			//キャラクター同士の場合
			if (idA & 0b100 && idB & 0b100) {

				if (Length((*iteratorA)->GetSphare().center - (*iteratorB)->GetSphare().center) <=
					((*iteratorA)->GetSphare().radius + (*iteratorB)->GetSphare().radius)) {
					
				}
			}
		}
	}

}

void GameScene::AddPlayerBullet(Vector3 translate, Vector3 rotate) {
	PlayerBullet* newBullet = new PlayerBullet;
	newBullet->Initialize(translate, rotate);
	playerBullet_.push_back(newBullet);

	pointLightElement_.intensity = 1.0f;

	particle_2->Emit();
}

void GameScene::AddBossBullet(Vector3 translate, Vector3 rotate) {
	BossBullet* newBullet = new BossBullet;
	newBullet->Initialize(translate, rotate);
	bossBullet_.push_back(newBullet);
}