#pragma once
#include "Object/Object.h"

class Player;

class Cylinder {
private:
	//装弾数
	std::unique_ptr<Object> object_;
	SRT transform_;

	Player* player_;
public:

	void Initialize(std::shared_ptr<DirectionalLight> directionalLight, Player* player);

	void Update();

	void Draw();
};