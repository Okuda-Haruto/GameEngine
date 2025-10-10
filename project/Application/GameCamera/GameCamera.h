#pragma once
#include <Resource/Camera.h>
#include <memory>

class GameCamera
{
private:
	//カメラ
	std::unique_ptr<Camera> camera_;

	std::unique_ptr<SRT> transform_;

	const SRT* target_ = nullptr;
public:
public:
	//初期化
	void Initialize();
	//更新
	void Update();

	//Transformのゲッター
	SRT* GetTransform() { return transform_.get(); }

	//追従対象を指定
	void SetTarget(const SRT* target) { target_ = target; }

	Camera* GetCamera() { return camera_.get(); }

};