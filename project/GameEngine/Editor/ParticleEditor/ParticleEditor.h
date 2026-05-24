#pragma once
#include <ParticleManager/ParticleManager.h>
#include <ParticleEmitter/ParticleEmitter.h>

class ParticleEditor {
private:

	ParticleEmitter* particleEmitter_;
	Emitter emitter_;
	AccelerationField field_;

	SRT transform_;
public:
	void Initialize(ParticleEmitter* particleEmitter);

	void Update();

	void Draw();

	Emitter GetEmitter() { return emitter_; }
	void SetEmitter(Emitter emitter) { emitter_ = emitter; }
	AccelerationField GetField() { return field_; }
	void SetField(AccelerationField field) { field_ = field; }
	SRT GetTransform() { return transform_; }
	void SetTransform(SRT transform) { transform_ = transform; }
};