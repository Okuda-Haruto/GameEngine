#include "Player.h"
#include "GameEngine.h"
#include <Vector3.h>
#include <Matrix4x4.h>
#include <numbers>
#include "Math/Lerp.h"
#include "Scene/GameScene/GameScene.h"

Player::~Player() {

}

void Player::Initialize(ModelHolder* modelHolder, GameScene* gameScene, ParticleEmitter* particle1) {
	modelHolder_ = modelHolder;
	gameScene_ = gameScene;
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
	object_->Initialize(modelHolder_->GetModel(ModelIndex::Player));
	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.rotate = { 0.0f,0.0f,0.0f };
	transform_.translate = { 0.0f,1.0f,-20.0f };
	object_->SetTransform(transform_);

	targetTransform_ = std::make_unique<SRT>();
	*targetTransform_ = transform_;
	dodgeCoolTime = 0.0f;
	dodgeActiveTime = kMaxDodgeActiveTime;
	shotCooltime = 0.0f;

	HP_ = kMaxHP;

	InitializeCollider(1.0f, CollisionID_Player_Character);
	UpdateCollider();
}

void Player::Update() {
	Pad pad = GameEngine::GetPad();
	Keybord keys = GameEngine::GetKeybord();
	const float deadZone = 0.7f;
	bool isMove = false;

	//速さ
	const float speed = 0.3f;

	Vector3 move{};

	//移動量
	if (pad.isConnected) {
		move = {
			pad.LeftStick.vector.x * pad.LeftStick.magnitude,0.0f,pad.LeftStick.vector.y * pad.LeftStick.magnitude
		};
	}
	if (Length(move) > deadZone) {
		isMove = true;
	}

	if (!isMove) {
		if (keys.hold[DIK_W] || keys.hold[DIK_UP]) {
			move.z = 1.0f;
		}
		if (keys.hold[DIK_S] || keys.hold[DIK_DOWN]) {
			move.z = -1.0f;
		}
		if (keys.hold[DIK_D] || keys.hold[DIK_RIGHT]) {
			move.x = 1.0f;
		}
		if (keys.hold[DIK_A] || keys.hold[DIK_LEFT]) {
			move.x = -1.0f;
		}
		if (Length(move) > deadZone) {
			isMove = true;
		}
	}

#pragma region 移動処理

	//回避中は移動させないこと
	if (isMove && dodgeActiveTime >= kMaxDodgeActiveTime) {
		//移動量に速さを反映
		velocity_ = Normalize(move) * speed;

		Matrix4x4 rotateMatrix = MakeRotateYMatrix(cameraTransform_->rotate.y);
		velocity_ = rotateMatrix * velocity_;

		//移動
		transform_.translate += velocity_;

		angle = std::atan2(velocity_.x, velocity_.z);

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
	transform_.rotate.y = (transform_.rotate.y + diff * 0.1f);
	transform_.rotate.y = std::fmodf(transform_.rotate.y, std::numbers::pi_v<float> *2);

#pragma endregion

#pragma region 回避行動

	const float dodgeSpeed = 2.0f;

	//回避中
	if (dodgeActiveTime < kMaxDodgeActiveTime) {
		dodgeActiveTime += 1.0f / 60.0f;
		//kMaxDodgeActiveTimeを超えると角度がおかしくなる気がするので戻す
		if (dodgeActiveTime >= kMaxDodgeActiveTime) {
			//dodgeActiveTime = kMaxDodgeActiveTime;
		}
		velocity_ = Vector3{ 0.0f,0.0f,1.0f } *Lerp(0.2f, dodgeSpeed, powf(1.0f - dodgeActiveTime / kMaxDodgeActiveTime, 3));
		//前転
		transform_.rotate.x = Lerp(0.0f, std::numbers::pi_v<float> *2, 1.0f - powf(1.0f - dodgeActiveTime / kMaxDodgeActiveTime, 3));

		Matrix4x4 rotateMatrix = MakeRotateYMatrix(transform_.rotate.y);
		velocity_ = rotateMatrix * velocity_;

		//移動
		transform_.translate += velocity_;

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
		if ((keys.trigger[DIK_C] || keys.trigger[DIK_LSHIFT] || keys.trigger[DIK_RSHIFT] || keys.trigger[DIK_SPACE] || pad.Button[PAD_BUTTON_Y].trigger) && isMove) {
			dodgeActiveTime = 0.0f;
			dodgeCoolTime = 0.0f;
		}
	}

#pragma endregion

#pragma region 攻撃行動

	if (shotCooltime >= kMaxShotCooltime){
		if (keys.hold[DIK_Z] || keys.hold[DIK_X]) {
			gameScene_->AddPlayerBullet(transform_.translate, transform_.rotate);
			shotCooltime = 0.0f;
		}
	} else {
		shotCooltime += 1.0f / 60.0f;
	}

#pragma endregion

#ifdef USE_IMGUI
	ImGui::Begin("プレイヤー");
	ImGui::Text("%f", transform_.rotate.y / std::numbers::pi_v<float> * 180.0f);
	ImGui::DragFloat3("translate", &transform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.1f);
	ImGui::DragFloat3("velocity", &velocity_.x);
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
		object_->Draw3D();
	}
}

void Player::IsCollision() {
	HP_--;
}