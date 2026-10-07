#include "LightingManager.h"

std::unique_ptr<LightingManager> LightingManager::instance;

LightingManager* LightingManager::GetInstance() {
    if (!instance) {
        instance = std::make_unique<LightingManager>();
    }
    return instance.get();
}

void LightingManager::Finalize() {
    instance.reset();
}

void LightingManager::Initialize(DirectXCommon* dxCommon, SRVManager* srvManager) {

	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	//頂点リソースを作る
	directionalLightResource_ = dxCommon_->CreateBufferResources(sizeof(DirectionalLightElement) * 8);

	directionalLightBufferSRVindex_ = srvManager_->Allocate();
	srvManager_->CreateSRVforStructuredBuffer(directionalLightBufferSRVindex_, directionalLightResource_.Get(), 8, sizeof(DirectionalLightElement));

    lightingStateResource_ = dxCommon_->CreateBufferResources(sizeof(LightingState));
}

void LightingManager::SetNewDirectionalLigth(std::weak_ptr<DirectionalLightElement> directionalLight) {

    directionalLights_.push_back(directionalLight);
}

void LightingManager::PreDraw() {

    lightingStateResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightingState_));

    directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));

    //カウントの初期化
    lightingState_->numDirectionalLight = 0;

    //消えている光源はGPUに送るデータに含めない
    for (auto it = directionalLights_.begin(); it != directionalLights_.end();){
        if (it->expired())
        {
            it = directionalLights_.erase(it);
            continue;
        }

        auto light = it->lock();

        //GPUに送る用のデータに書き込む
        directionalLightData_[lightingState_->numDirectionalLight] = *light;
        lightingState_->numDirectionalLight++;

        ++it;
    }

    directionalLightResource_->Unmap(0, nullptr);

    lightingStateResource_->Unmap(0, nullptr);

}