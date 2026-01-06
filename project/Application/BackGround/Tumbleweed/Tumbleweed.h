#pragma once
#include <memory>
#include <Object/Object.h>
#include <Camera/Camera.h>
#include <ModelHolder/ModelHolder.h>
#include "ParticleEmitter/ParticleEmitter.h"

using namespace std;

class Tumbleweed
{
private:
	//タンブルウィードモデル
	std::vector<unique_ptr<Object>> objects_;
	const int size = 3;
	shared_ptr<DirectionalLight> directionalLight_;
	shared_ptr<PointLight> pointLight_;

	ParticleEmitter* particle_ = nullptr;
	Emitter emitter_;

	const float kMaxMoveTime = 60.0f;
	float moveTime_ = 0.0f;
public:
	~Tumbleweed();
	//初期化
	void Initialize(shared_ptr<Camera> camera, shared_ptr<DirectionalLight> directionalLight, shared_ptr<PointLight> pointLight, ParticleEmitter* particle);
	//更新処理
	void Update();
	//描画
	void Draw();
};
