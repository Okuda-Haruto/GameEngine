#pragma once
#include <memory>
#include <Object/Object.h>
#include <Camera.h>

class Skydome
{
private:
	//天球モデル
	std::unique_ptr<Object> object_;

	Camera* camera_;

public:
	//初期化
	void Initialize();
	//描画
	void Draw();

	void SetCamera(Camera* camera) { camera_ = camera; }
};
