#pragma once
#include <string>
#include <vector>
#include "DirectXCommon/DirectXCommon.h"

#include "Model/Model.h"
#include "Material.h"
#include "Parts.h"
#include <Camera/Camera.h>
#include <DirectionalLight/DirectionalLight.h>
#include <PointLight/PointLight.h>
#include <SpotLight/SpotLight.h>

using namespace std;

enum REFLECTION {
	REFLECTION_None,
	REFLECTION_Lambert,
	REFLECTION_HalfLambert,
};

class Object {
private:
	// モデル
	std::shared_ptr<Model> model_ = nullptr;
	// パーツ(offset)
	std::vector<Parts> parts_;
	// SRT
	SRT transform_;

	//カメラ
	shared_ptr<Camera> camera_;

	weak_ptr<DirectionalLight> directionalLight_;
	weak_ptr<PointLight> pointLight_;
	weak_ptr<SpotLight> spotLight_;

	//デフォルトカメラ
	static shared_ptr<Camera> DefaultCamera;
public:

	~Object();

	//初期化
	void Initialize(shared_ptr<Model> model);
	//描画
	void Draw3D();
	void Draw2D();

	static shared_ptr<Camera> GetDefaultCamera() { return DefaultCamera; }
	static void SetDefaultCamera(shared_ptr<Camera> defaultCamera) { DefaultCamera = defaultCamera; }
	//デフォルトカメラ消去
	static void FinalizeDefaultCamera() { DefaultCamera.reset(); }

	shared_ptr<Camera> GetCamera() { return camera_; }
	void SetCamera(shared_ptr<Camera> camera) { camera_ = camera; }

	std::vector<Parts> GetParts() { return parts_; }
	void SetParts(Parts parts,UINT index) { parts_[index] = parts; }
	SRT GetTransform() { return transform_; }
	void SetTransform(SRT transform) { transform_ = transform; }

	void SetColor(Vector4 color) { for (Parts part : parts_) { part.material->color = color; }; }

	//反射方法(例:REFLECTION_Lambert)
	void SetReflection(UINT reflection);
	//鏡面反射(大きいほどつるつるになる。例:40.0f)
	void SetShininess(float shininess);

	void SetDirectionalLight(const shared_ptr<DirectionalLight>& directionalLight) { directionalLight_ = directionalLight; }
	void SetPointLight(const shared_ptr<PointLight>& pointLight) { pointLight_ = pointLight; }
	void SetSpotLight(const shared_ptr<SpotLight>& spotLight) { spotLight_ = spotLight; }
	
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW& GetVBV() { return model_->GetVBV(); }
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW& GetIBV() { return model_->GetIBV(); }
	//頂点の数
	UINT GetVertexIndex() { return model_->GetVertexIndex(); }
	//オフセット
	std::vector<Offset> GetOffsets() { return model_->GetOffsets(); }
};