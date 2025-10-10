#include "PointLight.h"

#include "CreateBufferResource.h"
#include "GameEngine.h"

PointLight::~PointLight() {
	PointLightElementData_ = nullptr;
}

void PointLight::Initialize() {

	//光源用のリソースを作る
	PointLightElementResource_ = CreateBufferResource(GameEngine::GetDevice(), sizeof(PointLightElement));
	//書き込むためのアドレスを取得
	PointLightElementResource_->Map(0, nullptr, reinterpret_cast<void**>(&PointLightElementData_));

	PointLightElementData_->color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	PointLightElementData_->position = { 0.0f,2.0f,0.0f };
	PointLightElementData_->intensity = 1.0f;
	PointLightElementData_->radius = 20.0f;
	PointLightElementData_->decay = 1.0f;

	PointLightElementResource_->Unmap(0, nullptr);

}

void PointLight::SetColor(Vector4 color) {
	PointLightElementData_->color = color;
}

void PointLight::SetPosition(Vector3 position) {
	PointLightElementData_->position = position;
}

void PointLight::SetIntensity(float intensity) {
	PointLightElementData_->intensity = intensity;
}

void PointLight::SetRadius(float radius) {
	PointLightElementData_->radius = radius;
}

void PointLight::SetDecay(float decay) {
	PointLightElementData_->decay = decay;
}

void PointLight::SetPointLightElement(PointLightElement PointLightElement) {
	SetColor(PointLightElement.color);
	SetPosition(PointLightElement.position);
	SetIntensity(PointLightElement.intensity);
	SetRadius(PointLightElement.radius);
	SetDecay(PointLightElement.decay);
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource>& PointLight::PointLightElementResource() {
	return PointLightElementResource_;
}