#include "BreakObject.h"
#include <StageManager/Stage/Stage.h>
#include <ModelManager/ModelManager.h>
#include <Operation/Operation.h>
#include <PrimitiveManager/Primitive3DManager.h>

std::weak_ptr<GameCamera> BreakObject::gameCamera_;
std::weak_ptr<DirectionalLight> BreakObject::directionalLight_;
std::weak_ptr<PointLight> BreakObject::pointLight_;
Stage* BreakObject:: stage_;

void BreakBehavior_Explosion_Small::Behavior() {
	if (Length(stage_->GetBoss()->GetTransform().translate - breakObject_->GetTransform().translate) < 13) {
		stage_->GetBoss()->Damage(15);
		breakObject_->GetGameCamera().lock()->SetShakeTime(0.2f);
	}
}

void BreakBehavior_DropItem::Behavior() {

}

void BreakObject::Initialize(std::string directoryPath, std::string fileName, SRT startTransform, float maxHP, std::unique_ptr<BaseBreakBehavior> breakBehavior) {
	
	maxHP_ = maxHP;
	HP_ = maxHP_;

	//モデルの生成
	object_ = std::make_unique<Object>();
	object_->Initialize(ModelManager::GetInstance()->GetModel(directoryPath, fileName));
	transform_ = startTransform;
	object_->SetTransform(transform_);
	object_->SetCamera(gameCamera_.lock()->GetCamera());
	object_->SetDirectionalLight(directionalLight_.lock());
	object_->SetPointLight(pointLight_.lock());

	trackingSphere_ = std::make_shared<Sphere>();
	trackingSphere_->center = transform_.translate;
	trackingSphere_->radius = Length(transform_.scale);
	gameCamera_.lock()->SetTargetSphere(trackingSphere_);

	BaseCharacter::Initialize(trackingSphere_->radius, CollisionID_Item_Body);

	OBB obb;
	obb.center = { 0,0,0 };
	obb.orientations[0] = { 1,0,0 };
	obb.orientations[1] = { 0,1,0 };
	obb.orientations[2] = { 0,0,1 };
	obb.size = transform_.scale;

	colliders_->AddOBBCollider(obb, CollisionID_Anything_Body, CollisionID_Anything_Body, colliderParent_);
	colliders_->Update();

	breakBehavior_ = move(breakBehavior);
	breakBehavior_->Initialize(stage_, this);
}

void BreakObject::Update() {

	trackingSphere_->center = transform_.translate;
	trackingSphere_->radius = Length(transform_.scale);
#ifdef USE_IMGUI
	Primitive3DManager::GetInstance()->AddSphere(*trackingSphere_);
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


		if (IsDead()) {
			breakBehavior_->Behavior();
		}
	}
}

void BreakObject::IsCollisionGround(OBB obb) {

}