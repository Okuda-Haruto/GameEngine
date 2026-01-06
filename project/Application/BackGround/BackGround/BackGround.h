#pragma once
#include <memory>
#include <Object/Object.h>
#include <Camera/Camera.h>
#include <ModelHolder/ModelHolder.h>

using namespace std;

class BackGround
{
private:
	//背景モデル
	std::unique_ptr<Object> object_;
	uint32_t index_;

public:
	//初期化
	void Initialize(uint32_t index);
	//描画
	void Draw();

	void SetIndex(uint32_t index) { index_ = index; }
	void SetTransform(SRT transform) { object_->SetTransform(transform); }

	void SetCamera(shared_ptr<Camera> camera) { object_->SetCamera(camera); }

	void SetDirectionalLight(shared_ptr<DirectionalLight> directionalLight) { object_->SetDirectionalLight(directionalLight); }
	void SetPointLight(shared_ptr<PointLight> pointLight) { object_->SetPointLight(pointLight); }

	uint32_t GetIndex() { return index_; }
	SRT GetTransform() { return object_->GetTransform(); }
};
