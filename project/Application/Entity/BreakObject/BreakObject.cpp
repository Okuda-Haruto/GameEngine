#include "BreakObject.h"
#include <StageManager/Stage/Stage.h>
#include <ModelManager/ModelManager.h>
#include <Operation/Operation.h>
#include <PrimitiveManager/PrimitiveManager.h>

std::weak_ptr<GameCamera> BreakObject::gameCamera_;
std::weak_ptr<DirectionalLight> BreakObject::directionalLight_;
std::weak_ptr<PointLight> BreakObject::pointLight_;
Stage* BreakObject:: stage_;

void BreakObject::Initialize(SRT transform, float maxHP, std::shared_ptr<Model> model, std::unique_ptr<BaseEvent> deadEvent) {
	
	maxHP_ = maxHP;
	HP_ = maxHP_;
	deadEvent_ = move(deadEvent);

	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(model);
	transform_ = transform;
	object_->SetTransform(transform_);
	object_->SetCamera(gameCamera_.lock()->GetCamera());
	object_->SetDirectionalLight(directionalLight_.lock());
	object_->SetPointLight(pointLight_.lock());

	trackingSphere_ = std::make_shared<Sphere>();
	trackingSphere_->center = transform_.translate;
	trackingSphere_->radius = Length(transform_.scale);
	gameCamera_.lock()->SetTargetSphere(trackingSphere_);

	BaseEntity::Initialize(trackingSphere_->radius, CollisionID_Item_Body);

	OBB obb;
	obb.center = { 0,0,0 };
	obb.orientations[0] = { 1,0,0 };
	obb.orientations[1] = { 0,1,0 };
	obb.orientations[2] = { 0,0,1 };
	obb.size = transform_.scale;

	colliders_->AddOBBCollider(obb, CollisionID_Anything_Body, CollisionID_Anything_Body, colliderParent_);
	colliders_->Update();
}

void BreakObject::Update() {

	trackingSphere_->center = transform_.translate;
	trackingSphere_->radius = Length(transform_.scale);
#ifdef USE_IMGUI
	PrimitiveManager::GetInstance()->AddSphere(*trackingSphere_);
#endif // USE_IMGUI
}

void BreakObject::Draw() {
	object_->Draw3D();
}

void BreakObject::IsCollision(uint8_t targetId) {
	if (targetId == CollisionID_Player_Attack) {	//プレイヤー攻撃
		HP_--;

		//particle_->Emit();

		gameCamera_.lock()->SetShakeTime(0.05f);


		if (IsDead() && deadEvent_) {
			deadEvent_->Play(transform_);
		}
	}
}

void BreakObject::IsCollisionGround(OBB obb) {

}