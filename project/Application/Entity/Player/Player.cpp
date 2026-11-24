#include "Player.h"
#include "GameEngine.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include <numbers>
#include "Easing.h"
#include "StageManager/Stage/Stage.h"
#include "AudioHolder/AudioHolder.h"
#include "Input/Input.h"

void PlayerNormalState::Initialize(Player* player, std::weak_ptr<Input> input) {
	player_ = player;
	input_ = input;
	reloadTime_ = 0;
	shotCooltime_ = 0;
	stopTime_ = 0;
}

void PlayerNormalState::Update() {
	Pad pad = input_.lock()->GetPad();
	Keyboard keyboard = input_.lock()->GetKeyboard();

	Vector3 movementInput{};

	//移動量
	if (pad.isConnected) {
		movementInput = {
			pad.LeftStick.vector.x * pad.LeftStick.magnitude,0.0f,pad.LeftStick.vector.y * pad.LeftStick.magnitude
		};
	}
	if (Length(movementInput) > deadZone) {
		player_->Move(movementInput);
	} else {
		if (keyboard.keys[DIK_W].hold || keyboard.keys[DIK_UP].hold) {
			movementInput.z = 1.0f;
		}
		if (keyboard.keys[DIK_S].hold || keyboard.keys[DIK_DOWN].hold) {
			movementInput.z = -1.0f;
		}
		if (keyboard.keys[DIK_D].hold || keyboard.keys[DIK_RIGHT].hold) {
			movementInput.x = 1.0f;
		}
		if (keyboard.keys[DIK_A].hold || keyboard.keys[DIK_LEFT].hold) {
			movementInput.x = -1.0f;
		}
		if (Length(movementInput) > deadZone) {
			player_->Move(movementInput);
		}
	}

	//回避
	if (keyboard.keys[DIK_C].trigger || keyboard.keys[DIK_SPACE].trigger || pad.Button[PAD_BUTTON_B].trigger) {
		player_->SetNextState(std::unique_ptr<PlayerDodgeState>());
	}

	if (keyboard.keys[DIK_LSHIFT].hold || keyboard.keys[DIK_RSHIFT].hold || pad.Button[PAD_BUTTON_LT].hold) {
		player_->SetIsTargeted(true);
	} else {
		player_->SetIsTargeted(false);
	}

	//発射間隔
	if (shotCooltime_ <= kMaxShotCooltime) {
		shotCooltime_ += 1.0f / 60.0f;
	}

	//攻撃
	if (keyboard.keys[DIK_Z].hold || keyboard.keys[DIK_X].hold || pad.Button[PAD_BUTTON_RT].hold) {
		player_->Shot();
		shotCooltime_ = 0.0f;
	}

	//動いていない場合
	if (shotCooltime_ != 0.0f && dodgeCoolTime_ != 0.0f) {
		if (reloadTime_ >= kMaxReloadTime * Easing::Lerp<float>(1.0f, 0.1f, stopTime_ / kMaxStopTime) && player_->IsRemainingRoundsFull()) {
			player_->Reload();
			reloadTime_ = 0.0f;
		} else {
			reloadTime_ += 1.0f / 60.0f;
		}
	} else {
		reloadTime_ = 0.0f;
	}
	
	if (Length(movementInput) <= 0.0f) {
		stopTime_ += 1.0f / 60.0f;
		if (stopTime_ > kMaxStopTime) {
			stopTime_ = kMaxStopTime;
		}
	} else {
		stopTime_ = 0.0f;
	}

	if (shotCooltime_ < kMaxShotCooltime) {
		shotCooltime_ += 1.0f / 60.0f;
	}
}

void PlayerDodgeState::Initialize(Player* player, std::weak_ptr<Input> input) {
	player_ = player;
	input_ = input;
	dodgeActiveTime_ = 0.0f;

	if (player->IsTargeted()) {
		Vector3 movementInput = player->GetMovementInput();
		if (Length(movementInput) > 0.0f) {
			if (fabsf(movementInput.x) > fabsf(movementInput.z)) {
				if (movementInput.x > 0.0f) {
					dodgeAngle_ = DODGE_ANGLE::RIGHT;
				} else {
					dodgeAngle_ = DODGE_ANGLE::LEFT;
				}
			} else {
				if (movementInput.z > 0.0f) {
					dodgeAngle_ = DODGE_ANGLE::FRONT;
				} else {
					dodgeAngle_ = DODGE_ANGLE::BACK;
				}
			}
		} else {
			//動いていない場合
			dodgeAngle_ = DODGE_ANGLE::FRONT;
		}
	} else {
		//注目視点ではない場合
		dodgeAngle_ = DODGE_ANGLE::FRONT;
	}
}

void PlayerDodgeState::Update() {

	dodgeActiveTime_ += 1.0f / 60.0f;

	if (dodgeActiveTime_ >= kMaxDodgeActiveTime) {
		dodgeActiveTime_ = kMaxDodgeActiveTime;
		player_->SetNextState(std::make_unique<PlayerNormalState>());
	}

	float easingRate = powf(1.0f - dodgeActiveTime_ / kMaxDodgeActiveTime, 3);

	player_->Dodge(dodgeAngle_, easingRate);
}

void PlayerStunState::Initialize(Player* player, std::weak_ptr<Input> input) {

}

void PlayerStunState::Update() {
	stunTime_ += 1.0f / 60.0f;

	if (stunTime_ >= kMaxStunTime) {
		stunTime_ = kMaxStunTime;
		player_->SetNextState(std::make_unique<PlayerNormalState>());
	}

	SRT transform = *player_->GetTransform();

	if (stunTime_ > 0.0f) {
		transform.rotate.x = std::numbers::pi_v<float> / 2;
		transform.rotate.z = 0.0f;
		transform.translate.y = 0.5f;
	} else {
		transform.rotate.x = 0.0f;
		transform.rotate.z = 0.0f;
		transform.translate.y = 1.0f;
	}

	player_->SetTransfrom(transform);
}

Player::~Player() {

}

void Player::Initialize(Stage* stage, std::shared_ptr<GameCamera> gameCamera, std::shared_ptr<Input> input, SRT startTransform) {
	stage_ = stage;
	gameCamera_ = gameCamera;
	input_ = input;
  
	//パーティクル
	damageParticle_ = std::make_unique<ParticleEmitter>("Particle_PlayerDamage");
	moveParticle_ = std::make_unique<ParticleEmitter>("Particle_Move");

	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Player));
	transform_ = startTransform;
	object_->SetTransform(transform_);

	trackingTransform_ = std::make_unique<SRT>();
	*trackingTransform_ = transform_;

	velocity_ = {};

	HP_ = kMaxHP;
	remainingRounds_ = kMaxRemainingRounds;
	invincibleTime_ = 0.0f;

	isTargeted_ = false;

	state_ = std::make_unique<PlayerNormalState>();
	state_->Initialize(this, input_);

	BaseEntity::Initialize(1.5f, CollisionID_Player_Body);
}

void Player::Update() {
	Pad pad = input_->GetPad(0);
	Keyboard keyboard = input_->GetKeyboard();

	if (invincibleTime_ > 0.0f) {
		invincibleTime_ -= 1.0f / 60.0f;
		if (invincibleTime_ <= 0.0f) {
			invincibleTime_ = 0.0f;
		}
	}

	//入力で変動する値の初期化
	movementInput_ = {};
	acceleration_ = {};

	//行動ステート
	state_->Update();
	if (nextState_) {
		state_ = move(nextState_);
		state_->Initialize(this, input_);
	}


#ifdef USE_IMGUI
	ImGuiState();
#endif

	if (isTargeted_) {
		//ロックオン中
		facingAngle_ = gameCamera_->GetTransform()->rotate.y;
	} else if (Length(velocity_) > 0.0f) {
		//俯瞰視点
		facingAngle_ = std::atan2(velocity_.x, velocity_.z);
	}
	//最短角度補完
	float diff = facingAngle_ - transform_.rotate.y;

	if (diff >= std::numbers::pi_v<float>*2) {
		diff = facingAngle_ - transform_.rotate.y;
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

	//加速度を加算
	velocity_ += acceleration_;

	//加速度がかかっていないなら減速
	if (Length(velocity_) > 0.0f) {
		if (Length(acceleration_) <= 0.0f) {
			velocity_ = velocity_ * 0.8f;
			if (Length(velocity_) < 0.01f) {
				velocity_ = {};
			}
		}
	}

	//速度制限
	if (Length(velocity_) > 0.0f) {

		if (Length(velocity_) > kMaxSpeed) {
			velocity_ = Normalize(velocity_) * kMaxSpeed;
		}

		if (isTargeted_ && Length(velocity_) > kMaxSpeed / 2) {
			velocity_ = Normalize(velocity_) * kMaxSpeed / 2;
		}
	}

	//オブジェクトに沿って移動
	velocity_ = stage_->MoveWithCollision(colliders_->GetSphereColliders()[0], velocity_);

	transform_.translate = colliders_->GetSphereColliders()[0].colliderSphere.center;
	transform_.translate += velocity_;

	if ((transform_.translate.x > 58.5f || transform_.translate.x < -58.5f ||
		transform_.translate.z > 58.5f || transform_.translate.z < -58.5f) && 
		dynamic_cast<PlayerDodgeState*>(state_.get())) {
		Stun();
	}

	transform_.translate.x = std::clamp(transform_.translate.x, -58.5f, 58.5f);
	transform_.translate.z = std::clamp(transform_.translate.z, -58.5f, 58.5f);

	*trackingTransform_ = transform_;

	object_->SetTransform(transform_);
	BaseEntity::Update();

	if (Length(velocity_) > 0.0f) {
		SRT	transform;
		transform.translate = transform_.translate;
		transform.translate.y = 0.0f;
		moveParticle_->Emit(transform);
	}
}

void Player::Draw() {
	//particle_2->Draw();
	if (HP_ > 0.0f) {
		if (invincibleTime_ > 0.0f) {
			object_->SetColor(Vector4{ 0.3f,0.3f,0.3f,1.0f });
		} else {
			object_->SetColor(Vector4{ 1.0f,1.0f,1.0f,1.0f });
		}
		object_->Draw3D();
	}
}

void Player::IsCollision(uint8_t targetId) {
	if (targetId & CollisionID_Enemy_Attack) {	//敵攻撃
		if (invincibleTime_ <= 0.0f) {
			HP_--;
			gameCamera_->SetShakeTime(0.3f);
			invincibleTime_ = kMaxInvincibleTime_;
			
			damageParticle_->Emit(transform_);

			AudioHolder::GetInstance()->GetAudio(AudioIndex::Player_Damage_SE).lock()->SoundPlayWave();
		}
	} else if(targetId & CollisionID_Anything_Body){
		if (dynamic_cast<PlayerDodgeState*>(state_.get())) {
			Stun();
		}
	}
}

void Player::Move(Vector3 movementInput) {

	movementInput_ = movementInput;

	//移動量に速さを反映
	acceleration_ = Normalize(movementInput_) * speed;

	Matrix4x4 rotateMatrix = MakeRotateYMatrix(cameraTransform_->rotate.y);
	acceleration_ = rotateMatrix * acceleration_;

	//ロックオン中は加速度を下げる
	if (isTargeted_) {
		acceleration_ /= 4.0f;
	}
}

void Player::Dodge(PlayerDodgeState::DODGE_ANGLE angle, float easingRate) {
	switch (angle)
	{
	case PlayerDodgeState::DODGE_ANGLE::FRONT:
		//前転
		velocity_ = Vector3{ 0.0f,0.0f,1.0f } * Easing::Lerp(0.2f, dodgeSpeed, easingRate) * MakeRotateYMatrix(transform_.rotate.y);
		transform_.rotate.x = Easing::Lerp(0.0f, std::numbers::pi_v<float> *2, 1.0f - easingRate);
		break;
	case PlayerDodgeState::DODGE_ANGLE::BACK:
		//後転
		velocity_ = Vector3{ 0.0f,0.0f,-1.0f } * Easing::Lerp(0.2f, dodgeSpeed, easingRate) * MakeRotateYMatrix(transform_.rotate.y);
		transform_.rotate.x = -Easing::Lerp(0.0f, std::numbers::pi_v<float> *2, 1.0f - easingRate);
		break;
	case PlayerDodgeState::DODGE_ANGLE::RIGHT:
		//右側転
		velocity_ = Vector3{ 1.0f,0.0f,0.0f } * Easing::Lerp(0.2f, dodgeSpeed, easingRate) * MakeRotateYMatrix(transform_.rotate.y);
		transform_.rotate.z = -Easing::Lerp(0.0f, std::numbers::pi_v<float> *2, 1.0f - easingRate);
		break;
	case PlayerDodgeState::DODGE_ANGLE::LEFT:
		//左側転
		velocity_ = Vector3{ -1.0f,0.0f,0.0f } * Easing::Lerp(0.2f, dodgeSpeed, easingRate) * MakeRotateYMatrix(transform_.rotate.y);
		transform_.rotate.z = Easing::Lerp(0.0f, std::numbers::pi_v<float> *2, 1.0f - easingRate);
		break;
	default:
		break;
	}

	//回避ブラー
	gameCamera_->SetBlurWidth(0.01f * (easingRate));
}

void Player::Shot() {
	stage_->AddBullet(transform_, 2.0f, CollisionID_Player_Attack, Bullet::BulletMove::NormalBullet, ModelManager::GetInstance()->GetModel("resources/Bullet", "Bullet.obj"));
	--remainingRounds_;
	gameCamera_->SetShakeTime(0.1f);
	velocity_ = {};
}

void Player::Reload() {
	remainingRounds_ = kMaxRemainingRounds;
	AudioHolder::GetInstance()->GetAudio(AudioIndex::Reload_SE).lock()->SoundPlayWave();
}

void Player::Stun() {
	SetNextState(std::make_unique<PlayerStunState>());
	AudioHolder::GetInstance()->GetAudio(AudioIndex::Fence_Collision_SE).lock()->SoundPlayWave();
}

void Player::SetNextState(std::unique_ptr<BasePlayerState> state) {
	nextState_ = move(state);
}

#ifdef USE_IMGUI
void Player::ImGuiState() {
	ImGui::Begin("プレイヤー");
	ImGui::Text("%f", transform_.rotate.y / std::numbers::pi_v<float> *180.0f);
	ImGui::DragFloat3("translate", &transform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.1f);
	ImGui::DragFloat3("velocity", &velocity_.x);
	ImGui::InputInt("remainingRounds", &remainingRounds_);
	ImGui::End();
}
#endif