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

using namespace std;

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

	//アニメーションを使用するか
	bool isUseAnimation_ = false;
	//使用するアニメーション
	UINT animationIndex_ = 0;
	//アニメーションをループさせるか
	bool isLoopAnimation_ = false;
	//アニメーション用時間
	float animationTime_ = 0.0f;
	//アニメーション補完方法
	AnimationInterpolation interpolation_ = AnimationInterpolation::Linear;
	//ボーン
	std::vector<Bone> bones_;


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

	static shared_ptr<Camera> GetDefaultCamera() { return DefaultCamera; }
	static void SetDefaultCamera(shared_ptr<Camera> defaultCamera) { DefaultCamera = defaultCamera; }
	//デフォルトカメラ消去
	static void FinalizeDefaultCamera() { DefaultCamera.reset(); }

	shared_ptr<Camera> GetCamera() { return camera_; }
	void SetCamera(shared_ptr<Camera> camera) { camera_ = camera; }

	std::vector<Parts> GetParts() { return parts_; }
	void SetParts(Parts parts, uint32_t index) { parts_[index] = parts; }
	SRT GetTransform() { return transform_; }
	void SetTransform(SRT transform) { transform_ = transform; }

	void SetColor(Vector4 color) { for (Parts part : parts_) { part.material->color = color; }; }

	//反射方法(例:REFLECTION_Lambert)
	void SetReflection(UINT reflection);
	
	void SetShading(UINT shading);
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

	//アニメーションを使用するか
	void SetIsUseAnimation(bool isUseAnimation) { isUseAnimation_ = isUseAnimation; }
	//使用するアニメーション番号
	void SetAnimationIndex(UINT animationIndex) { animationIndex_ = animationIndex; }
	//アニメーションをループさせるか
	void SetIsLoopAnimation(bool isLoopAnimation) { isLoopAnimation_ = isLoopAnimation; }
	//アニメーション時間リセット
	void ResetAnimationTime() { animationTime_ = 0.0f; }
	//アニメーションが終了しているか
	bool IsEndAnimation(){ return model_->IsEndAnimation(animationTime_, animationIndex_); }
	//アニメーション補完方法
	void SetAnimationInterpolation(AnimationInterpolation interpolation) { interpolation_ = interpolation; }

	//ボーン
	std::vector<Bone> GetBones() { return bones_; }
};