#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(std::string name) {
	name_ = name;
}

void ParticleEmitter::Update() {

	ParticleManager::GetInstance()->Update(name_);

	std::unordered_map<std::string, ParticleGroup> particleGroups = ParticleManager::GetInstance()->GetParticleGroups();

	if (particleGroups[name_].emitter.frequencyTime >= particleGroups[name_].emitter.frequency) {
		ParticleManager::GetInstance()->Emit(
			name_,
			{ 0.5f,0.5f,0.5f },
			particleGroups[name_].emitter.count
		);
		particleGroups[name_].emitter.frequencyTime -= particleGroups[name_].emitter.frequency;
	}

	particleGroups[name_].emitter.frequencyTime += 1.0f / 60.0f;

	ParticleManager::GetInstance()->SetEmitter(name_, particleGroups[name_].emitter);
}

void ParticleEmitter::Emit() {
	std::unordered_map<std::string, ParticleGroup> particleGroups = ParticleManager::GetInstance()->GetParticleGroups();
	for (std::unordered_map<std::string, ParticleGroup>::iterator groupIterator = particleGroups.begin();
		groupIterator != particleGroups.end(); ++groupIterator) {

		ParticleManager::GetInstance()->Emit(
			(*groupIterator).first,
			{0.5f,0.5f,0.5f},
			(*groupIterator).second.emitter.count
		);
	}
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