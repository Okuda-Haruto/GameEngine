#include "Fence.h"
#include "GameEngine.h"
#include <numbers>

Fence::~Fence() {

}

void Fence::Initialize(shared_ptr<Camera> camera, shared_ptr<DirectionalLight> directionalLight, shared_ptr<PointLight> pointLight){
	directionalLight_ = directionalLight;
	pointLight_ = pointLight;

	SRT transform{};
	transform.scale = { 1.0f,1.0f,1.0f };
	//モデルの生成
	for (int i = 0; i < size; i++) {
		
		unique_ptr<Object> object = make_unique<Object>();
		object->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::Fence));

		switch (i / (size / 4))
		{
		case 0:
			transform.rotate.y = 0.0f;
			transform.translate.x = 3.0f * (-size / 8 + i);
			transform.translate.z = 3.0f * -size / 8;
			break;
		case 1:
			transform.rotate.y = -std::numbers::pi_v<float> / 2;
			transform.translate.x = -3.0f * -size / 8;
			transform.translate.z = 3.0f * (-size / 8 + i - size / 4);
			break;
		case 2:
			transform.rotate.y = -std::numbers::pi_v<float>;
			transform.translate.x = -3.0f * (-size / 8 + i - size / 4 * 2);
			transform.translate.z = -3.0f * -size / 8;
			break;
		case 3:
			transform.rotate.y = -std::numbers::pi_v<float> *3 / 2;
			transform.translate.x = 3.0f * -size / 8;
			transform.translate.z = -3.0f * (-size / 8 + i - size / 4 * 3);
			break;
		default:
			break;
		}
		object->SetTransform(transform);
		object->SetCamera(camera);
		object->SetDirectionalLight(directionalLight_);
		object->SetShininess(0);

		objects_.push_back(move(object));
	}
}

void Fence::Draw() {
	for (const unique_ptr<Object>& object : objects_) {
		object->Draw3D();
	}
}