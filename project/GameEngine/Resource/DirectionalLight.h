#pragma once

#include <d3d12.h>

#include <wrl.h>
#include "DirectionalLightElement.h"

//光源
class DirectionalLight {
private:
	//光源用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> DirectionalLightElementResource_;
	//光源データ
	DirectionalLightElement* directionalLightElementData_ = nullptr;
public:

	~DirectionalLight();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize();

	// Color入力
	void SetColor(Vector4 color);
	// Direction入力
	void SetDirection(Vector3 direction);
	// Intensity入力
	void SetIntensity(float intensity);
	// DirectionalLightElement入力
	void SetDirectionalLightElement(DirectionalLightElement directionalLightElement);

	//光源用のリソース
	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource>& DirectionalLightElementResource();

};