#pragma once
#include <Camera.h>
#include <memory>

class GameCamera
{
private:
	//カメラ
	std::unique_ptr<Camera> camera_;

	std::unique_ptr<SRT> transform_;

	const SRT* target_ = nullptr;

	//追従対象からカメラ位置までのオフセット
	Vector3 offset_;
public:
	//初期化
	void Initialize();
	//更新
	void Update();

	//Transformのゲッター
	SRT* GetTransform() { return transform_.get(); }

	//
	void SetOffset(Vector3 offset) { offset_ = offset; }

	//追従対象を指定
	void SetTarget(const SRT* target) { target_ = target; }

	Camera* GetCamera() { return camera_.get(); }

};