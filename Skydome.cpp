#include "Skydome.h"

Skydome::~Skydome() {

}

void Skydome::Initialize(Object_3D* object,Texture* texture) {
	object_ = object;
	texture_ = texture;

	object->isLighting(false);
	object_->SetTexture(texture);
}

void Skydome::Update() {
	
}

void Skydome::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList,Camera camera) {

	data_.camera = camera;
	//3Dモデル描画
	object_->Draw(commandList,data_);
}