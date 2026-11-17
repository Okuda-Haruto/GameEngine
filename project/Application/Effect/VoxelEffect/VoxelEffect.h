#pragma once
#include <memory>
#include "InstancingObject/InstancingObject.h"
#include "Camera/Camera.h"
#include "ModelHolder/ModelHolder.h"
#include "Collider/Colliders.h"

class VoxelEffect
{
private:
	
	//消え始めるまでの時間
	static const float kMaxCoolTime;
	//消えるまでの時間
	static const float kMaxFadeOutTime;
	//減衰係数
	static const float kDampingCoefficient;
	//重力加速度
	static const float kGravitationalAcceleration;
	//名前の最大値
	static const int kMaxNameIndex;
	//接触時の最終エフェクトサイズ
	static const float kMaxHitEffectScale;
	//エフェクト接触時のダメージ
	static const int kHitDamage;

	struct EffectState {
		std::string name;
		SRT transform;
		Vector3 velocity;
		float coolTime;
		float fadeOutTime;
		bool isLanding;
		bool isDead;
		bool isHit;
		float scale;
	};
	std::list<EffectState> effectStates_;

	std::unique_ptr<InstancingObject> objects_;

	int nameIndex_;
public:
	~VoxelEffect();
	//初期化
	void Initialize(std::weak_ptr<Model> model, std::weak_ptr<Camera> camera, std::weak_ptr<DirectionalLight> directionalLight, std::weak_ptr<PointLight> pointLight);
	//更新処理
	void Update();
	//描画
	void Draw();

	//エフェクト追加
	void AddNewEffect(SRT transform, Vector3 velocity);

	void Collision(const Colliders* collider);
};
