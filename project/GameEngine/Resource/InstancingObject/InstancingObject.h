#pragma once
#include <string>
#include <list>
#include "DirectXCommon/DirectXCommon.h"

#include "Parts.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class InstancingObject {
private:
	// パーツ
	std::vector<Parts> parts_;
	// SRT
	std::list<SRT> transforms_;

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
public:

	//初期化
	void Initialize(const std::string& directoryPath, const std::string& filename, DirectXCommon* dxCommon);
	//描画
	void Draw3D(Camera* camera, int reflection, DirectionalLight* directionalLight, PointLight* pointLight, SpotLight* spotLight);

	std::vector<Parts> GetParts() { return parts_; }
	std::list<SRT> GetTransforms() { return transforms_; }
	void SetTransforms(std::list<SRT> transforms) { transforms_ = transforms; }
	void AddTransform(SRT transform) { transforms_.push_back(transform); }
	void CleraTransform() { transforms_.clear(); }
};