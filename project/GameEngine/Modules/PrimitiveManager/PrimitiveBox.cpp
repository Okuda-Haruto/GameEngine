#include "PrimitiveBox.h"
#include "GameEngine.h"
#include "LoadObjFile.h"
#include "LoadGLTFFile.h"
#include "Animation/Animation.h"

PrimitiveBox::~PrimitiveBox() {

}

void PrimitiveBox::Initialize(uint32_t textureIndex, std::shared_ptr<Camera> camera, DirectXCommon* dxCommon) {

	textureIndex_ = textureIndex;

	camera_ = camera;

	//頂点リソースを作る
	vertexResource_ = dxCommon->CreateBufferResources(sizeof(VertexData) * 24);

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点は4つサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 24;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//Primitive用のインデックスリソースを作る
	indexResource_ = dxCommon->CreateBufferResources(sizeof(uint32_t) * 3 * 12);

	//インデックスバッファビューを作成する
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 3 * 12;
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//頂点のローカル座標系を設定
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//インデックスデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

#pragma region Box

	//右
	vertexData_[0].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[0].normal = { -1.0f,  0.0f, 0.0f };
	vertexData_[1].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData_[1].texcoord = { 1.0f,1.0f };
	vertexData_[1].normal = { -1.0f,  0.0f, 0.0f };
	vertexData_[2].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData_[2].texcoord = { 0.0f,0.0f };
	vertexData_[2].normal = { -1.0f,  0.0f, 0.0f };
	vertexData_[3].position = { 1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[3].texcoord = { 1.0f,0.0f };
	vertexData_[3].normal = { -1.0f,  0.0f, 0.0f };
	//左
	vertexData_[4].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData_[4].texcoord = { 0.0f,1.0f };
	vertexData_[4].normal = { 1.0f,  0.0f, 0.0f };
	vertexData_[5].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData_[5].texcoord = { 1.0f,1.0f };
	vertexData_[5].normal = { 1.0f,  0.0f, 0.0f };
	vertexData_[6].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[6].texcoord = { 0.0f,0.0f };
	vertexData_[6].normal = { 1.0f,  0.0f, 0.0f };
	vertexData_[7].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexData_[7].texcoord = { 1.0f,0.0f };
	vertexData_[7].normal = { 1.0f,  0.0f, 0.0f };
	//前
	vertexData_[8].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData_[8].texcoord = { 0.0f,1.0f };
	vertexData_[8].normal = { 0.0f,  0.0f, -1.0f };
	vertexData_[9].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData_[9].texcoord = { 1.0f,1.0f };
	vertexData_[9].normal = { 0.0f,  0.0f, -1.0f };
	vertexData_[10].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexData_[10].texcoord = { 0.0f,0.0f };
	vertexData_[10].normal = { 0.0f,  0.0f, -1.0f };
	vertexData_[11].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData_[11].texcoord = { 1.0f,0.0f };
	vertexData_[11].normal = { 0.0f,  0.0f, -1.0f };
	//後
	vertexData_[12].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData_[12].texcoord = { 0.0f,1.0f };
	vertexData_[12].normal = { 0.0f,  0.0f, 1.0f };
	vertexData_[13].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData_[13].texcoord = { 1.0f,1.0f };
	vertexData_[13].normal = { 0.0f,  0.0f, 1.0f };
	vertexData_[14].position = { 1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[14].texcoord = { 0.0f,0.0f };
	vertexData_[14].normal = { 0.0f,  0.0f, 1.0f };
	vertexData_[15].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[15].texcoord = { 1.0f,0.0f };
	vertexData_[15].normal = { 0.0f,  0.0f, 1.0f };
	//上
	vertexData_[16].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData_[16].texcoord = { 0.0f,1.0f };
	vertexData_[16].normal = { 0.0f,  -1.0f, 0.0f };
	vertexData_[17].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData_[17].texcoord = { 1.0f,1.0f };
	vertexData_[17].normal = { 0.0f,  -1.0f, 0.0f };
	vertexData_[18].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData_[18].texcoord = { 0.0f,0.0f };
	vertexData_[18].normal = { 0.0f,  -1.0f, 0.0f };
	vertexData_[19].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData_[19].texcoord = { 1.0f,0.0f };
	vertexData_[19].normal = { 0.0f,  -1.0f, 0.0f };
	//下
	vertexData_[20].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData_[20].texcoord = { 0.0f,1.0f };
	vertexData_[20].normal = { 0.0f,  1.0f, 0.0f };
	vertexData_[21].position = { 1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[21].texcoord = { 1.0f,1.0f };
	vertexData_[21].normal = { 0.0f,  1.0f, 0.0f };
	vertexData_[22].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexData_[22].texcoord = { 0.0f,0.0f };
	vertexData_[22].normal = { 0.0f,  1.0f, 0.0f };
	vertexData_[23].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[23].texcoord = { 1.0f,0.0f };
	vertexData_[23].normal = { 0.0f,  1.0f, 0.0f };

	for (int i = 0; i < 6; i++) {
		indexData_[3 * (i * 2)] = 0 + i * 4; indexData_[3 * (i * 2) + 1] = 1 + i * 4; indexData_[3 * (i * 2) + 2] = 2 + i * 4;
		indexData_[3 * (1 + i * 2)] = 2 + i * 4; indexData_[3 * (1 + i * 2) + 1] = 1 + i * 4; indexData_[3 * (1 + i * 2) + 2] = 3 + i * 4;
	}

	//頂点2つで直線を作る

#pragma endregion

	vertexResource_->Unmap(0, nullptr);

	indexResource_->Unmap(0, nullptr);
}

void PrimitiveBox::Draw() {
	GameEngine::DrawPrimitiveBox(this);
}