#include "SpotLight.h"

#include "GameEngine.h"
#include <numbers>

SpotLight::~SpotLight() {
	SpotLightElementData_ = nullptr;
}

void SpotLight::Initialize(DirectXCommon* dxCommon) {

	//光源用のリソースを作る
	SpotLightElementResource_ = dxCommon->CreateBufferResources(sizeof(SpotLightElement));
	//書き込むためのアドレスを取得
	SpotLightElementResource_->Map(0, nullptr, reinterpret_cast<void**>(&SpotLightElementData_));

	SpotLightElementData_->color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	SpotLightElementData_->position = { 0.0f,2.0f,0.0f };
	SpotLightElementData_->intensity = 1.0f;
	SpotLightElementData_->direction = {1.0f,0.0f,0.0f};
	SpotLightElementData_->distance = 10.0f;
	SpotLightElementData_->decay = 1.0f;
	SpotLightElementData_->cosAngle = std::numbers::pi_v<float> / 180 * 30;
	SpotLightElementData_->cosFalloutStart = std::numbers::pi_v<float> / 180 * 40;

	SpotLightElementResource_->Unmap(0, nullptr);

}

void SpotLight::SetColor(Vector4 color) {
	SpotLightElementData_->color = color;
}

void SpotLight::SetPosition(Vector3 position) {
	SpotLightElementData_->position = position;
}

void SpotLight::SetIntensity(float intensity) {
	SpotLightElementData_->intensity = intensity;
}

void SpotLight::SetDirection(Vector3 direction) {
	SpotLightElementData_->direction = direction;
}

void SpotLight::SetDistance(float distance) {
	SpotLightElementData_->distance = distance;
}

void SpotLight::SetDecay(float decay) {
	SpotLightElementData_->decay = decay;
}

void SpotLight::SetCosAngle(float cosAngle) {
	SpotLightElementData_->cosAngle = cosAngle;
}

void SpotLight::SetCosFalloutStart(float cosFalloutStart) {
	SpotLightElementData_->cosFalloutStart = cosFalloutStart;
}

void SpotLight::SetSpotLightElement(SpotLightElement SpotLightElement) {
	SpotLightElementData_->color = SpotLightElement.color;
	SpotLightElementData_->position = SpotLightElement.position;
	SpotLightElementData_->intensity = SpotLightElement.intensity;
	SpotLightElementData_->direction = SpotLightElement.direction;
	SpotLightElementData_->distance = SpotLightElement.distance;
	SpotLightElementData_->decay = SpotLightElement.decay;
	SpotLightElementData_->cosAngle = SpotLightElement.cosAngle;
	SpotLightElementData_->cosFalloutStart = SpotLightElement.cosFalloutStart;
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource>& SpotLight::SpotLightElementResource() {
	return SpotLightElementResource_;
}