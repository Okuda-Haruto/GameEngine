#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <DirectXCommon/DirectXCommon.h>

//光源
class Light {
protected:
	//光源用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> lightElementResource_;
public:

	//初期化
	virtual void Initialize(DirectXCommon* dxCommon) = 0;

	//光源用のリソース
	[[nodiscard]]
	const Microsoft::WRL::ComPtr<ID3D12Resource>& LightElementResource() { return lightElementResource_; }
};