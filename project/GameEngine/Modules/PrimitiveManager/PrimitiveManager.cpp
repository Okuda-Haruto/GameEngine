#include "PrimitiveManager.h"
#include <GameEngine.h>
#include <LoadObjFile.h>
#include <SpriteManager/SpriteManager.h>

unique_ptr<PrimitiveManager> PrimitiveManager::instance;

PrimitiveManager* PrimitiveManager::GetInstance() {
	if (!instance) {
		instance = make_unique<PrimitiveManager>();
	}
	return instance.get();
}

void PrimitiveManager::Initialize(DirectXCommon* dxCommon, SRVManager* srvManager) {

	//頂点リソースを作る
	vertexResource_ = dxCommon->CreateBufferResources(sizeof(VertexData) * 20);

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点は4つサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 20;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//Primitive用のインデックスリソースを作る
	indexResource_ = dxCommon->CreateBufferResources(sizeof(uint32_t) * 40);

	//インデックスバッファビューを作成する
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 40;
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//頂点のローカル座標系を設定
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//インデックスデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	//	線分
	primitiveResource_[SHAPE_Line].offset.vertexStart = 0;
	primitiveResource_[SHAPE_Line].offset.vertexCount = 2;
	//左下
	vertexData_[primitiveResource_[SHAPE_Line].offset.vertexStart + 0].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Line].offset.vertexStart + 0].texcoord = { 0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Line].offset.vertexStart + 0].normal = { 0.0f,0.0f,-1.0f };
	//右下
	vertexData_[primitiveResource_[SHAPE_Line].offset.vertexStart + 1].position = { 0.0f,0.0f,1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Line].offset.vertexStart + 1].texcoord = { 1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Line].offset.vertexStart + 1].normal = { 0.0f,0.0f,-1.0f };

	primitiveResource_[SHAPE_Line].offset.indexStart = 0;
	primitiveResource_[SHAPE_Line].offset.indexCount = 2;

	//頂点2つで線分を作る
	indexData_[primitiveResource_[SHAPE_Line].offset.indexStart + 0] = 0; indexData_[primitiveResource_[SHAPE_Line].offset.indexStart + 1] = 1;

	//	平面
	primitiveResource_[SHAPE_Plane].offset.vertexStart = primitiveResource_[SHAPE_Line].offset.vertexStart + primitiveResource_[SHAPE_Line].offset.vertexCount;
	primitiveResource_[SHAPE_Plane].offset.vertexCount = 4;
	//左上
	vertexData_[primitiveResource_[SHAPE_Plane].offset.vertexStart + 0].position = { 0.0f,1.0f,0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Plane].offset.vertexStart + 0].texcoord = { 0.0f,0.0f };
	vertexData_[primitiveResource_[SHAPE_Plane].offset.vertexStart + 0].normal = { 0.0f,0.0f,-1.0f };
	//左下
	vertexData_[primitiveResource_[SHAPE_Plane].offset.vertexStart + 1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Plane].offset.vertexStart + 1].texcoord = { 0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Plane].offset.vertexStart + 1].normal = { 0.0f,0.0f,-1.0f };
	//右下
	vertexData_[primitiveResource_[SHAPE_Plane].offset.vertexStart + 2].position = { 0.0f,0.0f,1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Plane].offset.vertexStart + 2].texcoord = { 1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Plane].offset.vertexStart + 2].normal = { 0.0f,0.0f,-1.0f };
	//右上
	vertexData_[primitiveResource_[SHAPE_Plane].offset.vertexStart + 3].position = { 1.0f,1.0f,0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Plane].offset.vertexStart + 3].texcoord = { 1.0f,0.0f };
	vertexData_[primitiveResource_[SHAPE_Plane].offset.vertexStart + 3].normal = { 0.0f,0.0f,-1.0f };

	primitiveResource_[SHAPE_Plane].offset.indexStart =  primitiveResource_[SHAPE_Line].offset.indexStart +  primitiveResource_[SHAPE_Line].offset.indexCount;
	primitiveResource_[SHAPE_Plane].offset.indexCount = 8;

	//頂点2つで線分を作る
	indexData_[primitiveResource_[SHAPE_Plane].offset.indexStart + 0] = primitiveResource_[SHAPE_Plane].offset.vertexStart + 0; indexData_[primitiveResource_[SHAPE_Plane].offset.indexStart + 1] = primitiveResource_[SHAPE_Plane].offset.vertexStart + 1;
	indexData_[primitiveResource_[SHAPE_Plane].offset.indexStart + 2] = primitiveResource_[SHAPE_Plane].offset.vertexStart + 1; indexData_[primitiveResource_[SHAPE_Plane].offset.indexStart + 3] = primitiveResource_[SHAPE_Plane].offset.vertexStart + 2;
	indexData_[primitiveResource_[SHAPE_Plane].offset.indexStart + 4] = primitiveResource_[SHAPE_Plane].offset.vertexStart + 2; indexData_[primitiveResource_[SHAPE_Plane].offset.indexStart + 5] = primitiveResource_[SHAPE_Plane].offset.vertexStart + 3;
	indexData_[primitiveResource_[SHAPE_Plane].offset.indexStart + 6] = primitiveResource_[SHAPE_Plane].offset.vertexStart + 3; indexData_[primitiveResource_[SHAPE_Plane].offset.indexStart + 7] = primitiveResource_[SHAPE_Plane].offset.vertexStart + 0;

	//	点
	primitiveResource_[SHAPE_Point].offset.vertexStart = primitiveResource_[SHAPE_Plane].offset.vertexStart + primitiveResource_[SHAPE_Plane].offset.vertexCount;
	primitiveResource_[SHAPE_Point].offset.vertexCount = 6;
	//左
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 0].position = { -0.05f,0.0f,0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 0].texcoord = { 0.0f,0.0f };
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 0].normal = { 0.0f,0.0f,-1.0f };
	//右
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 1].position = { 0.05f,0.0f,0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 1].texcoord = { 1.0f,0.0f };
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 1].normal = { 0.0f,0.0f,-1.0f };
	//下
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 2].position = { 0.0f,-0.05f,0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 2].texcoord = { 0.0f,0.0f };
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 2].normal = { 0.0f,0.0f,-1.0f };
	//上
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 3].position = { 0.0f,0.05f,0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 3].texcoord = { 0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 3].normal = { 0.0f,0.0f,-1.0f };
	//後
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 4].position = { 0.0f,0.0f,-0.05f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 4].texcoord = { 0.0f,0.0f };
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 4].normal = { 0.0f,0.0f,-1.0f };
	//前
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 5].position = { 0.0f,0.0f,0.05f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 5].texcoord = { 1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Point].offset.vertexStart + 5].normal = { 0.0f,0.0f,-1.0f };

	primitiveResource_[SHAPE_Point].offset.indexStart = primitiveResource_[SHAPE_Plane].offset.indexStart + primitiveResource_[SHAPE_Plane].offset.indexCount;
	primitiveResource_[SHAPE_Point].offset.indexCount = 6;

	//頂点2つで線分を作る
	indexData_[primitiveResource_[SHAPE_Point].offset.indexStart + 0] = primitiveResource_[SHAPE_Point].offset.vertexStart + 0; indexData_[primitiveResource_[SHAPE_Point].offset.indexStart + 1] = primitiveResource_[SHAPE_Point].offset.vertexStart + 1;
	indexData_[primitiveResource_[SHAPE_Point].offset.indexStart + 2] = primitiveResource_[SHAPE_Point].offset.vertexStart + 2; indexData_[primitiveResource_[SHAPE_Point].offset.indexStart + 3] = primitiveResource_[SHAPE_Point].offset.vertexStart + 3;
	indexData_[primitiveResource_[SHAPE_Point].offset.indexStart + 4] = primitiveResource_[SHAPE_Point].offset.vertexStart + 4; indexData_[primitiveResource_[SHAPE_Point].offset.indexStart + 5] = primitiveResource_[SHAPE_Point].offset.vertexStart + 5;

	//	AABB
	primitiveResource_[SHAPE_AABB].offset.vertexStart = primitiveResource_[SHAPE_Point].offset.vertexStart + primitiveResource_[SHAPE_Point].offset.vertexCount;
	primitiveResource_[SHAPE_AABB].offset.vertexCount = 8;
	//左後下
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 0].position = { 0.0f,1.0f,0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 0].texcoord = { 0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 0].normal = { 0.0f,0.0f,-1.0f };
	//左後上
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 1].texcoord = { 0.0f,0.0f };
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 1].normal = { 0.0f,0.0f,-1.0f };
	//右後上
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 2].position = { 1.0f,0.0f,0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 2].texcoord = { 1.0f,0.0f };
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 2].normal = { 0.0f,0.0f,-1.0f };
	//右後下
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 3].position = { 1.0f,1.0f,0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 3].texcoord = { 1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 3].normal = { 0.0f,0.0f,-1.0f };
	//左前下
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 4].position = { 0.0f,1.0f,1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 4].texcoord = { 0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 4].normal = { 0.0f,0.0f,-1.0f };
	//左前上
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 5].position = { 0.0f,0.0f,1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 5].texcoord = { 0.0f,0.0f };
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 5].normal = { 0.0f,0.0f,-1.0f };
	//右前上
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 6].position = { 1.0f,0.0f,1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 6].texcoord = { 1.0f,0.0f };
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 6].normal = { 0.0f,0.0f,-1.0f };
	//右前下
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 7].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 7].texcoord = { 1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_AABB].offset.vertexStart + 7].normal = { 0.0f,0.0f,-1.0f };

	primitiveResource_[SHAPE_AABB].offset.indexStart = primitiveResource_[SHAPE_Point].offset.indexStart + primitiveResource_[SHAPE_Point].offset.indexCount;
	primitiveResource_[SHAPE_AABB].offset.indexCount = 24;

	//頂点2つで線分を作る
	indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 0] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 0; indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 1] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 1;
	indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 2] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 1; indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 3] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 2;
	indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 4] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 2; indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 5] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 3;
	indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 6] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 3; indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 7] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 0;

	indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 8]  = primitiveResource_[SHAPE_AABB].offset.vertexStart + 4; indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 9]  = primitiveResource_[SHAPE_AABB].offset.vertexStart + 5;
	indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 10] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 5; indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 11] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 6;
	indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 12] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 6; indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 13] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 7;
	indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 14] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 7; indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 15] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 4;

	indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 16] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 0; indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 17] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 4;
	indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 18] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 1; indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 19] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 5;
	indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 20] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 2; indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 21] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 6;
	indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 22] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 3; indexData_[primitiveResource_[SHAPE_AABB].offset.indexStart + 23] = primitiveResource_[SHAPE_AABB].offset.vertexStart + 7;

	vertexResource_->Unmap(0, nullptr);

	indexResource_->Unmap(0, nullptr);

	//それぞれのリソース確保
	for (int i = 0; i < SHAPE_count; i++) {
		// instancing 用リソースを maxInstances 分確保する
		primitiveResource_[i].instancingResource = dxCommon->CreateBufferResources(sizeof(InstancingTransformationMatrix) * kMaxNumPrimitive);
		primitiveResource_[i].instancingIndex = srvManager->Allocate();
		// SRV を作成（NumElements と stride は一致させる）
		srvManager->CreateSRVforStructuredBuffer(primitiveResource_[i].instancingIndex, primitiveResource_[i].instancingResource.Get(), kMaxNumPrimitive, sizeof(InstancingTransformationMatrix));
	}

	Reset();
}

void PrimitiveManager::Finalize() {
	instance.reset();
}

void PrimitiveManager::Draw() {
	std::list<Line> lines;
	for (int i = 0; i < lineIndex_; i++) {
		lines.push_back(line_[i]);
	}
	if (!lines.empty())GameEngine::DrawLine(lines,primitiveResource_[SHAPE_Line]);

	std::list<Vector3> points;
	for (int i = 0; i < pointIndex_; i++) {
		points.push_back(point_[i]);
	}
	if (!points.empty())GameEngine::DrawPoint(points, primitiveResource_[SHAPE_Point]);

	std::list<AABB> aabbs;
	for (int i = 0; i < aabbIndex_; i++) {
		aabbs.push_back(aabb_[i]);
	}
	if (!aabbs.empty())GameEngine::DrawAABB(aabbs, primitiveResource_[SHAPE_AABB]);

	Reset();
}

void PrimitiveManager::Reset() {
	lineIndex_ = 0;
	planeIndex_ = 0;
	pointIndex_ = 0;
	aabbIndex_ = 0;
}