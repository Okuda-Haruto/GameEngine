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
	//注目地点
	Vector3 centerPoint_{0.0f,0.0f,0.0f};
	//球面座標系
	Vector3 sphericalCoordinates_{ 10.0f,0.0f,0.0f };
	//ローカル座標
	Vector3 translation_ = { 0,0,-10 };
	//Windowのサイズ
	uint32_t kWindowWidth_ = 1280;
	uint32_t kWindowHeight_ = 720;
	//回転行列
	Matrix4x4 rotateMatrix_;
	//ビュー行列
	Matrix4x4 viewMatrix_;
	//射影行列
	Matrix4x4 projectionMatrix_;
public:

	DebugCamera(GameEngine* gameEngine);

	//初期化
	void Initialize();

	//更新
	void Update(Mouse mouse, Keybord keybord);

	//初期化
	void Reset();

	//出力
	void UpdateCamera(Camera* camera);

	Vector3 GetAttentionPoint() { return centerPoint_; }
};

