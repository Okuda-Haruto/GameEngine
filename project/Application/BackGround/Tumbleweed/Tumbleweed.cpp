#include <numbers>

#include "GameEngine.h"
#include "Tumbleweed.h"

Tumbleweed::~Tumbleweed() {

}

void Tumbleweed::Initialize(weak_ptr<Camera> camera, weak_ptr<DirectionalLight> directionalLight, weak_ptr<PointLight> pointLight) {
	directionalLight_ = directionalLight;
	pointLight_ = pointLight;

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
	moveTime_ += GameEngine::GetDeltaTime();;
	if (moveTime_ >= kMaxMoveTime) {
		moveTime_ -= kMaxMoveTime;
	}

	for (int i = 0; i < objects_.size();i++) {
		SRT transform = {
			{1.0f,1.0f,1.0f},
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f,0.0f}
		};
		transform.rotate.z = -std::numbers::pi_v<float> * 2 * moveTime_;
		transform.rotate.y = std::numbers::pi_v<float> * 2 * (moveTime_ / kMaxMoveTime + float(i) / 3);

		Matrix4x4 rotateMatrix = MakeRotateYMatrix(transform.rotate.y);

		transform.translate = kBasePosition * rotateMatrix;

		objects_[i]->SetTransform(transform);
	}

}

void Tumbleweed::Draw() {
	for (const unique_ptr<Object>& object : objects_) {
		object->Draw3D();
	}
}