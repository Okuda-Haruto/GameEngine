#pragma once
#include <string>
#include <vector>
#include "DirectXCommon/DirectXCommon.h"

#include "Model/Model.h"
#include "Material.h"
#include "Parts.h"
#include "AnimationInterpolation.h"
#include "Camera/Camera.h"
#include "Light/DirectionalLight/DirectionalLight.h"
#include "Light/PointLight/PointLight.h"
#include "Light/SpotLight/SpotLight.h"
#include "Joint.h"

class InstancingObject {
private:

	// モデル
	std::weak_ptr<Model> model_;
	// パーツ(offset)
	std::vector<Parts> parts_;

	struct ObjectData {
		SRT transform;
		Vector4 color;
		SRT uvTransform;
	};

	//Spriteデータ
	std::unordered_map<std::string, ObjectData> dataMap_;

	// テクスチャ番号
	uint32_t textureIndex_ = 0;

	//カメラ
	std::weak_ptr<Camera> camera_;

	std::weak_ptr<DirectionalLight> directionalLight_;
	std::weak_ptr<PointLight> pointLight_;
	std::weak_ptr<SpotLight> spotLight_;

	//環境マップ用テクスチャキューブ
	int32_t cubeTextureIndex_;

public:

	//初期化
	void Initialize(std::weak_ptr<Model> model);
	//更新
	void Update();
	//描画
	void Draw3D();

	//Object追加
	void AddNewObject(const std::string objectName);

	//Object削除
	void DeleteObject(const std::string objectName);

	std::vector<Parts> GetParts() { return parts_; }
	void SetParts(Parts parts, uint32_t index) { parts_[index] = parts; }
	SRT GetTransform(const std::string objectName) { return dataMap_[objectName].transform; }
	void SetTransform(const std::string objectName, SRT transform) { dataMap_[objectName].transform = transform; }

	Vector4 GetColor(const std::string objectName) { return dataMap_[objectName].color; }
	void SetColor(const std::string objectName, Vector4 color) { dataMap_[objectName].color = color; }

	SRT GetUVTransform(const std::string objectName) { return dataMap_[objectName].uvTransform; }
	void SetUVTransform(const std::string objectName, SRT uvTransform) { dataMap_[objectName].uvTransform = uvTransform; }



	const std::weak_ptr<Camera> GetCamera() { return camera_; }
	void SetCamera(std::weak_ptr<Camera> camera) { camera_ = camera; }

	//反射方法(例:REFLECTION_Lambert)
	void SetReflection(UINT reflection);

	void SetShading(UINT shading);
	//鏡面反射(大きいほどつるつるになる。例:40.0f)
	void SetShininess(float shininess);

	void SetDirectionalLight(const std::weak_ptr<DirectionalLight>& directionalLight) { directionalLight_ = directionalLight; }
	void SetPointLight(const std::weak_ptr<PointLight>& pointLight) { pointLight_ = pointLight; }
	void SetSpotLight(const std::weak_ptr<SpotLight>& spotLight) { spotLight_ = spotLight; }
	void SetCubeTextureIndex(int32_t cubeTextureIndex) { cubeTextureIndex_ = cubeTextureIndex; }

	int32_t GetCubeTextureIndex() { return cubeTextureIndex_; }

	//頂点バッファビュー
	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() { return model_.lock()->GetVBV(); }
	//インデックスバッファビュー
	const D3D12_INDEX_BUFFER_VIEW& GetIBV() { return model_.lock()->GetIBV(); }
	//頂点の数
	UINT GetVertexIndex() { return model_.lock()->GetVertexIndex(); }
	//オフセット
	std::vector<Offset> GetOffsets() { return model_.lock()->GetOffsets(); }

	//NameListGetter
	std::list<std::string> GetObjectNames() const {
		std::list<std::string> names;

		for (const auto& [name, data] : dataMap_) {
			names.push_back(name);
		}

		return names;
	}
};