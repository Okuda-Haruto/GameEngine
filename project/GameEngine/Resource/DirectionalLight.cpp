#include "DirectionalLight.h"

#include "CreateBufferResource.h"
#include "GameEngine.h"

DirectionalLight::~DirectionalLight() {
	directionalLightElementData_ = nullptr;
}

void DirectionalLight::Initialize() {

	//光源用のリソースを作る
	DirectionalLightElementResource_ = CreateBufferResource(GameEngine::GetDevice(), sizeof(DirectionalLightElement));
	//書き込むためのアドレスを取得
	DirectionalLightElementResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightElementData_));

	directionalLightElementData_->color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	directionalLightElementData_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightElementData_->intensity = 1.0f;

	DirectionalLightElementResource_->Unmap(0, nullptr);

}

void DirectionalLight::SetColor(Vector4 color) {
	directionalLightElementData_->color = color;
}

void DirectionalLight::SetDirection(Vector3 direction) {
	directionalLightElementData_->direction = direction;
}

void DirectionalLight::SetIntensity(float intensity) {
	directionalLightElementData_->intensity = intensity;
}

void DirectionalLight::SetDirectionalLightElement(DirectionalLightElement directionalLightElement) {
	SetColor(directionalLightElement.color);
	SetDirection(directionalLightElement.direction);
	SetIntensity(directionalLightElement.intensity);
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource>& DirectionalLight::DirectionalLightElementResource() {
	return DirectionalLightElementResource_;
}