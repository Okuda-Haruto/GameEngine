#pragma once
#include <memory>
#include <Object/Object.h>
#include <Camera.h>
#include <ModelHolder/ModelHolder.h>

class Fence
{
private:
	//地面モデル
	std::unique_ptr<Object> object_;
	SRT transform_{};
public:
	//初期化
	void Initialize(ModelHolder* modelHolder);
	//描画
	void Draw();

	void SetCamera(Camera* camera) { object_->SetCamera(camera); }
	void SetTransform(SRT transform) { transform_ = transform; object_->SetTransform(transform_); }

	void SetDirectionalLight(DirectionalLight* directionalLight) { object_->SetDirectionalLight(directionalLight); }
	void SetPointLight(PointLight* pointLight) { object_->SetPointLight(pointLight); }
};
