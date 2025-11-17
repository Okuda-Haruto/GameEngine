#pragma once
#include <memory>
#include <Object/Object.h>
#include <ModelHolder/ModelHolder.h>
#include <Sphere.h>

class BaseCharacter
{
protected:
	//自キャラモデル
	std::unique_ptr<Object> object_;
	SRT transform_;
	Vector3 velocity_;

	Sphere collision_;

	//モデル
	ModelHolder* modelHolder_;

public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();

	Sphere GetCollision() { return collision_; }
	void IsCollision() {}
};

