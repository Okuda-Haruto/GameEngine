#pragma once
#include <memory>
#include "Object/Object.h"
#include "Camera/Camera.h"
#include "ModelHolder/ModelHolder.h"

class Ground
{
private:
	//地面モデル
	std::unique_ptr<Object> object_;

public:
	//初期化
	void Initialize();
	//描画
	void Draw();

	void SetCamera(std::weak_ptr<Camera> camera) { object_->SetCamera(camera); }

	void SetDirectionalLight(std::weak_ptr<DirectionalLight> directionalLight) { object_->SetDirectionalLight(directionalLight); }
	void SetPointLight(std::weak_ptr<PointLight> pointLight) { object_->SetPointLight(pointLight); }
};
