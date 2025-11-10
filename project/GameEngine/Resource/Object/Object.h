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

	//カメラ
	Camera* camera_ = nullptr;

	//デフォルトカメラ
	static Camera* DefaultCamera;
public:

	~Object();

	//初期化
	void Initialize(Model* model);
	//描画
	void Draw3D(DirectionalLight* directionalLight, PointLight* pointLight, SpotLight* spotLight);

	static Camera* GetDefaultCamera() { return DefaultCamera; }
	static void SetDefaultCamera(Camera* defaultCamera) { DefaultCamera = defaultCamera; }
	//デフォルトカメラ消去
	static void FinalizeDefaultCamera() { delete DefaultCamera; }

	Camera* GetCamera() { return camera_; }
	void SetCamera(Camera* camera) { camera_ = camera; }

	std::vector<Parts> GetParts() { return parts_; }
	void SetParts(Parts parts,UINT index) { parts_[index] = parts; }
	SRT GetTransform() { return transform_; }
	void SetTransform(SRT transform) { transform_ = transform; }

	//反射方法(例:REFLECTION_Lambert)
	void SetReflection(UINT reflection);
	//鏡面反射(大きいほどつるつるになる。例:40.0f)
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