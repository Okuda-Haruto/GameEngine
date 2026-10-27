#pragma once
#include <memory>
#include "Object/Object.h"
#include "Camera/Camera.h"
#include "ModelHolder/ModelHolder.h"

class Fence
{
private:
	//地面モデル
	std::list<std::unique_ptr<Object>> objects_;
	const int size = 160;
	SRT transform_{};
	std::weak_ptr<DirectionalLight> directionalLight_;
	std::weak_ptr<PointLight> pointLight_;
public:
	~Fence();
	//初期化
	void Initialize(std::weak_ptr<Camera> camera, std::weak_ptr<DirectionalLight> directionalLight, std::weak_ptr<PointLight> pointLight);
	//描画
	void Draw();
};
