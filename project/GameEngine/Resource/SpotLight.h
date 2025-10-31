#pragma once

#include <d3d12.h>

#include <wrl.h>
#include "SpotLightElement.h"
#include "DirectXCommon/DirectXCommon.h"

//光源
class SpotLight {
private:
	//光源用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> SpotLightElementResource_;
	//光源データ
	SpotLightElement* SpotLightElementData_ = nullptr;
public:

	~SpotLight();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(DirectXCommon* dxCommon);

	// Color入力
	void SetColor(Vector4 color);
	// Position入力
	void SetPosition(Vector3 position);
	// Intensity入力
	void SetIntensity(float intensity);
	//Direction入力
	void SetDirection(Vector3 direction);
	//Distance入力
	void SetDistance(float distance);
	//Decay入力
	void SetDecay(float decay);
	//CosAngle入力
	void SetCosAngle(float cosAngle);
	//CosFalloutStart入力
	void SetCosFalloutStart(float cosFalloutStart);
	// SpotLightElement入力
	void SetSpotLightElement(SpotLightElement spotLightElement);

	//光源用のリソース
	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource>& SpotLightElementResource();

};