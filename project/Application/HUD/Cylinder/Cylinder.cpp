#include "Cylinder.h"
#include <ModelManager/ModelManager.h>

#include <Entity/Player/Player.h>

void Cylinder::Initialize(std::shared_ptr<DirectionalLight> directionalLight, Player* player) {
	player_ = player;

	object_ = std::make_unique<Object>();
	object_->Initialize(ModelManager::GetInstance()->GetModel("resources/HUD/Cylinder","Cylinder.obj"));
	object_->SetDirectionalLight(directionalLight);
	transform_.scale = { 0.0005f, 0.0005f, 0.0005f };
	transform_.rotate.x = std::numbers::pi_v<float> / 180 * 9;
	transform_.rotate.y = std::numbers::pi_v<float> / 180 * -15;
	transform_.translate = { -0.0032f,-0.0015f,0.01f };
	object_->SetTransform(transform_);
}

void Cylinder::Update() {
	object_->Update();
}

void Cylinder::Draw() {
	int32_t remainingRounds = player_->GetRemainingRounds();
	std::vector<Parts> parts = object_->GetParts();
	for (int32_t i = 1; i <= 6; i++) {
		if (remainingRounds >= i) {
			parts[i].material->color = { 1.0f,1.0f,1.0f,1.0f };
		} else {
			parts[i].material->color = { 1.0f,1.0f,1.0f,0.0f };
		}
		object_->SetParts(parts[i], i);
	}
	object_->Draw2D();
}