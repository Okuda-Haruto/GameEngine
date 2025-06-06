#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Camera.h"
#include <cstdint>

#include "Input.h"

//デバッグカメラ
class DebugCamera
{
private:
	//ローカル座標
	Vector3 translation_ = { 0,0,-10 };
	//Windowのサイズ
	uint32_t kWindowWidth_ = 1280;
	uint32_t kWindowHeight_ = 720;
	//累積回転行列
	Matrix4x4 matRot_;
	//ビュー行列
	Matrix4x4 viewMatrix_;
	//射影行列
	Matrix4x4 projectionMatrix_;
	//マウス入力
	Mouse mouse_;
public:

	DebugCamera(GameEngine* gameEngine);

	//初期化
	void Initialize();

	//更新
	void Update(Mouse mouse);

	//初期化
	void Reset();

	//出力
	void UpdateCamera(Camera* camera);
};

