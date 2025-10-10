#pragma once

#include <d3d12.h>

#include <wrl.h>
#include "PointLightElement.h"

//光源
class PointLight {
private:
	//光源用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> PointLightElementResource_;
	//光源データ
	PointLightElement* PointLightElementData_ = nullptr;
public:

	~PointLight();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize();

	// Color入力
	void SetColor(Vector4 color);
	// Position入力
	void SetPosition(Vector3 position);
	// Intensity入力
	void SetIntensity(float intensity);
	//Radius入力
	void SetRadius(float radius);
	//Decay入力
	void SetDecay(float decay);
	// PointLightElement入力
	void SetPointLightElement(PointLightElement PointLightElement);

	//光源用のリソース
	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource>& PointLightElementResource();

};