#pragma once
#include "ParticleManager/ParticleManager.h"

class ParticleEmitter {
public:
	ParticleEmitter(std::string name);

	void Update();
	void Emit();

	void Draw();

	void SetEmitter(Emitter emitter);
	void SetField(AccelerationField accelerationField);

private:
	std::string name_;
};