#pragma once
#include "ParticleManager/ParticleManager.h"
#include <SRT.h>

class ParticleEmitter {
public:
	ParticleEmitter(std::string name);

	void Update();
	void Emit();
	void Emit(SRT transform);

	void Draw();

	void SetEmitter(Emitter emitter);
	void SetField(AccelerationField accelerationField);
	void SetTransform(SRT transform) { transform_ = transform; }

	std::string GetName() { return name_; }

private:
	//名前
	std::string name_;
	//Transofrm
	SRT transform_;
};