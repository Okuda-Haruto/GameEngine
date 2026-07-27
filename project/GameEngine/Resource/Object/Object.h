#pragma once
#include <string>
#include <vector>
#include "DirectXCommon/DirectXCommon.h"

#include "Model/Model.h"
#include "Material.h"
#include "Parts.h"
#include <AnimationInterpolation.h>
#include <Camera/Camera.h>
#include <DirectionalLight/DirectionalLight.h>
#include <PointLight/PointLight.h>
#include <SpotLight/SpotLight.h>
#include <Joint.h>
#include <ObjectManager/ObejctManager.h>

using namespace std;

class Object {
private:
	// モデル
	std::shared_ptr<Model> model_ = nullptr;
	// SRT
	SRT transform_;
	SRT uvTransform_;

	//元データ
	Microsoft::WRL::ComPtr<ID3D12Resource> objectResource_;
	ObjectData* objectData_;
	//加工済みデータ
	Microsoft::WRL::ComPtr<ID3D12Resource> processedResource_;
	ObjectData* processedData_;
	//マテリアル
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_;

	//カメラ
	shared_ptr<Camera> camera_;

	weak_ptr<DirectionalLight> directionalLight_;
	weak_ptr<PointLight> pointLight_;
	weak_ptr<SpotLight> spotLight_;

	//デフォルトカメラ
	static shared_ptr<Camera> DefaultCamera;

	//アニメーションを使用するか
	bool isUseAnimation_ = false;
	//使用するアニメーション
	std::string animationName_;
	//アニメーションをループさせるか
	bool isLoopAnimation_ = false;
	//アニメーション用時間
	float animationTime_ = 0.0f;
	//アニメーション補完方法
	AnimationInterpolation interpolation_ = AnimationInterpolation::Linear;

	//ボーン
	std::vector<Bone> bones_;
	
	//ジョイント
	Skeleton skeleton_;

	//環境マップ用テクスチャキューブ
	int32_t cubeTextureIndex_;


public:

	~Object();

	//初期化
	void Initialize(shared_ptr<Model> model);

	//更新処理
	void Update();

	//描画
	void Draw3D();
	void Draw3D(uint32_t index);
	void Draw2D();
	void Draw2D(uint32_t index);

	void ResetTimer() { animationTime_ = 0.0f; }

	ID3D12Resource* GetObjectResource() { return objectResource_.Get(); }
	ObjectData* GetObjectData() { return objectData_; }
	ID3D12Resource* GetProcessedResource() { return processedResource_.Get(); }
	ID3D12Resource* GetMaterialResource() { return materialResource_.Get(); }
	void SetMaterial(Material material) { *materialData_ = material; }
	void SetColor(Vector4 color) { materialData_->color = color; }
	void SetRefrection(int32_t refrection) { materialData_->reflection = refrection; }
	void SetShading(SHADING shading) { materialData_->shading = shading; }
	void SetShininess(float shininess) { materialData_->shininess = shininess; }

	static shared_ptr<Camera> GetDefaultCamera() { return DefaultCamera; }
	static void SetDefaultCamera(shared_ptr<Camera> defaultCamera) { DefaultCamera = defaultCamera; }
	//デフォルトカメラ消去
	static void FinalizeDefaultCamera() { DefaultCamera.reset(); }

	shared_ptr<Camera> GetCamera() { return camera_; }
	void SetCamera(shared_ptr<Camera> camera) { camera_ = camera; }

	SRT GetTransform() { return transform_; }
	void SetTransform(SRT transform) { transform_ = transform; }

	void SetDirectionalLight(const shared_ptr<DirectionalLight>& directionalLight) { directionalLight_ = directionalLight; }
	void SetPointLight(const shared_ptr<PointLight>& pointLight) { pointLight_ = pointLight; }
	void SetSpotLight(const shared_ptr<SpotLight>& spotLight) { spotLight_ = spotLight; }
	void SetCubeTextureIndex(int32_t cubeTextureIndex) { cubeTextureIndex_ = cubeTextureIndex; }

	int32_t GetCubeTextureIndex() { return cubeTextureIndex_; }

	//アニメーションを使用するか
	void SetIsUseAnimation(bool isUseAnimation) { isUseAnimation_ = isUseAnimation; }
	//使用するアニメーション
	void SetAnimationName(std::string animationName) { animationName_ = animationName; }
	//アニメーションをループさせるか
	void SetIsLoopAnimation(bool isLoopAnimation) { isLoopAnimation_ = isLoopAnimation; }
	//アニメーション時間リセット
	void ResetAnimationTime() { animationTime_ = 0.0f; }
	//アニメーションが終了しているか
	bool IsEndAnimation(){ return model_->IsEndAnimation(animationTime_, animationName_); }
	//アニメーション補完方法
	void SetAnimationInterpolation(AnimationInterpolation interpolation) { interpolation_ = interpolation; }

	Skeleton GetSleleton() { return skeleton_; }

	//ボーン
	std::vector<Bone> GetBones() { return bones_; }

	std::vector<VertexData> GetModelVertices() { return model_->GetVertices(); }
	std::vector<VertexInfluence> GetVertexInfluences() { return model_->GetVertexInfluences(); }
};