#pragma once
#include <memory>
#include <Object/Object.h>
#include <Camera.h>
#include <ModelHolder/ModelHolder.h>

class Ground
{
private:
	//地面モデル
	std::unique_ptr<Object> object_;

public:
	//初期化
	void Initialize(ModelHolder* modelHolder);
	//描画
	void Draw();

	void SetCamera(Camera* camera) { object_->SetCamera(camera); }

	void SetDirectionalLight(DirectionalLight* directionalLight) { object_->SetDirectionalLight(directionalLight); }
	void SetPointLight(PointLight* pointLight) { object_->SetPointLight(pointLight); }
};
