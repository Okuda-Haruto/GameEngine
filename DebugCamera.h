#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Camera.h"

#include "Input.h"

//デバッグカメラ
class DebugCamera
{
private:
	//ローカル座標
	Vector3 translation_ = { 0,0,-50 };
	//累積回転行列
	Matrix4x4 matRot_;
	//ビュー行列
	Matrix4x4 viewMatrix_;
	//射影行列
	Matrix4x4 projectionMatrix_;
	//マウス入力
	Mouse mouse_;
public:
	//初期化
	void Initialize();

	//更新
	void Update(Mouse mouse);

	//Transform
	Camera GetTransform() { return { viewMatrix_ ,projectionMatrix_ }; };
};

