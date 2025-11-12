#pragma once
#include <memory>
#include <Object/Object.h>
#include <Sphere.h>
#include <Camera.h>
#include <ModelHolder/ModelHolder.h>

class BaseBullet {
protected:
	std::unique_ptr<Object> object_;
	SRT transform_;
	SRT velocity_;
	Sphere collision_;

	//モデル
	ModelHolder* modelHolder_;
public:
	void Initialize();
	void Update();
	void Draw();

	Sphere GetCollision() { return collision_; }
	void IsCollision() {}
};