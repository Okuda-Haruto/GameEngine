#pragma once
#include <memory>
#include "Object/Object.h"
#include "Camera/Camera.h"
#include "ModelHolder/ModelHolder.h"
#include "ParticleEmitter/ParticleEmitter.h"

class Tumbleweed
{
private:
	//タンブルウィードモデル
	std::vector<std::unique_ptr<Object>> objects_;
	const int size = 3;
	std::weak_ptr<DirectionalLight> directionalLight_;
	std::weak_ptr<PointLight> pointLight_;

	const float kMaxMoveTime = 60.0f;
	float moveTime_ = 0.0f;

	const Vector3 kBasePosition = { 0.0f,2.0f,128.0f };
public:
	~Tumbleweed();
	//初期化
	void Initialize(std::weak_ptr<Camera> camera, std::weak_ptr<DirectionalLight> directionalLight, std::weak_ptr<PointLight> pointLight);
	//更新処理
	void Update();
	//描画
	void Draw();
};
