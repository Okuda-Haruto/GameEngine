#include "Tumbleweed.h"
#include "GameEngine.h"
#include <numbers>

Tumbleweed::~Tumbleweed() {

}

void Tumbleweed::Initialize(shared_ptr<Camera> camera, shared_ptr<DirectionalLight> directionalLight, shared_ptr<PointLight> pointLight) {
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

		Transform.translate.y = 0.0f;
		Transform.scale = { 5,5,5 };
	}

}

void Tumbleweed::Draw() {
	for (const unique_ptr<Object>& object : objects_) {
		object->Draw3D();
	}
}