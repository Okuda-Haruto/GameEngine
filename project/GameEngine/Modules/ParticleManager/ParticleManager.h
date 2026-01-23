#pragma once
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXCommon/DirectXCommon.h>
#include <SRVManager/SRVManager.h>
#include <VertexData.h>
#include <Material.h>
#include <Particle.h>
#include <Emitter.h>
#include <Field.h>
#include <InstancingTransformationMatrix.h>
#include <Camera/Camera.h>

using namespace std;

struct ParticleGroup {
	std::string TextureFilePath;
	uint32_t textureIndex = 0;// テクスチャ番号
	Emitter emitter;
	AccelerationField accelerationField;
	std::list<Particle> particles;
	uint32_t instancingIndex = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
	uint32_t numInstance = 0;
	std::vector <InstancingTransformationMatrix> instancingData;
};

class ParticleManager {
private:

	static unique_ptr<ParticleManager> instance;

	DirectXCommon* dxCommon_ = nullptr;

	SRVManager* srvManager_ = nullptr;

	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//頂点リソースデータ
	VertexData* vertexData_ = nullptr;
	//インデックスリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	//インデックスデータ
	uint32_t* indexData_ = nullptr;

	shared_ptr<Camera> camera_;

	std::unordered_map<std::string, ParticleGroup> particleGroups;

	//メタデータ
	DirectX::TexMetadata metadata_;
public:

	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(ParticleManager&) = delete;
	ParticleManager& operator=(ParticleManager&) = delete;

	//シングルトンインスタンスの取得
	static ParticleManager* GetInstance();

	//終了
	void Finalize();

	//初期化
	void Initialize(DirectXCommon* dxCommon,SRVManager* srvManager);

	void Update(std::string name);

	void Draw(std::string name);

	void CreateParticleGroup(const std::string name, const std::string textureFilePath);

	void Emit(const std::string name, uint32_t count);

	void SetEmitter(const std::string name, Emitter emitter);
	void SetField(const std::string name, AccelerationField accelerationField);

	void SetCamera(shared_ptr<Camera> camera) { camera_ = camera; }

	std::unordered_map<std::string, ParticleGroup> GetParticleGroups() { return particleGroups; }

	D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return vertexBufferView_; }
	D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return indexBufferView_; }

	void Reset();
};