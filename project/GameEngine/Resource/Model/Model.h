#pragma once
#include <wrl.h>
#include <vector>
#include <ModelData.h>
#include <d3d12.h>
#include <string>
#include <ObjectVertexData.h>
#include <AnimationInterpolation.h>
#include <Offset.h>
#include <DirectXCommon/DirectXCommon.h>
#include <TextureManager/TextureManager.h>

//モデル
class Model {
private:
	//頂点の数
	UINT vertexIndex_;

	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//頂点リソースデータ
	ObjectVertexData* vertexData_ = nullptr;

	//インデックスリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	//インデックスデータ
	uint32_t* indexData_ = nullptr;

	//モデルデータ
	ModelData modelData_;

	DirectXCommon* dxCommon_ = nullptr;
public:
	~Model();

	//初期値
	void Initialize(const std::string& directoryPath, const std::string& filename, DirectXCommon* dxCommon);

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW& GetVBV() { return vertexBufferView_; }
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW& GetIBV() { return indexBufferView_; }
	//頂点の数
	UINT GetVertexIndex() { return vertexIndex_; }
	//オフセット
	std::vector<Offset> GetOffsets() { return modelData_.offset; }
	//テクスチャ番号
	UINT GetTextureIndex(UINT offsetNum) { if (modelData_.textureIndex.empty()) return TextureManager::GetInstance()->GetWhite2x2(); return modelData_.textureIndex[offsetNum]; }
	//ボーン
	std::vector<Bone> GetBones() { return modelData_.bones; }
	//ボーンアニメーション
	void BoneAnimation(std::vector<Bone>& bones, float time, UINT animationIndex, AnimationInterpolation interpolation);
	//アニメーションが終了しているか
	bool IsEndAnimation(float time, UINT index) { return time * modelData_.animations[index].FPS >= modelData_.animations[index].duration; }
	//アニメーションデータ
	AnimationData GetAnimationData(UINT index) { return modelData_.animations[index]; }

	DirectXCommon* GetDirectXCommon() { return dxCommon_; }
private:

	//階層構造の行列変換
	Matrix4x4 SetWorldMatrix(std::shared_ptr<Node> node, std::vector<Bone>& bones, Bone bone);

	Matrix4x4 FindBoneLocalMatrix(std::vector<Bone>& bones, std::string boneName);

};