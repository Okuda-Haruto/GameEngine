#pragma once
#include "../BaseScene.h"
#include <BoxFilterData.h>
#include <PrimitiveManager/PrimitiveRing.h>
#include <DissolveData.h>
#include <RadialBlurData.h>

#include <StageManager/Stage/Stage.h>

#include "Sprite/Sprite.h"
#include "Editor/ParticleEditor/ParticleEditor.h"
#include "AudioHolder/AudioHolder.h"
#include <array>
#include <fstream>

using namespace std;

class GameScene : public BaseScene
{
private:

	shared_ptr<Input> input_;

	std::unique_ptr<Stage> stage_;

	std::unique_ptr<Sprite> fadeSprite_;

	std::unique_ptr<ParticleEmitter> particle_;
	std::unique_ptr<ParticleEditor> editor_;
	std::unique_ptr<ParticleEmitter> particle_2;
	std::unique_ptr<ParticleEditor> editor_2;
	std::unique_ptr<ParticleEmitter> particle_3;
	std::unique_ptr<ParticleEditor> editor_3;
	std::unique_ptr<ParticleEmitter> particle_4;
	std::unique_ptr<ParticleEditor> editor_4;

	std::unique_ptr<PrimitiveRing> ring_;
	SRT ringTransform_;
	Material ringMaterial_;

	DissolveData dissolveData_;
	BoxFilterData boxFilterData_;

public:

	~GameScene();

	//初期化
	void Initialize(shared_ptr<Input> input) override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	
	void SetRingColorA(float a) { ringMaterial_.color.w = a; }
};

