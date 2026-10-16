#pragma once
#include "../Light.h"
#include "DirectionalLightElement.h"

//光源
class DirectionalLight : public Light{
private:
	//光源データ
	DirectionalLightElement* directionalLightElementData_ = nullptr;
public:

	~DirectionalLight();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(DirectXCommon* dxCommon) override;

	// Color入力
	void SetColor(Vector4 color);
	// Direction入力
	void SetDirection(Vector3 direction);
	// Intensity入力
	void SetIntensity(float intensity);
	// DirectionalLightElement入力
	void SetDirectionalLightElement(DirectionalLightElement directionalLightElement);
};