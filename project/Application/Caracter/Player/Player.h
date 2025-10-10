#pragma once
#include <memory>
#include <Resource/Object_3D.h>

class Player
{
private:
	//自キャラモデル
	std::unique_ptr<Object_3D> model_;
	Object_Multi_Data data_;

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

	void SetCamera(Camera* camera) { model_->SetCamera(camera); }
};

