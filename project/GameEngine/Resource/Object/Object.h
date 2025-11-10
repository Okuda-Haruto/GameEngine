#pragma once
#include <string>
#include <vector>
#include "DirectXCommon/DirectXCommon.h"

#include "Model/Model.h"
#include "Material.h"
#include "Parts.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

enum REFLECTION {
	REFLECTION_None,
	REFLECTION_Lambert,
	REFLECTION_HalfLambert,
};

class Object {
private:
	// モデル
	Model* model_ = nullptr;
	// パーツ(offset)
	std::vector<Parts> parts_;
	// SRT
	SRT transform_;
public:

	~Object();

	//初期化
	void Initialize(Model* model);
	//描画
	void Draw3D(Camera* camera, DirectionalLight* directionalLight, PointLight* pointLight, SpotLight* spotLight);

	std::vector<Parts> GetParts() { return parts_; }
	void SetParts(Parts parts,UINT index) { parts_[index] = parts; }
	SRT GetTransform() { return transform_; }
	void SetTransform(SRT transform) { transform_ = transform; }

	void SetReflection(UINT reflection);
	void SetShininess(float shininess);
	
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW& GetVBV() { return model_->GetVBV(); }
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW& GetIBV() { return model_->GetIBV(); }
	//頂点の数
	UINT GetVertexIndex() { return model_->GetVertexIndex(); }
	//オフセット
	std::vector<Offset> GetOffsets() { return model_->GetOffsets(); }
};