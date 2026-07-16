#include "ParticleEmitter.h"
#include <GameEngine.h>

ParticleEmitter::ParticleEmitter(std::string name) {
	name_ = name;
}

void ParticleEmitter::Update() {

	ParticleManager::GetInstance()->Update(name_);

	std::unordered_map<std::string, ParticleGroup> particleGroups = ParticleManager::GetInstance()->GetParticleGroups();
	particleGroups[name_].emitterSphereResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroups[name_].emitterSphere));

	if (particleGroups[name_].emitterSphere->frequency > 0.0f) {
		if (particleGroups[name_].emitterSphere->frequencyTime >= particleGroups[name_].emitterSphere->frequency) {

			particleGroups[name_].emitterSphere->frequencyTime -= particleGroups[name_].emitterSphere->frequency;
			particleGroups[name_].emitterSphere->emit = 1;

		} else {
			particleGroups[name_].emitterSphere->emit = 0;
		}

		particleGroups[name_].emitterSphere->frequencyTime += 1.0f / 60.0f;
	}

	particleGroups[name_].emitterSphereResource->Unmap(0, nullptr);

	ParticleManager::GetInstance()->SetEmitter(name_, *particleGroups[name_].emitterSphere);
}

void ParticleEmitter::Emit() {
	std::unordered_map<std::string, ParticleGroup> particleGroups = ParticleManager::GetInstance()->GetParticleGroups();
	ParticleManager::GetInstance()->Emit(
		name_,
		transform_,
		particleGroups[name_].emitterSphere->count
	);
}


void ParticleEmitter::Emit(SRT transform) {
	std::unordered_map<std::string, ParticleGroup> particleGroups = ParticleManager::GetInstance()->GetParticleGroups();
	ParticleManager::GetInstance()->Emit(
		name_,
		transform,
		particleGroups[name_].emitterSphere->count
	);
}

void ParticleEmitter::Draw() {
	ParticleManager::GetInstance()->Draw(name_);
}

void ParticleEmitter::Draw_AddBlend() {
	ParticleManager::GetInstance()->Draw_AddBlend(name_);
}

void ParticleEmitter::SetEmitter(EmitterSphere emitterSphere) {

	std::unordered_map<std::string, ParticleGroup> particleGroups = ParticleManager::GetInstance()->GetParticleGroups();

	*particleGroups[name_].emitterSphere = emitterSphere;

	ParticleManager::GetInstance()->SetEmitter(name_, emitterSphere);
}

void ParticleEmitter::SetField(AccelerationField accelerationField){

	ParticleManager::GetInstance()->SetField(name_, accelerationField);

}