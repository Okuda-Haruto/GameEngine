#include "SpotLight.h"

#include "GameEngine.h"
#include <numbers>

SpotLight::~SpotLight() {
	spotLightElementData_ = nullptr;
}

void SpotLight::Initialize(DirectXCommon* dxCommon) {

	//光源用のリソースを作る
	lightElementResource_ = dxCommon->CreateBufferResources(sizeof(SpotLightElement));
	//書き込むためのアドレスを取得
	lightElementResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightElementData_));

	spotLightElementData_->color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	spotLightElementData_->position = { 0.0f,2.0f,0.0f };
	spotLightElementData_->intensity = 1.0f;
	spotLightElementData_->direction = {1.0f,0.0f,0.0f};
	spotLightElementData_->distance = 10.0f;
	spotLightElementData_->decay = 1.0f;
	spotLightElementData_->cosAngle = std::numbers::pi_v<float> / 180 * 30;
	spotLightElementData_->cosFalloutStart = std::numbers::pi_v<float> / 180 * 40;

	lightElementResource_->Unmap(0, nullptr);

}

void SpotLight::SetColor(Vector4 color) {
	spotLightElementData_->color = color;
}

void SpotLight::SetPosition(Vector3 position) {
	spotLightElementData_->position = position;
}

void SpotLight::SetIntensity(float intensity) {
	spotLightElementData_->intensity = intensity;
}

void SpotLight::SetDirection(Vector3 direction) {
	spotLightElementData_->direction = direction;
}

void SpotLight::SetDistance(float distance) {
	spotLightElementData_->distance = distance;
}

void SpotLight::SetDecay(float decay) {
	spotLightElementData_->decay = decay;
}

void SpotLight::SetCosAngle(float cosAngle) {
	spotLightElementData_->cosAngle = cosAngle;
}

void SpotLight::SetCosFalloutStart(float cosFalloutStart) {
	spotLightElementData_->cosFalloutStart = cosFalloutStart;
}

void SpotLight::SetSpotLightElement(SpotLightElement SpotLightElement) {
	spotLightElementData_->color = SpotLightElement.color;
	spotLightElementData_->position = SpotLightElement.position;
	spotLightElementData_->intensity = SpotLightElement.intensity;
	spotLightElementData_->direction = SpotLightElement.direction;
	spotLightElementData_->distance = SpotLightElement.distance;
	spotLightElementData_->decay = SpotLightElement.decay;
	spotLightElementData_->cosAngle = SpotLightElement.cosAngle;
	spotLightElementData_->cosFalloutStart = SpotLightElement.cosFalloutStart;
}