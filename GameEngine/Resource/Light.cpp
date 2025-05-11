#include "Light.h"

#include "CreateBufferResource.h"

Light::~Light() {
	directionalLightData_ = nullptr;
}

void Light::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device) {

	//光源用のリソースを作る
	directionalLightResource_ = CreateBufferResource(device, sizeof(DirectionalLight));
	//書き込むためのアドレスを取得
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));

	directionalLightData_->color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData_->intensity = 1.0f;

	directionalLightResource_->Unmap(0, nullptr);

}

void Light::SetColor(Vector4 color) {
	directionalLightData_->color = color;
}

void Light::SetDirection(Vector3 direction) {
	directionalLightData_->direction = direction;
}

void Light::SetIntensity(float intensity) {
	directionalLightData_->intensity = intensity;
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource>& Light::directionalLightResource() {
	return directionalLightResource_;
}