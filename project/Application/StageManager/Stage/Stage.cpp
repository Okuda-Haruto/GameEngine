#include "Stage.h"
#include "../StageManager.h"
#include <AudioHolder/AudioHolder.h>
#include <Math/Collision.h>

void Stage::Initialize(std::string stageName, std::shared_ptr<Input> input) {
	input_ = input;

	//ステージ名からステージデータを得る
	StageData stageData = StageManager::GetInstance()->GetStageData(stageName);

	//プレイヤー
	player_->Initialize(this, gameCamera_, input_, stageData.playerSpownPosition);

	//ボス
	boss_ = std::unique_ptr<Boss>();
	boss_->Initialize(stageData.bossData.filepath, this, gameCamera_, player_.get(), stageData.bossData.spownPosition);
}

void Stage::Update() {

	player_->Update();

	boss_->Update();

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

void Stage::Draw() {

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