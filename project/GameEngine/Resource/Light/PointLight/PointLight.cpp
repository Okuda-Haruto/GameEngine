#include "PointLight.h"

#include "GameEngine.h"

PointLight::~PointLight() {
	pointLightElementData_ = nullptr;
}

void PointLight::Initialize(DirectXCommon* dxCommon) {

	//光源用のリソースを作る
	lightElementResource_ = dxCommon->CreateBufferResources(sizeof(PointLightElement));
	//書き込むためのアドレスを取得
	lightElementResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightElementData_));

	pointLightElementData_->color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	pointLightElementData_->position = { 0.0f,2.0f,0.0f };
	pointLightElementData_->intensity = 1.0f;
	pointLightElementData_->radius = 20.0f;
	pointLightElementData_->decay = 1.0f;

	lightElementResource_->Unmap(0, nullptr);

}

void PointLight::SetColor(Vector4 color) {
	pointLightElementData_->color = color;
}

void PointLight::SetPosition(Vector3 position) {
	pointLightElementData_->position = position;
}

void PointLight::SetIntensity(float intensity) {
	pointLightElementData_->intensity = intensity;
}

void PointLight::SetRadius(float radius) {
	pointLightElementData_->radius = radius;
}

void PointLight::SetDecay(float decay) {
	pointLightElementData_->decay = decay;
}

void PointLight::SetPointLightElement(PointLightElement PointLightElement) {
	pointLightElementData_->color = PointLightElement.color;
	pointLightElementData_->position = PointLightElement.position;
	pointLightElementData_->intensity = PointLightElement.intensity;
	pointLightElementData_->radius = PointLightElement.radius;
	pointLightElementData_->decay = PointLightElement.decay;
}