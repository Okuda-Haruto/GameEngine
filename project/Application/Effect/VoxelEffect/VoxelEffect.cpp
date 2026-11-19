#include "VoxelEffect.h"
#include "GameEngine.h"
#include <numbers>
#include "Collision.h"
#include "Easing.h"

const float VoxelEffect::kMaxCoolTime = 1.0f;
const float VoxelEffect::kMaxFadeOutTime = 0.5f;
const float VoxelEffect::kDampingCoefficient = 0.8f;
const float VoxelEffect::kGravitationalAcceleration = -0.1f;
const int VoxelEffect::kMaxNameIndex = 1024;
const float VoxelEffect::kMaxHitEffectScale = 5.0f;
const int VoxelEffect::kHitDamage = 3 ;

VoxelEffect::~VoxelEffect() {

}

void VoxelEffect::Initialize(std::weak_ptr<Model> model, std::weak_ptr<Camera> camera, std::weak_ptr<DirectionalLight> directionalLight, std::weak_ptr<PointLight> pointLight) {
	objects_ = std::make_unique<InstancingObject>();
	objects_->Initialize(model);
	objects_->SetCamera(camera);
	objects_->SetDirectionalLight(directionalLight);
	objects_->SetPointLight(pointLight);
}

void VoxelEffect::Update() {

	for (auto& state : effectStates_) {

		//着地していたらクールタイム減少
		if (state.isLanding) {
			state.coolTime -= 1.0f / 60.0f;
			if (state.coolTime <= 0) {
				state.coolTime = 0;
			}
		}

		//クールタイムが0ならフェードアウト
		if (state.coolTime <= 0.0f) {
			state.fadeOutTime -= 1.0f / 60.0f;
			if (state.fadeOutTime <= 0) {
				state.fadeOutTime = 0;
				state.isDead = true;
				continue;
			}
			//Objectに反映
			objects_->SetColor(state.name, Vector4(1.0f,1.0f,1.0f, state.fadeOutTime / kMaxFadeOutTime));
		} else if (state.isHit) {
			state.fadeOutTime -= 1.0f / 60.0f;
			if (state.fadeOutTime <= 0) {
				state.fadeOutTime = 0;
				state.isDead = true;
				continue;
			}
			//Objectに反映
			objects_->SetColor(state.name, Vector4(1.0f, 1.0f, 1.0f, state.fadeOutTime / kMaxFadeOutTime));

			state.transform.scale = Vector3{ state.scale,state.scale,state.scale } * Easing::EaseOut(1.0f, kMaxHitEffectScale, 1.0f - state.fadeOutTime / kMaxFadeOutTime);
			objects_->SetTransform(state.name, state.transform);
			continue;
		}

		//重力加速度を加算
		state.velocity.y += kGravitationalAcceleration;
		//x,z速度減衰
		if (state.isLanding) {
			if (std::fabsf(state.velocity.x) > 0.0f) {
				state.velocity.x *= kDampingCoefficient;
			}
			if (std::fabsf(state.velocity.z) > 0.0f) {
				state.velocity.z *= kDampingCoefficient;
			}
		}

		//速度加算
		state.transform.translate += state.velocity;

		if (state.transform.translate.y <= state.transform.scale.y) {
			state.transform.translate.y = state.transform.scale.y;
			state.isLanding = true;
		} else {
			state.isLanding = false;
		}

		//Objectに反映
		objects_->SetTransform(state.name, state.transform);
	}

	for (auto it = effectStates_.begin(); it != effectStates_.end();) {
		if (it->isDead) {
			objects_->DeleteObject(it->name);
			it = effectStates_.erase(it);
		} else {
			++it;
		}
	}

	objects_->Update();
}

void VoxelEffect::Draw() {
	objects_->Draw3D();
}

void VoxelEffect::AddNewEffect(SRT transform, Vector3 velocity) {
	std::string str;
	str = "Effect_" + std::to_string(effectStates_.size());

	objects_->AddNewObject(str);

	EffectState state;
	state.name = str;
	state.transform = transform;
	state.velocity = velocity;
	state.coolTime = kMaxCoolTime;
	state.fadeOutTime = kMaxFadeOutTime;
	state.isLanding = false;
	state.isDead = false;
	state.isHit = false;
	state.scale = transform.scale.x;

	effectStates_.push_back(state);
}

void VoxelEffect::Collision(const Colliders* collider) {
	std::vector<SphereCollider> sphereCollider = collider->GetSphereColliders();
	for (int i = 0; i < sphereCollider.size(); i++) {
		for (auto& state : effectStates_) {
			if (state.isHit)continue;

			Sphere sphere;
			sphere.center = state.transform.translate;
			sphere.radius = state.transform.scale.x;

			if (IsCollision(sphereCollider[i].colliderSphere, sphere)) {
				for (int j = 0; j < kHitDamage; j++) {
					collider->IsCollision(CollisionID_Player_Attack);
				}
				state.isHit = true;
			}
		}
	}
}