#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(std::string name) {
	name_ = name;
}

void ParticleEmitter::Update() {

	ParticleManager::GetInstance()->Update(name_);

	std::unordered_map<std::string, ParticleGroup> particleGroups = ParticleManager::GetInstance()->GetParticleGroups();
	if (particleGroups[name_].emitter.frequency > 0.0f) {
		if (particleGroups[name_].emitter.frequencyTime >= particleGroups[name_].emitter.frequency) {
			ParticleManager::GetInstance()->Emit(
				name_,
				particleGroups[name_].emitter.count
			);
			particleGroups[name_].emitter.frequencyTime -= particleGroups[name_].emitter.frequency;
		}

		particleGroups[name_].emitter.frequencyTime += 1.0f / 60.0f;
	}

	ParticleManager::GetInstance()->SetEmitter(name_, particleGroups[name_].emitter);
}

void ParticleEmitter::Emit() {
	std::unordered_map<std::string, ParticleGroup> particleGroups = ParticleManager::GetInstance()->GetParticleGroups();
	ParticleManager::GetInstance()->Emit(
		name_,
		particleGroups[name_].emitter.count
	);
}

void ParticleEmitter::Draw() {
	ParticleManager::GetInstance()->Draw(name_);
}

void ParticleEmitter::SetEmitter(Emitter emitter) {

	std::unordered_map<std::string, ParticleGroup> particleGroups = ParticleManager::GetInstance()->GetParticleGroups();

	emitter.frequencyTime = particleGroups[name_].emitter.frequencyTime;

	ParticleManager::GetInstance()->SetEmitter(name_, emitter);
}

void ParticleEmitter::SetField(AccelerationField accelerationField){

	ParticleManager::GetInstance()->SetField(name_, accelerationField);

}