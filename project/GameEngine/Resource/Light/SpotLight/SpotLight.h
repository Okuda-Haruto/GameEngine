#pragma once
#include "../Light.h"
#include "SpotLightElement.h"

//光源
class SpotLight : public Light{
private:
	//光源データ
	SpotLightElement* spotLightElementData_ = nullptr;
public:

	~SpotLight();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(DirectXCommon* dxCommon) override;

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
};