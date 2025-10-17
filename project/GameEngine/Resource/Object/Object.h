#pragma once
#include <string>
#include <vector>
#include "Parts.h"

#include "Camera.h"
#include "DirectionalLight.h"
#include "PointLight.h"

class Object {
private:
	// パーツ
	std::vector<Parts> parts_;
	// SRT
	SRT transform_;

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
public:

	//初期化
	void Initialize(const std::string& directoryPath, const std::string& filename);
	//描画
	void Draw3D(Camera* camera, int reflection, float shininess, DirectionalLight* directionalLight, PointLight* pointLight);

	std::vector<Parts> GetParts() { return parts_; }
	SRT GetTransform() { return transform_; }
	void SetTransform(SRT transform) { transform_ = transform; }
};