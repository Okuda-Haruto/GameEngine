#pragma once
#include <memory>
#include "../BaseCharacter.h"

class BossAction {
protected:
	SRT startTransform;

public:
	virtual void Initialize();
	virtual void Update();
	virtual void Finalize();
};

class Boss : public BaseCharacter
{
private:

	float angle;

	std::unique_ptr<SRT> targetTransform_;

	float maxHP_;
	float HP_;

public:

	~Boss();

	//初期化
	void Initialize(ModelHolder* modelHolder, float maxHP);
	//更新
	void Update();
	//描画
	void Draw();

	void IsCollision();

	SRT* GetTransform() { return targetTransform_.get(); }

	void SetCamera(Camera* camera) { object_->SetCamera(camera); }

	void SetDirectionalLight(DirectionalLight* directionalLight) { object_->SetDirectionalLight(directionalLight); }
	void SetPointLight(PointLight* pointLight) { object_->SetPointLight(pointLight); }

	bool IsDead() { return HP_ <= 0; }
};

