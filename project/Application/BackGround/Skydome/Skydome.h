#pragma once
#include <memory>
#include <Object/Object.h>
#include <Camera/Camera.h>
#include <ModelHolder/ModelHolder.h>

using namespace std;

class Skydome
{
private:
	//天球モデル
	std::unique_ptr<Object> object_;

public:
	//初期化
	void Initialize();
	//描画
	void Draw();

	void SetCamera(shared_ptr<Camera> camera) { object_->SetCamera(camera); }
};
