#pragma once
#include "GameEngine.h"
#include "Particle.h"
#include "Emitter.h"
#include <Field.h>

/*enum class EffectType {
	constant,				//一定の動きをする
	random_diffusion,		//ランダムで拡散する
	constant_convergence,	//一定の動きで収束する
	constant_diffusion,	//一定の動きで拡散する
};

class Effect {
private:
	Particle_3D* object_particle_ = nullptr;
	Particles particles_;
	Emitter emitter_;
	EffectType type_ = EffectType::random_diffusion;

	bool isUseField_ = true;
	AccelerationField accelerationField_;
public:

	Effect();
	~Effect();

	void Initialize();

	void Update();

	void Draw();

	Particle MakeNewParticle(Emitter emitter);

	void Emit();

	void SetEmitter(Emitter emitter) { emitter_ = emitter; }
	void SetEffectType(EffectType type) { type_ = type; }
	void SetTranslate(Vector3 translate) { emitter_.transform.translate = translate; }
	void SetTransform(SRT transform) { emitter_.transform = transform; }
	void SetCamera(Camera* camera) { object_particle_->SetCamera(camera); }
	void SetBeforeColor(Vector4 color) { emitter_.beforeColor = color; }
	void SetAfterColor(Vector4 color) { emitter_.afterColor = color; }
	void SetColor(Vector4 color) { emitter_.beforeColor = color; emitter_.afterColor = color; }
	void IsUseField(bool isUseField) { isUseField_ = isUseField; }
};*/