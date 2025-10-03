#include "Effect.h"
#include <list>
#include <Vector3_operation.h>
#include <numbers>
#include <Collision.h>

Effect::Effect() {

}

Effect::~Effect() {
	delete object_particle_;
}

void Effect::Initialize() {
	object_particle_ = new Particle_3D;
	object_particle_->Initialize();

	particles_.texture = GameEngine::TextureLoad("resources/particle.png");
	type_ = EffectType::random_diffusion;
	emitter_.transform.scale = {2.0f,2.0f,2.0f};
	emitter_.transform.rotate = {};
	emitter_.transform.translate = {};
	emitter_.velocity = {3.0f,0.0f,0.0f};
	emitter_.count = 6;
	emitter_.beforeColor = { 1.0f,1.0f,1.0f,1.0f };
	emitter_.afterColor = { 1.0f,1.0f,1.0f,1.0f };
	emitter_.lifeTime = 1.0f;
	emitter_.frequency = 0.5f;
	emitter_.frequencyTime = 0.0f;

	accelerationField_.acceleration = { 15.0f,0.0f,0.0f };
	accelerationField_.area.min = { -1.0f,-1.0f,-1.0f };
	accelerationField_.area.max = { 1.0f,1.0f,1.0f };
}

void Effect::Update() {
	const float kDeltaTime = 1.0f / 60.0f;
	for (std::list<Particle>::iterator particleIterator = particles_.particle.begin();
		particleIterator != particles_.particle.end();) {
		if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
			particleIterator = particles_.particle.erase(particleIterator);
			continue;
		}

		if (isUseField_ && IsCollision(accelerationField_.area, (*particleIterator).transform.translate)) {
			(*particleIterator).velocity += accelerationField_.acceleration * kDeltaTime;
		}

		(*particleIterator).transform.translate += (*particleIterator).velocity * kDeltaTime;
		(*particleIterator).currentTime += kDeltaTime;
		(*particleIterator).color.x = emitter_.beforeColor.x * ((*particleIterator).lifeTime - (*particleIterator).currentTime / (*particleIterator).lifeTime) + emitter_.afterColor.x * ((*particleIterator).currentTime / (*particleIterator).lifeTime);
		(*particleIterator).color.y = emitter_.beforeColor.y * ((*particleIterator).lifeTime - (*particleIterator).currentTime / (*particleIterator).lifeTime) + emitter_.afterColor.y * ((*particleIterator).currentTime / (*particleIterator).lifeTime);
		(*particleIterator).color.z = emitter_.beforeColor.z * ((*particleIterator).lifeTime - (*particleIterator).currentTime / (*particleIterator).lifeTime) + emitter_.afterColor.z * ((*particleIterator).currentTime / (*particleIterator).lifeTime);
		(*particleIterator).color.w = emitter_.beforeColor.w * ((*particleIterator).lifeTime - (*particleIterator).currentTime / (*particleIterator).lifeTime) + emitter_.afterColor.w * ((*particleIterator).currentTime / (*particleIterator).lifeTime);

		++particleIterator;
	}

	if (emitter_.frequency != 0.0f) {
		emitter_.frequencyTime += kDeltaTime;
		if (emitter_.frequency <= emitter_.frequencyTime) {
			Emit();
			emitter_.frequencyTime -= emitter_.frequency;
		}
	}
}

void Effect::Draw() {
	object_particle_->Draw(GameEngine::GetCommandList(), particles_);
}

Particle Effect::MakeNewParticle(Emitter emitter) {
	Particle particle;
	particle.transform = emitter.transform;
	particle.velocity = emitter.velocity;
	particle.beforeColor = emitter.beforeColor;
	particle.afterColor = emitter.afterColor;
	particle.color = emitter.beforeColor;
	particle.lifeTime = emitter.lifeTime;
	particle.currentTime = 0;
	return particle;
}

void Effect::Emit() {
	std::list<Particle> particle;
	for (uint32_t count = 0; count < emitter_.count; count++) {
		if (type_ == EffectType::random_diffusion) {
			Emitter emitter;
			emitter = emitter_;
			emitter.velocity = { GameEngine::randomFloat(-1.0f,1.0f),GameEngine::randomFloat(-1.0f,1.0f),GameEngine::randomFloat(-1.0f,1.0f) };
			emitter.velocity = Normalize(emitter.velocity);
			emitter.lifeTime = GameEngine::randomFloat(1.0f, 3.0f);

			particle.push_back(MakeNewParticle(emitter));
		} else if (type_ == EffectType::constant_convergence) {
			Emitter emitter;
			emitter = emitter_;
			Vector3 convergenceDistance;
			convergenceDistance.x = Length(emitter_.velocity) * emitter.lifeTime * emitter.transform.scale.x * cosf((std::numbers::pi_v<float> * 2 * count / emitter_.count));
			convergenceDistance.y = Length(emitter_.velocity) * emitter.lifeTime * emitter.transform.scale.x * sinf((std::numbers::pi_v<float> * 2 * count / emitter_.count));
			convergenceDistance.z = 0.0f;

			emitter.velocity = -convergenceDistance / emitter.lifeTime;
			emitter.transform.translate = emitter_.transform.translate + convergenceDistance;
			particle.push_back(MakeNewParticle(emitter));
		} else if (type_ == EffectType::constant_diffusion) {
			Emitter emitter;
			emitter = emitter_;
			Vector3 convergenceDistance;
			convergenceDistance.x = Length(emitter_.velocity) * emitter.lifeTime * emitter.transform.scale.x * cosf((std::numbers::pi_v<float> *2 * count / emitter_.count));
			convergenceDistance.y = Length(emitter_.velocity) * emitter.lifeTime * emitter.transform.scale.x * sinf((std::numbers::pi_v<float> *2 * count / emitter_.count));
			convergenceDistance.z = 0.0f;

			emitter.velocity = convergenceDistance / emitter.lifeTime;
			particle.push_back(MakeNewParticle(emitter));
		} else {
			particle.push_back(MakeNewParticle(emitter_));
		}
	}

	//Emitで得たエフェクトを追加
	particles_.particle.splice(particles_.particle.end(), particle);
}
