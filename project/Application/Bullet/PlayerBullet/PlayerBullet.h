#pragma once
#include "../BaseBullet.h"

class BasePlayerBulletType {
public:
	virtual void Initialize();
};

class PlayerBullet : public BaseBullet {
private:
	BasePlayerBulletType* bulletType_;
public:
	//初期化
	void Initialize(ModelHolder* modelHolder, Vector3 translate, Vector3 rotate, BasePlayerBulletType* bulletType);
	//更新処理
	void Update();
	//描画処理
	void Draw();

	void ChangeBulletType(BasePlayerBulletType* nextBulletType);
};