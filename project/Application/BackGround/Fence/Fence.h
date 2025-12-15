#pragma once
#include <memory>
#include <Object/Object.h>
#include <Camera/Camera.h>
#include <ModelHolder/ModelHolder.h>

using namespace std;

class Fence
{
private:
	//地面モデル
	std::list<unique_ptr<Object>> objects_;
	const int size = 160;
	SRT transform_{};
	shared_ptr<DirectionalLight> directionalLight_;
	shared_ptr<PointLight> pointLight_;
public:
	~Fence();
	//初期化
	void Initialize(shared_ptr<Camera> camera, shared_ptr<DirectionalLight> directionalLight, shared_ptr<PointLight> pointLight);
	//描画
	void Draw();
};
