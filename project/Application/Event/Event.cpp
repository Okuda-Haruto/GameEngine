#include "Event.h"
#include <StageManager/Stage/Stage.h>

void Event_Explosion::Initialize(Stage* stage, float range, float maxLifeTime, CollisionID id, float damage) {
	stage_ = stage;
	range_ = range;
	maxLifeTime_ = maxLifeTime;
	id_ = id;
	damage_ = damage;
}

void Event_Explosion::Play(SRT transform) {
	stage_->Explosion(transform.translate, range_, maxLifeTime_, id_, damage_);
}

void Event_AddBreakObject::Initialize(Stage* stage, float maxHP, std::shared_ptr<Model> model, std::unique_ptr<BaseEvent> deadEvent) {
	stage_ = stage;
	maxHP_ = maxHP;
	model_ = model;
	deadEvent_ = move(deadEvent);
}

void Event_AddBreakObject::Play(SRT transform) {
	stage_->AddBreakObject(transform, maxHP_, model_, move(deadEvent_));
}

void Event_AddItem::Initialize(Stage* stage) {
	stage_ = stage;
}

void Event_AddItem::Play(SRT transform) {
	stage_->AddItem(transform);
}

void Event_AddBomb::Initialize(Stage* stage, float range, float maxLifeTime, CollisionID id, std::shared_ptr<Model> model) {
	stage_ = stage;
	range_ = range;
	maxLifeTime_ = maxLifeTime;
	id_ = id;
	model_ = model;
}

void Event_AddBomb::Play(SRT transform) {
	stage_->AddBomb(transform, range_, maxLifeTime_, id_, model_);
}

void Event_AddShockWave::Initialize(Stage* stage, float range, float maxLifeTime, CollisionID id, std::unique_ptr<PrimitiveCylinder> cylinder) {
	stage_ = stage;
	range_ = range;
	maxLifeTime_ = maxLifeTime;
	id_ = id;
	cylinder_ = move(cylinder);
}

void Event_AddShockWave::Play(SRT transform) {
	stage_->AddShockWave(transform, range_, maxLifeTime_, id_,move(cylinder_));
}
