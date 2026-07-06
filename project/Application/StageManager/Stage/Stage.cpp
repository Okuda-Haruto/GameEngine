#include "Stage.h"
#include <AudioHolder/AudioHolder.h>
#include <Math/Collision.h>
#include <GameEngine.h>
#include <numbers>
#include <Math/Easing.h>

Stage::~Stage() {
	playerBullets_.clear();
	bossBullets_.clear();

#ifdef USE_IMGUI
	backGround_->SaveBackGround("resources/CSV/BackGround.csv");
#endif
}

void Stage::Initialize(StageData stageData, std::shared_ptr<Input> input) {
	input_ = input;

	BreakObject::SetStage(this);

	//メインカメラ
	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Initialize(input_);
	gameCamera_->ChangeCamera(std::make_unique<LockOnCamera>(), 0.0f);
	PlayerBullet::SetCamera(gameCamera_->GetCamera());
	BossBullet::SetCamera(gameCamera_->GetCamera());
	BreakObject::SetGameCamera(gameCamera_);

	directionalLight_ = std::make_unique<DirectionalLight>();
	directionalLight_->Initialize(GameEngine::GetDirectXCommon());
	directionalLightElement_.color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	directionalLightElement_.direction = Normalize(Vector3{ 0.0f,-1.0f,1.0f });
	directionalLightElement_.intensity = 1.0f;
	directionalLight_->SetDirectionalLightElement(directionalLightElement_);
	PlayerBullet::SetDirectionalLight(directionalLight_);
	BossBullet::SetDirectionalLight(directionalLight_);
	BreakObject::SetDirectionalLight(directionalLight_);

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
	BreakObject::SetPointLight(pointLight_);

	ParticleManager::GetInstance()->CreateParticleGroup("Particle_Sandstorm", "resources/Particle/sand.png");
	particle_ = std::make_unique<ParticleEmitter>("Particle_Sandstorm");

	Emitter emitter;
	emitter.count = 2;
	emitter.lifeTime = 3.0f;
	emitter.frequency = 0.1f;
	emitter.frequencyTime = 0.0f;
	emitter.spawnRange.min = { -5.0f,-0.5f,-50.0f };
	emitter.spawnRange.max = { 5.0f,0.5f,50.0f };
	emitter.angleBase = { -1.0f,0.0f,0.0f };
	emitter.angleRange = { 0.0f,0.0f,0.1f };	//方向範囲
	emitter.speedBase = 0.7f;	//基礎速度
	emitter.speedRange = 0.4f;	//速度範囲
	emitter.beforeColor = { 1.0f,1.0f,1.0f,1.0f };
	emitter.afterColor = { 1.0f,1.0f,1.0f,0.0f };
	emitter.rotateVelocity = 0.0f;
	emitter.rotateRate = std::numbers::pi_v<float>;
	particle_->SetEmitter(emitter);
	SRT emitterTransform;
	emitterTransform.scale = { 10.0f, 10.0f, 10.0f };
	emitterTransform.rotate = { 0, 0, 0 };
	emitterTransform.translate = { 50.0f, 0.0f, 0.0f };
	particle_->SetTransform(emitterTransform);

	ParticleManager::GetInstance()->CreateParticleGroup("Particle_Shot", "resources/Particle/particle.png");
	particle_2 = std::make_unique<ParticleEmitter>("Particle_Shot");

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
	emitter_2.afterColor = { 40.0f / 256.0f, 40.0f / 256.0f, 40.0f / 256.0f,0.0f };
	particle_2->SetEmitter(emitter_2);
	SRT emitterTransform_2;
	emitterTransform_2.scale = { 1.0f,1.0f,1.0f };
	emitterTransform_2.rotate = { 0.0f,0.0f,0.0f };
	emitterTransform_2.translate = { 0.0f,0.0f,0.0f };
	particle_2->SetTransform(emitterTransform_2);

	//パーティクル
	ParticleManager::GetInstance()->CreateParticleGroup("Particle_PlayerDamage", "resources/Particle/particle.png");
	particle_3 = std::make_unique<ParticleEmitter>("Particle_PlayerDamage");

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
	particle_3->SetEmitter(emitter_3);
	SRT emitterTransform_3;
	emitterTransform_3.scale = { 2.0f,0.1f,0.1f };
	emitterTransform_3.rotate = { 0.0f,0.0f,0.0f };
	emitterTransform_3.translate = { 0.0f,0.0f,0.0f };
	particle_3->SetTransform(emitterTransform_3);

	//パーティクル
	ParticleManager::GetInstance()->CreateParticleGroup("Particle_Move", "resources/Particle/sand.png");
	particle_4 = std::make_unique<ParticleEmitter>("Particle_Move");

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
	particle_4->SetEmitter(emitter_4);
	SRT emitterTransform_4;
	emitterTransform_4.scale = { 3.0f,3.0f,3.0f };
	emitterTransform_4.rotate = { 0.0f,0.0f,0.0f };
	emitterTransform_4.translate = { 0.0f,0.0f,0.0f };
	particle_4->SetTransform(emitterTransform_4);

	//背景
	backGround_ = std::make_unique<BackGround>();
	backGround_->Initialize("resources/CSV/BackGround.csv",gameCamera_,directionalLight_,pointLight_);

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize(this, gameCamera_, input_, stageData.playerStartTransform);
	gameCamera_->SetObserverTransform(player_->GetTransform());
	player_->SetCameraTransform(gameCamera_->GetTransform());
	player_->SetCamera(gameCamera_->GetCamera());
	player_->SetDirectionalLight(directionalLight_);
	player_->SetPointLight(pointLight_);

	//ボス
	boss_ = std::make_unique<Boss>();
	boss_->Initialize(stageData.bossData.filepath, this, gameCamera_, player_.get(), stageData.bossData.startTransform);
	boss_->SetCamera(gameCamera_->GetCamera());
	boss_->SetDirectionalLight(directionalLight_);
	boss_->SetPointLight(pointLight_);

	/*std::unique_ptr<BreakObject> breakObject = std::make_unique<BreakObject>();
	breakObject->Initialize("resources/Object", "Box.obj", SRT{ {1,1,1} ,{0,0,0},{10,1.5f,-10} }, 1);
	breakObjects_.push_back(move(breakObject));
	breakObject = std::make_unique<BreakObject>();
	breakObject->Initialize("resources/Object", "Box.obj", SRT{ {1,1,1} ,{0,0,0},{-10,1.5f,-10} }, 1);
	breakObjects_.push_back(move(breakObject));*/

	//接触可能オブジェクト
	for (auto& object : stageData.colliderObjects) {
		std::unique_ptr<ColliderObject> colliderObject = std::make_unique<ColliderObject>();
		colliderObject->Initialize(ModelManager::GetInstance()->GetModel(object.directoryPath, object.filename), directionalLight_, gameCamera_.get(), object.startTransform);
		colliderObjects_.push_back(move(colliderObject));
	}

	//情報表示
	hud_ = std::make_unique<HUD>();
	hud_->Initialize(directionalLight_, player_.get());

	ring_ = std::make_unique<PrimitiveRing>();
	ring_->Initialize(TextureManager::GetInstance()->GetSrvIndex("resources/Particle/gradationLine.png"), gameCamera_->GetCamera(), GameEngine::GetDirectXCommon());
	ringTransform_ = {};
	ringTransform_.scale = { 4,4,4 };
	ringMaterial_.color = { 1.0f,1.0f,1.0f,0.0f };
	ringMaterial_.uvTransform = MakeTranslateMatrix({ 1.0f,1.0f,1.0f });

	cylider_ = std::make_unique<PrimitiveCylinder>();
	cylider_->Initialize(TextureManager::GetInstance()->GetSrvIndex("resources/DebugResources/gradationLine.png"), gameCamera_->GetCamera(), GameEngine::GetDirectXCommon());
	cylinderTransform_ = boss_->GetTransform();
	cylinderTransform_.translate.y = 0.0f;
	cylinderTransform_.scale = { 1.5f,3.0f,1.5f };
	cylinderMaterial_.color = { 1.0f,1.0f,1.0f,1.0f };
	cylinderMaterial_.uvTransform = MakeIdentity4x4();
	cylinderTime_ = 0.0f;
}

void Stage::Update() {
	Keyboard keyboard = input_->GetKeyboard();
	Pad pad = input_->GetPad();

	backGround_->Update();
	hud_->Update();

	if (!boss_->IsStartAnimation()) {
		if (!debugCamera_) {
			player_->Update();
		}

		if ((keyboard.keys[DIK_LSHIFT].hold || keyboard.keys[DIK_RSHIFT].hold) ||
			(pad.isConnected && pad.Button[PAD_BUTTON_LT].hold)) {
			gameCamera_->ChangeCamera(std::make_unique<WideViewCamera>(), 0.1f);
		}
		if ((keyboard.keys[DIK_LSHIFT].idle && keyboard.keys[DIK_RSHIFT].idle) &&
			!(pad.isConnected && pad.Button[PAD_BUTTON_LT].hold)) {
			gameCamera_->ChangeCamera(std::make_unique<LockOnCamera>(), 0.1f);
		}
	}

	gameCamera_->Update();

	if (!isClear_) {

		if (boss_->IsDead()) {
			isClear_ = true;
			isEnd_ = true;
		}
		if (player_->IsDead()) {
			isClear_ = false;
			isEnd_ = true;
		}
		if (pointLightElement_.intensity > 0.0f) {
			pointLightElement_.intensity -= 0.05f;
			if (pointLightElement_.intensity < 0.0f)pointLightElement_.intensity = 0.0f;
			Matrix4x4 rotateMatrix = MakeRotateYMatrix(player_->GetTransform()->rotate.y);
			pointLightElement_.position = player_->GetTransform()->translate + rotateMatrix * Vector3(0.0f, 0.0f, 1.0f);
			pointLight_->SetPointLightElement(pointLightElement_);
		}

		for (auto& bullet : playerBullets_) {
			bullet->Update();
		}
		for (auto& bullet : bossBullets_) {
			bullet->Update();
		}
		for (auto& object : breakObjects_) {
			object->Update();
		}
		for (auto& object : colliderObjects_) {
			object->Update();
		}

		if (!debugCamera_) {
			boss_->Update();
		}

		Collision();

		std::erase_if(playerBullets_, [](const auto& bullet) {
			return bullet->IsDead();
			});

		std::erase_if(bossBullets_, [](const auto& bullet) {
			return bullet->IsDead();
			});
		std::erase_if(breakObjects_, [](const auto& object) {
			return object->IsDead();
			});
	}




	particle_->Update();
	particle_2->Update();
	particle_3->Update();
	particle_4->Update();

	SRT transform;
	transform = particle_3->GetTransform();
	transform.translate = player_->GetTransform()->translate;
	transform.scale = { 1.0f,5.0f,5.0f };
	particle_3->SetTransform(transform);

	ringTransform_.translate = boss_->GetTransform().translate + Vector3{ 0,0,1 } * MakeRotateYMatrix(boss_->GetTransform().rotate.y);
	if (ringMaterial_.color.w > 0.0f) {
		ringMaterial_.color.w -= 2.0f / 60.0f;
		if (ringMaterial_.color.w < 0.0f) {
			ringMaterial_.color.w = 0.0f;
		}
	}

	if (cylinderTime_ < kMaxCyliderTime_) {
		cylinderTime_ += 1.0f / 60.0f;
		if (cylinderTime_ > kMaxCyliderTime_) {
			cylinderTime_ = kMaxCyliderTime_;
		}
	}

	cylinderTransform_.scale = Easing::EaseOut(Vector3{ 1.5f,3.0f,1.5f }, Vector3{4.5f,0.0f,4.5f}, cylinderTime_ / kMaxCyliderTime_);
	cylinderMaterial_.color.w = Easing::EaseOut(0.0f, 1.0f, cylinderTime_ / kMaxCyliderTime_);
	cylinderMaterial_.uvTransform = MakeTranslateMatrix({ Easing::EaseOut(0.0f,1.0f,cylinderTime_ / kMaxCyliderTime_),0.0f,0.0f });
}

void Stage::Draw() {

	player_->Draw();

	boss_->Draw();

	for (auto& bullet : playerBullets_) {
		bullet->Draw();
	}

	for (auto& bullet : bossBullets_) {
		bullet->Draw();
	}

	backGround_->Draw();

	for (auto& object : breakObjects_) {
		object->Draw();
	}

	for (auto& object : colliderObjects_) {
		object->Draw();
	}

	ring_->DrawBillBoard(ringTransform_, ringMaterial_);

	if(cylinderTime_ < kMaxCyliderTime_) {
		cylider_->Draw(cylinderTransform_, cylinderMaterial_);
	}

	particle_->Draw();
	particle_2->Draw();
	particle_3->Draw();
	particle_4->Draw();

	hud_->Draw();
}

void Stage::DrawSprite() {
	hud_->DrawSprite();
}

void Stage::Collision() {
	std::list<BaseCharacter*> characterList;
	characterList.push_back(player_.get());
	characterList.push_back(boss_.get());

	std::list<Colliders*> colliders;
	colliders.push_back(player_.get()->GetColliders());
	colliders.push_back(boss_.get()->GetColliders());
	for (auto& bullet : playerBullets_) {
		colliders.push_back(bullet.get()->GetColliders());
	}
	for (auto& bullet : bossBullets_) {
		colliders.push_back(bullet.get()->GetColliders());
	}

	//障害物コライダー
	std::vector<OBBCollider> obbCollider;
	for (auto& colliderObject_ : colliderObjects_) {
		std::vector<OBBCollider> colliders = colliderObject_->GetCollider()->GetOBBColliders();
		for (auto& collider : colliders) {
			obbCollider.push_back(collider);
		}
	}

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

						if (IsCollision(sphereColliderA[a].colliderSphere_, sphereColliderB[b].colliderSphere_)) {
							(*iteratorA)->IsCollision(sphereColliderB[b].sourceId_);
							(*iteratorB)->IsCollision(sphereColliderA[a].sourceId_);
						}


					}
				}
			}
		}


		for (int a = 0; a < sphereColliderA.size(); a++) {
			for (int i = 0; i < obbCollider.size(); i++) {
				if (IsCollision(obbCollider[i].colliderOBB_, (sphereColliderA[a].colliderSphere_))) {
					(*iteratorA)->IsCollision(obbCollider[i].sourceId_);
				}
			}

		}
	}
}

void Stage::AddPlayerBullet(Vector3 translate, Vector3 rotate) {
	unique_ptr<PlayerBullet> newBullet = make_unique<PlayerBullet>();
	newBullet->Initialize(translate, rotate);
	playerBullets_.push_back(move(newBullet));

	Matrix4x4 rotateMatrix = MakeRotateYMatrix(player_->GetTransform()->rotate.y);
	pointLightElement_.position = player_->GetTransform()->translate + rotateMatrix * Vector3(0.0f, 0.0f, 1.0f);
	pointLight_->SetPointLightElement(pointLightElement_);

	pointLightElement_.intensity = 1.0f;

	SRT transform = particle_2->GetTransform();
	Emitter emitter = particle_2->GetEmitter();
	transform.translate = pointLightElement_.position;
	emitter.angleBase = Normalize(rotateMatrix * Vector3(0.0f, 0.0f, 0.8f) + Vector3(0.0f, 1.0f, 0.0f));
	particle_2->SetTransform(transform);
	particle_2->SetEmitter(emitter);

	particle_2->Emit();

	AudioHolder::GetInstance()->GetAudio(AudioIndex::Shot_SE).lock()->SoundPlayWave();
}

void Stage::AddBossBullet(Vector3 translate, Vector3 rotate) {
	unique_ptr<BossBullet> newBullet = make_unique<BossBullet>();
	newBullet->Initialize(translate, rotate);
	bossBullets_.push_back(move(newBullet));

	Matrix4x4 rotateMatrix = MakeRotateYMatrix(boss_->GetTransform().rotate.y);
	pointLightElement_.position = boss_->GetTransform().translate + rotateMatrix * Vector3(0.0f, 0.0f, 1.0f);
	pointLight_->SetPointLightElement(pointLightElement_);

	pointLightElement_.intensity = 1.0f;

	SRT transform = particle_2->GetTransform();
	Emitter emitter = particle_2->GetEmitter();
	transform.translate = pointLightElement_.position;
	emitter.angleBase = Normalize(rotateMatrix * Vector3(0.0f, 0.0f, 0.8f) + Vector3(0.0f, 1.0f, 0.0f));
	particle_2->SetTransform(transform);
	particle_2->SetEmitter(emitter);

	particle_2->Emit();

	AudioHolder::GetInstance()->GetAudio(AudioIndex::Shot_SE).lock()->SoundPlayWave();
}