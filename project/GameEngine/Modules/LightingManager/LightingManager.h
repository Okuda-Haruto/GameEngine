#pragma once

#include <d3d12.h>

#include <wrl.h>
#include "DirectionalLightElement.h"
#include "DirectXCommon/DirectXCommon.h"
#include <SRVManager/SRVManager.h>
#include <LightingState.h>

//光源
class LightingManager {
private:

	static std::unique_ptr<LightingManager> instance;

	DirectXCommon* dxCommon_ = nullptr;
	SRVManager* srvManager_ = nullptr;

	//並行光源リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	//並行光源データ
	DirectionalLightElement* directionalLightData_ = nullptr;
	//並行光源バッファIndex
	uint32_t directionalLightBufferSRVindex_;

	//並行光源データ
	std::vector<std::weak_ptr<DirectionalLightElement>> directionalLights_;

	//光源データリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> lightingStateResource_;
	//各種光源の数などのデータ
	LightingState* lightingState_;

public:

	LightingManager() = default;
	~LightingManager() = default;
	LightingManager(LightingManager&) = delete;
	LightingManager& operator=(LightingManager&) = delete;

	//シングルトンインスタンスの取得
	static LightingManager* GetInstance();

	//終了
	void Finalize();

	//初期化
	void Initialize(DirectXCommon* dxCommon, SRVManager* srvManager);

	//平行光源をセット
	void SetNewDirectionalLigth(std::weak_ptr<DirectionalLightElement> directionalLight);

	//描画前処理
	void PreDraw();

	//getter
	uint32_t GetDirectionalLightSRVindex() { return directionalLightBufferSRVindex_; }

	ID3D12Resource* GetDirectionalLightResource() { return directionalLightResource_.Get(); }
	ID3D12Resource* GetLightingStateResource() { return lightingStateResource_.Get(); }
};