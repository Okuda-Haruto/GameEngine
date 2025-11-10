#include "ParticleManager.h"
#include <algorithm>
#include "ConvertString.h"
#include "GameEngine.h"
#include "Collision.h"

ParticleManager* ParticleManager::instance = nullptr;

ParticleManager* ParticleManager::GetInstance() {
	if (instance == nullptr) {
		instance = new ParticleManager;
	}
	return instance;
}

void ParticleManager::Finalize() {
	particleGroups.clear();

	delete instance;
	instance = nullptr;
}

void ParticleManager::Initialize(DirectXCommon* dxCommon, SRVManager* srvManager) {
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	//頂点リソースを作る
	vertexResource_ = dxCommon->CreateBufferResources(sizeof(VertexData) * 4);

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点は4つサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点のローカル座標系を設定
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//左下
	vertexData_[0].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[0].normal = { 0.0f,0.0f,-1.0f };
	//左上
	vertexData_[1].position = { 0.0f,1.0f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[1].normal = { 0.0f,0.0f,-1.0f };
	//右下
	vertexData_[2].position = { 1.0f,0.0f,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[2].normal = { 0.0f,0.0f,-1.0f };
	//右上
	vertexData_[3].position = { 1.0f,1.0f,0.0f,1.0f };
	vertexData_[3].texcoord = { 1.0f,0.0f };
	vertexData_[3].normal = { 0.0f,0.0f,-1.0f };

	vertexResource_->Unmap(0, nullptr);

	//Sprite用のインデックスリソースを作る
	indexResource_ = dxCommon->CreateBufferResources(sizeof(uint32_t) * 6);

	//インデックスバッファビューを作成する
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックスは6つサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//インデックスデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	//3角形2つを組合わせ4角形にする
	indexData_[0] = 0; indexData_[1] = 1; indexData_[2] = 2;
	indexData_[3] = 1; indexData_[4] = 3; indexData_[5] = 2;

	indexResource_->Unmap(0, nullptr);

	camera_ = Object::GetDefaultCamera();
}

void ParticleManager::Update(std::string name) {
	if (particleGroups.contains(name)) {

		for (std::list<Particle>::iterator particleIterator = particleGroups[name].particles.begin();
			particleIterator != particleGroups[name].particles.end(); ++particleIterator) {

			if ((*particleIterator).lifeTime >= particleGroups[name].emitter.lifeTime) {
				particleIterator = particleGroups[name].particles.erase(particleIterator);
				continue;
			}

			if (IsCollision(particleGroups[name].accelerationField.area, (*particleIterator).transform.translate)) {
				(*particleIterator).velocity += particleGroups[name].accelerationField.acceleration;
			}
			(*particleIterator).transform.translate += (*particleIterator).velocity;
			(*particleIterator).color.x = (*particleIterator).beforeColor.x * (1.0f - (*particleIterator).lifeTime / particleGroups[name].emitter.lifeTime) + (*particleIterator).afterColor.x * ((*particleIterator).lifeTime / particleGroups[name].emitter.lifeTime);
			(*particleIterator).color.y = (*particleIterator).beforeColor.y * (1.0f - (*particleIterator).lifeTime / particleGroups[name].emitter.lifeTime) + (*particleIterator).afterColor.y * ((*particleIterator).lifeTime / particleGroups[name].emitter.lifeTime);
			(*particleIterator).color.z = (*particleIterator).beforeColor.z * (1.0f - (*particleIterator).lifeTime / particleGroups[name].emitter.lifeTime) + (*particleIterator).afterColor.z * ((*particleIterator).lifeTime / particleGroups[name].emitter.lifeTime);
			(*particleIterator).color.w = (*particleIterator).beforeColor.w * (1.0f - (*particleIterator).lifeTime / particleGroups[name].emitter.lifeTime) + (*particleIterator).afterColor.w * ((*particleIterator).lifeTime / particleGroups[name].emitter.lifeTime);
			
			(*particleIterator).lifeTime += 1.0f / 60.0f;
		}
	}
}

void ParticleManager::Draw(std::string name) {

	GameEngine::DrawParticle(particleGroups[name]);
}

//テクスチャファイルの読み込み
void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath) {
	//読み込み済みテクスチャを検索
	if (particleGroups.contains(name)) {
		assert(0);
	}

	//テクスチャデータを追加して書き込む
	ParticleGroup& particleGroup = particleGroups[name];

	particleGroup.TextureFilePath = textureFilePath;
	particleGroup.textureIndex = TextureManager::GetInstance()->GetSrvIndex(textureFilePath);

	// 最大インスタンス数（SRV 作成時と合わせる）
	const UINT maxInstances = 32;

	// instancing 用リソースを maxInstances 分確保する（sizeof(ParticleForGPU) * maxInstances）
	particleGroup.instancingResource = dxCommon_->CreateBufferResources(sizeof(ParticleForGPU) * maxInstances);

	particleGroup.instancingIndex = srvManager_->Allocate();
	particleGroup.numInstance = 0;

	// SRV を作成（NumElements と stride は一致させる）
	srvManager_->CreateSRVforStructuredBuffer(particleGroup.instancingIndex, particleGroup.instancingResource.Get(), maxInstances, sizeof(ParticleForGPU));
}

void ParticleManager::Emit(const std::string name, const Vector3& position, uint32_t count) {
	//読み込み済みテクスチャを検索
	if (particleGroups.contains(name)) {
		for (uint32_t i = 0; i < count; i++) {

			if (i > 32) {
				break;
			}
			Particle particle;
			particle.lifeTime = 0.0f;
			particle.transform = particleGroups[name].emitter.transform;
			particle.transform.translate.x = particleGroups[name].emitter.transform.translate.x + GameEngine::randomFloat(-position.x, position.x);
			particle.transform.translate.y = particleGroups[name].emitter.transform.translate.y + GameEngine::randomFloat(-position.y, position.y);
			particle.transform.translate.z = particleGroups[name].emitter.transform.translate.z + GameEngine::randomFloat(-position.z, position.z);
			particle.velocity = particleGroups[name].emitter.velocity;
			particle.beforeColor = particleGroups[name].emitter.beforeColor;
			particle.afterColor = particleGroups[name].emitter.afterColor;
			particle.color = particle.beforeColor;
			particleGroups[name].particles.push_back(particle);
		}
	}
}

void ParticleManager::SetEmitter(const std::string name, Emitter emitter) {
	//読み込み済みテクスチャを検索
	if (particleGroups.contains(name)) {
		particleGroups[name].emitter = emitter;
	}
}

void ParticleManager::SetField(const std::string name, AccelerationField accelerationField) {
	//読み込み済みテクスチャを検索
	if (particleGroups.contains(name)) {
		particleGroups[name].accelerationField = accelerationField;
	}
}