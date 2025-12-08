#pragma once
#include <memory>
#include <Object/Object.h>
#include <Camera.h>
#include <ModelHolder/ModelHolder.h>

class Fence
{
private:
	//地面モデル
	std::list<Object*> objects_;
	const int size = 160;
	SRT transform_{};
	DirectionalLight* directionalLight_ = nullptr;
	PointLight* pointLight_ = nullptr;
public:
	~Fence();
	//初期化
	void Initialize(ModelHolder* modelHolder, Camera* camera, DirectionalLight* directionalLight, PointLight* pointLight);
	//描画
	void Draw();
};
