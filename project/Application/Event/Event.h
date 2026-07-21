#pragma once
#include <SRT.h>
#include <Model/Model.h>
#include <PrimitiveManager/PrimitiveCylinder.h>
#include <Collider/CollisionID.h>

class Stage;

class BaseEvent {
protected:

public:
	virtual void Play(SRT transform) = 0;
};

class Event_Explosion : public BaseEvent {
private:
	Stage* stage_;
	float range_;
	float maxLifeTime_;
	CollisionID id_;
	float damage_;
public:
	void Initialize(Stage* stage, float range, float maxLifeTime, CollisionID id, float damage);
	void Play(SRT transform) override;
};

class Event_AddBreakObject : public  BaseEvent {
private:
	Stage* stage_;
	float maxHP_;
	std::shared_ptr<Model> model_;
	std::unique_ptr<BaseEvent> deadEvent_;
public:
	void Initialize(Stage* stage, float maxHP, std::shared_ptr<Model> model, std::unique_ptr<BaseEvent> deadEvent);
	void Play(SRT transform) override;
};

class Event_AddItem : public  BaseEvent {
private:
	Stage* stage_;
public:
	void Initialize(Stage* stage);
	void Play(SRT transform) override;
};

class Event_AddBomb : public  BaseEvent {
private:
	Stage* stage_;
	float range_;
	float maxLifeTime_;
	CollisionID id_;
	std::shared_ptr<Model> model_;
public:
	void Initialize(Stage* stage, float range, float maxLifeTime, CollisionID id, std::shared_ptr<Model> model);
	void Play(SRT transform) override;
};

class Event_AddShockWave : public BaseEvent {
private:
	Stage* stage_;
	float range_;
	float maxLifeTime_;
	CollisionID id_;
	std::unique_ptr<PrimitiveCylinder> cylinder_;
public:
	void Initialize(Stage* stage, float range, float maxLifeTime, CollisionID id, std::unique_ptr<PrimitiveCylinder> cylinder);
	void Play(SRT transform) override;
};