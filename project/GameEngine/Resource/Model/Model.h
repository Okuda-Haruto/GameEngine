#pragma once
#include <wrl.h>
#include <vector>
#include <ModelData.h>
#include <d3d12.h>
#include <string>
#include <VertexData.h>
#include <VertexInfluence.h>
#include <AnimationInterpolation.h>
#include <Offset.h>
#include <DirectXCommon/DirectXCommon.h>
#include <TextureManager/TextureManager.h>
#include <Joint.h>

//モデル
class Model {
private:
	//インデックスデータ
	std::vector<uint32_t> indexData_;

	//モデルデータ
	ModelData modelData_;
	Skeleton skeleton_;

	DirectXCommon* dxCommon_ = nullptr;
public:
	~Model();

	//初期値
	void Initialize(const std::string& directoryPath, const std::string& filename, DirectXCommon* dxCommon);

	//頂点データ
	std::vector<VertexData> GetVertices() { return modelData_.vertices; }
	//インデックスデータ
	std::vector<uint32_t> GetIndexes() { return modelData_.indexes; }
	//頂点ボーンデータ
	std::vector<VertexInfluence> GetVertexInfluences() { return modelData_.Influences; }

	//オフセット
	std::vector<Offset> GetOffsets() { return modelData_.offset; }
	//テクスチャ番号
	UINT GetTextureIndex(UINT offsetNum) { if (modelData_.textureIndex.empty()) return TextureManager::GetInstance()->GetWhite2x2(); return modelData_.textureIndex[offsetNum]; }
	//ボーン
	std::vector<Bone> GetBones() { return modelData_.bones; }

	Skeleton GetSkeleton() { return skeleton_; }

	//ボーンアニメーション
	void BoneAnimation(Skeleton& skeleton, float time, std::string animationName, AnimationInterpolation interpolation);
	//アニメーションが終了しているか
	bool IsEndAnimation(float time, std::string animationName) { return time  >= modelData_.animations[animationName].duration; }
	//アニメーションデータ
	AnimationData GetAnimationData(std::string animationName) { return modelData_.animations[animationName]; }
	std::unordered_map<string, AnimationData> GetAnimationDatas() { return modelData_.animations; }

	DirectXCommon* GetDirectXCommon() { return dxCommon_; }

	std::string GetTextureFilePath() { return modelData_.material.textureFilepath; }
private:

	//階層構造の行列変換
	Matrix4x4 SetWorldMatrix(std::shared_ptr<Node> node, std::vector<Bone>& bones, Bone bone);

	Matrix4x4 FindBoneLocalMatrix(std::vector<Bone>& bones, std::string boneName);
};