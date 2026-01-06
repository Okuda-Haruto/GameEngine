#include "Player.h"
#include "GameEngine.h"
#include <Vector3.h>
#include <Matrix4x4.h>
#include <numbers>
#include "Math/Lerp.h"
#include "Scene/GameScene/GameScene.h"
#include "Input/Input.h"

Player::~Player() {

}

void Player::Initialize(GameScene* gameScene, GameCamera* gameCamera, shared_ptr<Input> input, ParticleEmitter* particle1) {
	gameScene_ = gameScene;
	gameCamera_ = gameCamera;
	input_ = input;
	particle_1 = particle1;
	//パーティクル
	ParticleManager::GetInstance()->CreateParticleGroup("particle_5", "resources/Particle/Sand.png");
	particle_2 = std::make_unique<ParticleEmitter>("particle_5");

	emitter_.count = 8;
	emitter_.lifeTime = 0.2f;
	emitter_.frequency = 0.0f;
	emitter_.frequencyTime = 0.0f;
	emitter_.transform.scale = { 2.0f,2.0f,2.0f };
	emitter_.transform.translate = { 0.0f,0.0f,0.0f };
	emitter_.spawnRange.min = { -0.5f,0.0f,-0.5f };
	emitter_.spawnRange.max = { 0.5f,0.0f,0.5f };
	emitter_.angleBase = { 0.0f,0.0f,0.0f };
	emitter_.angleRange = { 1.0f,0.0f,1.0f };	//方向範囲
	emitter_.speedBase = 0.2f;	//基礎速度
	emitter_.speedRange = 0.1f;	//速度範囲
	emitter_.beforeColor = { 0.2f,0.2f,0.2f,0.2f };
	emitter_.afterColor = { 0.0f,0.0f,0.0f,0.0f };
	particle_2->SetEmitter(emitter_);

	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Player));
	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.rotate = { 0.0f,0.0f,0.0f };
	transform_.translate = { 0.0f,1.0f,-20.0f };
	object_->SetTransform(transform_);

	targetTransform_ = std::make_unique<SRT>();
	*targetTransform_ = transform_;
	dodgeCoolTime = 0.0f;
	dodgeActiveTime = kMaxDodgeActiveTime;
	shotCooltime_ = 0.0f;
	stopTime_ = 0.0f;

	velocity_ = {};

	HP_ = kMaxHP;
	remainingRounds_ = kMaxRemainingRounds;
	reloadTime_ = 0.0f;
	invincibleTime_ = 0.0f;

	isTargeted_ = false;

	InitializeCollider(1.0f, CollisionID_Player_Character);
	UpdateCollider();
}

void Player::Update() {
	Pad pad = input_->GetPad(0);
	Keybord keys = input_->GetKeyBord();

	move_ = {};
	isMove = false;

	if (stunTime > 0.0f) {
		stunTime -= 1.0f / 60.0f;
		if (stunTime < 0.0f) {
			stunTime = 0.0f;
		}
	}
	if (invincibleTime_ > 0.0f) {
		invincibleTime_ -= 1.0f / 60.0f;
		if (invincibleTime_ <= 0.0f) {
			invincibleTime_ = 0.0f;
			SetInvincible(false);
		}
	}

	if (stunTime > 0.0f) {
		transform_.rotate.x = std::numbers::pi_v<float> / 2;
		transform_.rotate.z = 0.0f;
		transform_.translate.y = 0.5f;
	} else {
		transform_.rotate.x = 0.0f;
		transform_.rotate.z = 0.0f;
		transform_.translate.y = 1.0f;
	}

	if (stunTime <= 0.0f) {
		//移動量
		if (pad.isConnected) {
			move_ = {
				pad.LeftStick.vector.x * pad.LeftStick.magnitude,0.0f,pad.LeftStick.vector.y * pad.LeftStick.magnitude
			};
		}
		if (Length(move_) > deadZone) {
			isMove = true;
		}

		if (!isMove) {
			if (keys.hold[DIK_W] || keys.hold[DIK_UP]) {
				move_.z = 1.0f;
			}
			if (keys.hold[DIK_S] || keys.hold[DIK_DOWN]) {
				move_.z = -1.0f;
			}
			if (keys.hold[DIK_D] || keys.hold[DIK_RIGHT]) {
				move_.x = 1.0f;
			}
			if (keys.hold[DIK_A] || keys.hold[DIK_LEFT]) {
				move_.x = -1.0f;
			}
			if (Length(move_) > deadZone) {
				isMove = true;
			}
		}

#pragma region 移動処理

		//注目中は角度をつける
		if (isTargeted_ && bossTransform_) {
			Vector3 diff = Normalize(bossTransform_->translate - transform_.translate);
			angle = std::atan2(diff.x, diff.z);
		}

		acceleration_ = {};
		//回避中は移動させないこと
		if (isMove && dodgeActiveTime >= kMaxDodgeActiveTime) {
			//移動量に速さを反映
			acceleration_ = Normalize(move_) * speed;

			Matrix4x4 rotateMatrix = MakeRotateYMatrix(cameraTransform_->rotate.y);
			acceleration_ = rotateMatrix * acceleration_;

			if (isTargeted_) {
				acceleration_ /= 4.0f;
			}

			velocity_ += acceleration_;
		}

		if (Length(velocity_) > 0.0f) {
			if (Length(acceleration_) <= 0.0f) {
				velocity_ = velocity_ * 0.8f;
				if (Length(velocity_) < 0.01f) {
					velocity_ = {};
				}
			}
		}

		if(Length(velocity_) > 0.0f){

			if (Length(velocity_) > kMaxSpeed) {
				velocity_ = Normalize(velocity_) * kMaxSpeed;
			}

			if (isTargeted_ && Length(velocity_) > kMaxSpeed / 2) {
				velocity_ = Normalize(velocity_) * kMaxSpeed / 2;
			}

			//移動
			transform_.translate += velocity_;

			transform_.translate.x = std::clamp(transform_.translate.x, -59.0f, 59.0f);
			transform_.translate.z = std::clamp(transform_.translate.z, -59.0f, 59.0f);

			if (!(isTargeted_ && bossTransform_)) {
				angle = std::atan2(velocity_.x, velocity_.z);
			}

			particle_1->Emit();
		}

		//最短角度補完
		float diff = angle - transform_.rotate.y;

		if (diff >= std::numbers::pi_v<float>*2) {
			diff = angle - transform_.rotate.y;
		}

		diff = std::fmodf(diff, std::numbers::pi_v<float> *2);

		if (diff > std::numbers::pi_v<float>) {
			diff -= std::numbers::pi_v<float> *2;
		} else if (diff < -std::numbers::pi_v<float>) {
			diff += std::numbers::pi_v<float> *2;
		}

		if (diff > std::numbers::pi_v<float>) {
			diff -= std::numbers::pi_v<float> *2;
		} else if (diff < -std::numbers::pi_v<float>) {
			diff += std::numbers::pi_v<float> *2;
		}

		//正直あまりやりたくはない方法だが2πを超えると遠回りで回転してしまうので致し方無い
		transform_.rotate.y = (transform_.rotate.y + diff * 0.2f);
		transform_.rotate.y = std::fmodf(transform_.rotate.y, std::numbers::pi_v<float> *2);

#pragma endregion

#pragma region 回避行動

		//回避中
		if (dodgeActiveTime < kMaxDodgeActiveTime) {
			dodgeActiveTime += 1.0f / 60.0f;
			//kMaxDodgeActiveTimeを超えると角度がおかしくなる気がするので戻す
			if (dodgeActiveTime >= kMaxDodgeActiveTime) {
				dodgeActiveTime = kMaxDodgeActiveTime;
			}
			switch (dodgeAngle_)
			{
			case Player::DODGE_ANGLE::FRONT:
				//前転
				velocity_ = Vector3{ 0.0f,0.0f,1.0f } *Lerp(0.2f, dodgeSpeed, powf(1.0f - dodgeActiveTime / kMaxDodgeActiveTime, 3));
				transform_.rotate.x = Lerp(0.0f, std::numbers::pi_v<float> *2, 1.0f - powf(1.0f - dodgeActiveTime / kMaxDodgeActiveTime, 3));
				break;
			case Player::DODGE_ANGLE::BACK:
				//後転
				velocity_ = Vector3{ 0.0f,0.0f,-1.0f } *Lerp(0.2f, dodgeSpeed, powf(1.0f - dodgeActiveTime / kMaxDodgeActiveTime, 3));
				transform_.rotate.x = -Lerp(0.0f, std::numbers::pi_v<float> *2, 1.0f - powf(1.0f - dodgeActiveTime / kMaxDodgeActiveTime, 3));
				break;
			case Player::DODGE_ANGLE::RIGHT:
				//右側転
				velocity_ = Vector3{ 1.0f,0.0f,0.0f } *Lerp(0.2f, dodgeSpeed, powf(1.0f - dodgeActiveTime / kMaxDodgeActiveTime, 3));
				transform_.rotate.z = -Lerp(0.0f, std::numbers::pi_v<float> *2, 1.0f - powf(1.0f - dodgeActiveTime / kMaxDodgeActiveTime, 3));
				break;
			case Player::DODGE_ANGLE::LEFT:
				//左側転
				velocity_ = Vector3{ -1.0f,0.0f,0.0f } *Lerp(0.2f, dodgeSpeed, powf(1.0f - dodgeActiveTime / kMaxDodgeActiveTime, 3));
				transform_.rotate.z = Lerp(0.0f, std::numbers::pi_v<float> *2, 1.0f - powf(1.0f - dodgeActiveTime / kMaxDodgeActiveTime, 3));
				break;
			default:
				break;
			}

			Matrix4x4 rotateMatrix = MakeRotateYMatrix(transform_.rotate.y);
			velocity_ = rotateMatrix * velocity_;

			//移動
			transform_.translate += velocity_;

			if (transform_.translate.x > 59.0f || transform_.translate.x < -59.0f ||
				transform_.translate.z > 59.0f || transform_.translate.z < -59.0f) {
				transform_.translate.x = std::clamp(transform_.translate.x, -59.0f, 59.0f);
				transform_.translate.z = std::clamp(transform_.translate.z, -59.0f, 59.0f);
				stunTime = kMaxHitFenceStunTime;
				dodgeActiveTime = kMaxDodgeActiveTime;
				AudioHolder::GetInstance()->GetAudio(AudioIndex::Fence_Collision_SE).lock()->SoundPlayWave();
			}

			emitter_.transform.translate = transform_.translate;
			emitter_.transform.translate.y = 0.0f;
			particle_2->SetEmitter(emitter_);
			particle_2->Emit();

			//回避中じゃない
		} else {
			//回避インターバル
			if (dodgeCoolTime < kMaxDodgeCoolTime) {
				dodgeCoolTime += 1.0f / 60.0f;
			}
		}

		//回避インターバル
		if (dodgeCoolTime >= kMaxDodgeCoolTime) {
			if ((keys.trigger[DIK_C] || keys.trigger[DIK_SPACE] || pad.Button[PAD_BUTTON_B].trigger) && isMove) {
				dodgeActiveTime = 0.0f;
				dodgeCoolTime = 0.0f;
				//注視中は他の方にも回避できる
				if (isTargeted_) {
					if (fabsf(move_.x) > fabsf(move_.z)) {
						if (move_.x > 0.0f) {
							dodgeAngle_ = DODGE_ANGLE::RIGHT;
						} else {
							dodgeAngle_ = DODGE_ANGLE::LEFT;
						}
					} else {
						if (move_.z > 0.0f) {
							dodgeAngle_ = DODGE_ANGLE::FRONT;
						} else {
							dodgeAngle_ = DODGE_ANGLE::BACK;
						}
					}
				} else {
					dodgeAngle_ = DODGE_ANGLE::FRONT;
				}
				AudioHolder::GetInstance()->GetAudio(AudioIndex::Dodge_SE).lock()->SoundPlayWave();
			}
		}

#pragma endregion

#pragma region 攻撃行動

		if (Length(velocity_) <= 0.0f) {
			stopTime_ += 1.0f / 60.0f;
			if (stopTime_ > kMaxStopTime_) {
				stopTime_ = kMaxStopTime_;
			}
		} else {
			stopTime_ = 0.0f;
		}

		if (shotCooltime_ < kMaxShotCooltime) {
			shotCooltime_ += 1.0f / 60.0f;
		}
		if (keys.hold[DIK_Z] || keys.hold[DIK_X] || pad.Button[PAD_BUTTON_RT].hold && dodgeActiveTime >= kMaxDodgeActiveTime) {
			if (remainingRounds_ > 0.0f) {
				if (shotCooltime_ >= kMaxShotCooltime * Lerp<float>(1.0f, 0.2f, stopTime_ / kMaxStopTime_)) {
					gameScene_->AddPlayerBullet(transform_.translate, transform_.rotate);
					shotCooltime_ = 0.0f;
					remainingRounds_--;
					gameCamera_->SetShakeTime(0.1f);
					velocity_ = {};
				}
			}
		}

#pragma endregion

#pragma region 注目行動

		if (keys.hold[DIK_LSHIFT] || keys.hold[DIK_RSHIFT] || pad.Button[PAD_BUTTON_LT].hold) {
			isTargeted_ = true;
		} else {
			isTargeted_ = false;
		}

#pragma endregion

#pragma region リロード行動

		//動いていない場合
		if (shotCooltime_ != 0.0f && dodgeCoolTime != 0.0f) {
			if (reloadTime_ >= kMaxReloadTime * Lerp<float>(1.0f, 0.1f, stopTime_ / kMaxStopTime_) && remainingRounds_ != kMaxRemainingRounds) {
				remainingRounds_ = kMaxRemainingRounds;
				reloadTime_ = 0.0f;
				AudioHolder::GetInstance()->GetAudio(AudioIndex::Reload_SE).lock()->SoundPlayWave();
			} else {
				reloadTime_ += 1.0f / 60.0f;
			}
		} else {
			reloadTime_ = 0.0f;
		}

#pragma endregion
	}

#ifdef USE_IMGUI
	ImGui::Begin("プレイヤー");
	ImGui::Text("%f", transform_.rotate.y / std::numbers::pi_v<float> * 180.0f);
	ImGui::DragFloat3("translate", &transform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.1f);
	ImGui::DragFloat3("velocity", &velocity_.x);
	ImGui::InputInt("remainingRounds", &remainingRounds_);
	ImGui::End();
#endif

	*targetTransform_ = transform_;

	object_->SetTransform(transform_);
	UpdateCollider();
	particle_2->Update();
}

void Player::Draw() {
	particle_2->Draw();
	if (HP_ > 0.0f) {
		if (invincibleTime_ > 0.0f) {
			object_->SetColor(Vector4{ 0.3f,0.3f,0.3f,1.0f });
		} else {
			object_->SetColor(Vector4{ 1.0f,1.0f,1.0f,1.0f });
		}
		object_->Draw3D();
	}
}

void Player::IsCollision() {
	if (invincibleTime_ <= 0.0f) {
		HP_--;
		gameCamera_->SetShakeTime(0.3f);
		invincibleTime_ = kMaxInvincibleTime_;
		SetInvincible(true);
		AudioHolder::GetInstance()->GetAudio(AudioIndex::Player_Damage_SE).lock()->SoundPlayWave();
	}
}