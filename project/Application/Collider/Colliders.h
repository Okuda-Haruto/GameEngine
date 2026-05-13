#pragma once
#include "SphereCollider.h"
#include "OBBCollider.h"

class Colliders {
private:
	//攻撃接触判定
	SphereCollider attackCollider;

	//身体接触判定
	SphereCollider bodyCollider;

	//地面接触判定
	OBBCollider grundCollider;
public:
	//初期化
	void Initialize();
	//更新処理
	void Update();
};