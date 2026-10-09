#pragma once
#include <memory>
#include <Object/Object.h>
#include <Camera/Camera.h>
#include <ModelHolder/ModelHolder.h>

class Fence
{
private:
	//地面モデル
	std::list<unique_ptr<Object>> objects_;
	const int size = 160;
	SRT transform_{};
	weak_ptr<DirectionalLight> directionalLight_;
	weak_ptr<PointLight> pointLight_;
public:
	~Fence();
	//初期化
	void Initialize(weak_ptr<Camera> camera, weak_ptr<DirectionalLight> directionalLight, weak_ptr<PointLight> pointLight);
	//描画
	void Draw();
};
