#include "Stage.h"
#include <AudioHolder/AudioHolder.h>
#include <Math/Collision.h>
#include <GameEngine.h>
#include <numbers>

Stage::~Stage() {
	playerBullet_.clear();
	bossBullet_.clear();

#ifdef USE_IMGUI
	backGround_->SaveBackGround("resources/CSV/BackGround.csv");
#endif
}

void Stage::Initialize(StageData stageData, std::shared_ptr<Input> input) {
	input_ = input;

	//メインカメラ
	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Initialize(input_);
	gameCamera_->SetOffset(Vector3{ 0.0f,6.0f,-60.0f });
	gameCamera_->SetRotate(Vector3{ std::numbers::pi_v<float> / 180 * 10,0.0f,0.0f });
	SRT event{};
	event.translate = { 0.0f,5.0f,-35.0f };
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

	//背景
	backGround_ = std::make_unique<BackGround>();
	backGround_->Initialize("resources/CSV/BackGround.csv",gameCamera_,directionalLight_,pointLight_);

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize(this, gameCamera_, input_, stageData.playerStartTransform);
	gameCamera_->SetPlayer(player_->GetTransform());
	player_->SetCameraTransform(gameCamera_->GetTransform());
	player_->SetCamera(gameCamera_->GetCamera());
	player_->SetDirectionalLight(directionalLight_);
	player_->SetPointLight(pointLight_);

	//ボス
	boss_ = std::make_unique<Boss>();
	boss_->Initialize(stageData.bossData.filepath, this, gameCamera_, player_.get(), stageData.bossData.startTransform);
	gameCamera_->SetTarget(boss_->GetTransform());
	player_->SetBossTransform(boss_->GetTransform());
	boss_->SetCamera(gameCamera_->GetCamera());
	boss_->SetDirectionalLight(directionalLight_);
	boss_->SetPointLight(pointLight_);

	//接触可能オブジェクト
	for (auto& object : stageData.colliderObjects) {
		std::unique_ptr<ColliderObject> colliderObject = std::make_unique<ColliderObject>();
		colliderObject->Initialize(ModelManager::GetInstance()->GetModel(object.directoryPath, object.filename), object.startTransform);
		colliderObjects_.push_back(move(colliderObject));
	}

	//情報表示
	hud_ = std::make_unique<HUD>();
	hud_->Initialize(directionalLight_, player_.get());
}

void Stage::Update() {

	backGround_->Update();
	hud_->Update();

	if (!boss_->IsStartAnimation()) {
		gameCamera_->SetIsEvent(false);
		if (!debugCamera_) {
			player_->Update();
		}
	}

	gameCamera_->Update();

	if (!isClear_) {
		gameCamera_->SetMoveVelocity(player_->GetMove().x);
		gameCamera_->SetIsTargeted(player_->GetIsTargeted());

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

		for (auto& bullet : playerBullet_) {
			bullet->Update();
		}
		for (auto& bullet : bossBullet_) {
			bullet->Update();
		}
		for (auto& object : colliderObjects_) {
			object->Update();
		}

		if (!debugCamera_) {
			boss_->Update();
		}

		Collision();

		std::erase_if(playerBullet_, [](const auto& bullet) {
			return bullet->IsDead();
			});

		std::erase_if(bossBullet_, [](const auto& bullet) {
			return bullet->IsDead();
			});
	}

}

void Stage::Draw() {

	backGround_->Draw();

	player_->Draw();

	boss_->Draw();

	for (auto& bullet : playerBullet_) {
		bullet->Draw();
	}

	for (auto& bullet : bossBullet_) {
		bullet->Draw();
	}

	for (auto& object : colliderObjects_) {
		object->Draw();
	}

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
	for (auto& bullet : playerBullet_) {
		colliders.push_back(bullet.get()->GetColliders());
	}
	for (auto& bullet : bossBullet_) {
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
	playerBullet_.push_back(move(newBullet));

	//pointLightElement_.intensity = 1.0f;

	//particle_2->Emit();

	AudioHolder::GetInstance()->GetAudio(AudioIndex::Shot_SE).lock()->SoundPlayWave();
}

void Stage::AddBossBullet(Vector3 translate, Vector3 rotate) {
	unique_ptr<BossBullet> newBullet = make_unique<BossBullet>();
	newBullet->Initialize(translate, rotate);
	bossBullet_.push_back(move(newBullet));

	AudioHolder::GetInstance()->GetAudio(AudioIndex::Shot_SE).lock()->SoundPlayWave();
}