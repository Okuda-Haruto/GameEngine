#pragma once
#include <string>
#include <wrl.h>
#include <DirectXCommon/DirectXCommon.h>
#include <Model/Model.h>
#include <GPUAABB.h>

using namespace std;

struct OffsetAllocation {
	uint32_t vertexStart;
	uint32_t vertexCount;
	uint32_t indexStart;
	uint32_t indexCount;
};

struct ObjectData {
	GPUAABB rayTracingAABB;
	OffsetAllocation allocation;
};

class ObjectManager {
public:
	static const uint32_t maxIndexSize = 1048576;	//2^20

private:

	static unique_ptr<ObjectManager> instance;

	DirectXCommon* dxCommon_ = nullptr;
	SRVManager* srvManager_ = nullptr;

	//全体オブジェクトデータ
	 
	//次の頂点開始地点
	uint32_t nextVertexAllocation;
	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//頂点リソースデータ
	VertexData* mappedVertexData_ = nullptr;

	//頂点バッファIndex
	uint32_t verticesBufferSRVindex_;
	uint32_t verticesBufferUAVindex_;

	//次のインデックス開始地点
	uint32_t nextIndexAllocation;
	//インデックスリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	//インデックスデータ
	uint32_t* mappedIndexData_ = nullptr;
public:

	ObjectManager() = default;
	~ObjectManager() = default;
	ObjectManager(ObjectManager&) = delete;
	ObjectManager& operator=(ObjectManager&) = delete;

	//シングルトンインスタンスの取得
	static ObjectManager* GetInstance();

	//終了
	void Finalize();

	//初期化
	void Initialize(DirectXCommon* dxCommon, SRVManager* srvManager);

	uint32_t GetVerticesBufferUAVindex() { return verticesBufferUAVindex_; }
	uint32_t GetVerticesBufferSRVindex() { return verticesBufferSRVindex_; }

	OffsetAllocation MakeNewOffsetAllocation(shared_ptr<Model> model);

	Microsoft::WRL::ComPtr<ID3D12Resource> MakeObjectDataSRVResource();
	Microsoft::WRL::ComPtr<ID3D12Resource> MakeObjectDataUAVResource();
};