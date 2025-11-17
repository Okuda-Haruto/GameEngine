#pragma once
#include <memory>
#include <Object/Object.h>
#include <Camera.h>
#include <ModelHolder/ModelHolder.h>

class Skydome
{
private:
	//天球モデル
	std::unique_ptr<Object> object_;

public:
	//初期化
	void Initialize(ModelHolder* modelHolder);
	//描画
	void Draw();

	void SetCamera(Camera* camera) { object_->SetCamera(camera); }
};
