#include "Tumbleweed.h"
#include "GameEngine.h"
#include <numbers>

Tumbleweed::~Tumbleweed() {

}

void Tumbleweed::Initialize(shared_ptr<Camera> camera, shared_ptr<DirectionalLight> directionalLight, shared_ptr<PointLight> pointLight, ParticleEmitter* particle) {
	directionalLight_ = directionalLight;
	pointLight_ = pointLight;
	particle_ = particle;
	emitter_.count = 1;
	emitter_.lifeTime = 0.2f;
	emitter_.frequency = 0.0f;
	emitter_.frequencyTime = 0.0f;
	emitter_.transform.scale = { 5.0f,5.0f,5.0f };
	emitter_.transform.translate = { 0.0f,0.0f,0.0f };
	emitter_.spawnRange.min = { 0.0f,0.0f,0.0f };
	emitter_.spawnRange.max = { 0.0f,0.0f,0.0f };
	emitter_.angleBase = { 0.0f,0.0f,1.0f };
	emitter_.angleRange = { 0.2f,0.2f,0.2f };	//方向範囲
	emitter_.speedBase = 0.2f;	//基礎速度
	emitter_.speedRange = 0.1f;	//速度範囲
	emitter_.beforeColor = { 1.0f,1.0f,1.0f,1.0f };
	emitter_.afterColor = { 1.0f,1.0f,1.0f,0.0f };

	SRT transform{};
	transform.scale = { 1.0f,1.0f,1.0f };
	//モデルの生成
	for (int i = 0; i < size; i++) {

		unique_ptr<Object> object = make_unique<Object>();
		object->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Tumbleweed));
		object->SetTransform(transform);
		object->SetCamera(camera);
		object->SetDirectionalLight(directionalLight_);
		object->SetShininess(0);

		objects_.push_back(move(object));
	}
}

void Tumbleweed::Update() {
	moveTime_ += 1.0f / 60.0f;
	if (moveTime_ >= kMaxMoveTime) {
		moveTime_ -= kMaxMoveTime;
	}

	for (int i = 0; i < objects_.size();i++) {
		SRT Transform = { {1,1,1},{0,0,0},{0,0,0} };
		Transform.rotate.z = -std::numbers::pi_v<float> *2 * moveTime_;
		Transform.rotate.y = std::numbers::pi_v<float> * 2 * (moveTime_ / kMaxMoveTime + float(i) / 3);

		Matrix4x4 rotateMatrix = MakeRotateYMatrix(Transform.rotate.y);

		Transform.translate = Vector3{ 0,2,128 } * rotateMatrix;

		objects_[i]->SetTransform(Transform);

		emitter_.transform.translate = Transform.translate;
		emitter_.transform.translate.y = 0.0f;
		particle_->SetEmitter(emitter_);
		particle_->Emit();
	}

}

void Tumbleweed::Draw() {
	for (const unique_ptr<Object>& object : objects_) {
		object->Draw3D();
	}
}