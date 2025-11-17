#pragma once
#include <memory>
#include "../BaseCharacter.h"

class Player : public BaseCharacter
{
private:
	float angle;

	std::unique_ptr<SRT> targetTransform_;

	const SRT* cameraTransform_{};

	const float kMaxDodgeCoolTime = 0.2f;
	float dodgeCoolTime = 0.0f;
	const float kMaxDodgeActiveTime = 0.4f;
	float dodgeActiveTime = 0.0f;

public:
	//初期化
	void Initialize(ModelHolder* modelHolder);
	//更新
	void Update();
	//描画
	void Draw();

	SRT* GetTransform() { return targetTransform_.get(); }

	void SetCameraTransform(const SRT* transform) { cameraTransform_ = transform; }

	void SetCamera(Camera* camera) { object_->SetCamera(camera); }

	void SetDirectionalLight(DirectionalLight* directionalLight) { object_->SetDirectionalLight(directionalLight); }
};

