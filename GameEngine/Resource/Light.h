#pragma once

#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>

#include <wrl.h>
#include "DirectionalLight.h"


//光源
class Light {
private:
	//光源用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	//光源データ
	DirectionalLight* directionalLightData_ = nullptr;
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device);

	// Color入力
	void SetColor(Vector4 color);
	// Direction入力
	void SetDirection(Vector3 direction);
	// Intensity入力
	void SetIntensity(float intensity);

	//光源用のリソース
	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource>& directionalLightResource();

};