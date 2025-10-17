#pragma once
#include <memory>
#include <Resource/Object/Object.h>
#include <Camera.h>

class Player
{
private:
	//自キャラモデル
	std::unique_ptr<Object> object_;
	SRT objectTransform;

	Camera* camera_;

	float angle;

	std::unique_ptr<SRT> targetTransform_;

	const SRT* cameraTransform_{};

public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();

	SRT* GetTransform() { return targetTransform_.get(); }

	void SetCameraTransform(const SRT* transform) { cameraTransform_ = transform; }

	void SetCamera(Camera* camera) { camera_ = camera; }
};

