#pragma once
#include <memory>
#include "../BaseCharacter.h"
#include "Shape/Sphere.h"
#include "GameCamera/GameCamera.h"
#include "ParticleEmitter/ParticleEmitter.h"

class Stage;
class BreakObject;

#pragma region BreakBehavior

class BaseBreakBehavior {
protected:
	Stage* stage_;
	BreakObject* breakObject_;
public:
	void Initialize(Stage* stage, BreakObject* breakObject) { stage_ = stage; breakObject_ = breakObject; }
	virtual void Behavior() = 0;
};

class BreakBehavior_Explosion_Small : public BaseBreakBehavior {
public:
	void Behavior() override;
};

class BreakBehavior_DropItem : public BaseBreakBehavior {
public:
	void Behavior() override;
};

#pragma endregion



class BreakObject : public BaseCharacter
{
private:
	static std::weak_ptr<GameCamera> gameCamera_;
	static std::weak_ptr<DirectionalLight> directionalLight_;
	static std::weak_ptr<PointLight> pointLight_;
	static Stage* stage_;

	//ターゲット用Sphere
	std::shared_ptr<Sphere> trackingSphere_;

	//体力
	float maxHP_;
	float HP_;

public:

	//初期化
	void Initialize(std::string directoryPath, std::string fileName, SRT startTransform, float maxHP);
	//更新
	void Update();
	//描画
	void Draw();

	void IsCollision(uint8_t targetId) override;

	void IsCollisionGround(OBB obb) override;

	SRT GetTransform() { return transform_; }
	void SetTransfrom(SRT transfrom) { transform_ = transfrom; object_->SetTransform(transform_); }

	std::shared_ptr<Sphere> GetTrackingSphere() { return trackingSphere_; }

	void SetCamera(shared_ptr<Camera> camera) { object_->SetCamera(camera); }

	bool IsDead() { return HP_ <= 0; }
	float GetHP() { return HP_; }

	static void SetGameCamera(std::weak_ptr<GameCamera> gameCamera) { gameCamera_ = gameCamera; }
	static void SetDirectionalLight(std::weak_ptr<DirectionalLight> directionalLight) { directionalLight_ = directionalLight; }
	static void SetPointLight(std::weak_ptr<PointLight> pointLight) { pointLight_ = pointLight; }
	static void SetStage(Stage* stage) { stage_ = stage; }
};

