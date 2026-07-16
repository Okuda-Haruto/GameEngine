#include "PrimitiveManager.h"
#include <GameEngine.h>
#include <LoadObjFile.h>
#include <SpriteManager/SpriteManager.h>
#include <numbers>

unique_ptr<PrimitiveManager> PrimitiveManager::instance;

PrimitiveManager* PrimitiveManager::GetInstance() {
	if (!instance) {
		instance = make_unique<PrimitiveManager>();
	}
	return instance.get();
}

void PrimitiveManager::Initialize(DirectXCommon* dxCommon, SRVManager* srvManager) {

	//頂点リソースを作る
	vertexResource_ = dxCommon->CreateBufferResources(sizeof(VertexData) * 4000);

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点は4つサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 2000;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//Primitive用のインデックスリソースを作る
	indexResource_ = dxCommon->CreateBufferResources(sizeof(uint32_t) * 4000);

	//インデックスバッファビューを作成する
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 4000;
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//頂点のローカル座標系を設定
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//インデックスデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

#pragma region 点

	primitiveResource_[SHAPE_Point].offset.vertexStart = 0;
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

	primitiveResource_[SHAPE_Point].offset.indexStart = 0;
	primitiveResource_[SHAPE_Point].offset.indexCount = 6;

	//点は頂点数とインデックス数が同じ
	for (UINT i = 0; i < primitiveResource_[SHAPE_Point].offset.vertexCount; i++) {
		indexData_[primitiveResource_[SHAPE_Point].offset.indexStart + i] = primitiveResource_[SHAPE_Point].offset.vertexStart + i;
	}

	//頂点2つで直線を作る

#pragma endregion

#pragma region 直線

	primitiveResource_[SHAPE_Line].offset.vertexStart = primitiveResource_[SHAPE_Point].offset.vertexStart + primitiveResource_[SHAPE_Point].offset.vertexCount;
	primitiveResource_[SHAPE_Line].offset.vertexCount = 2;
	//左下
	vertexData_[primitiveResource_[SHAPE_Line].offset.vertexStart + 0].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Line].offset.vertexStart + 0].texcoord = { 0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Line].offset.vertexStart + 0].normal = { 0.0f,0.0f,-1.0f };
	//右下
	vertexData_[primitiveResource_[SHAPE_Line].offset.vertexStart + 1].position = { 0.0f,0.0f,1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Line].offset.vertexStart + 1].texcoord = { 1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Line].offset.vertexStart + 1].normal = { 0.0f,0.0f,-1.0f };

	primitiveResource_[SHAPE_Line].offset.indexStart = primitiveResource_[SHAPE_Point].offset.indexStart + primitiveResource_[SHAPE_Point].offset.indexCount;;
	primitiveResource_[SHAPE_Line].offset.indexCount = 2;

	//直線は頂点数とインデックス数が同じ
	for (UINT i = 0; i < primitiveResource_[SHAPE_Line].offset.vertexCount; i++) {
		indexData_[primitiveResource_[SHAPE_Line].offset.indexStart + i] = primitiveResource_[SHAPE_Line].offset.vertexStart + i;
	}

#pragma endregion

#pragma region 円

	/*primitiveResource_[SHAPE_Circle].offset.vertexStart = primitiveResource_[SHAPE_Line].offset.vertexStart + primitiveResource_[SHAPE_Line].offset.vertexCount;
	primitiveResource_[SHAPE_Circle].offset.vertexCount = 24;

	//正24角形
	for (UINT i = 0; i < primitiveResource_[SHAPE_Circle].offset.vertexCount; i++) {
		//左上
		vertexData_[primitiveResource_[SHAPE_Circle].offset.vertexStart + i].position = { 
			cosf((std::numbers::pi_v<float> * 2) / primitiveResource_[SHAPE_Circle].offset.vertexCount * i),
			sinf((std::numbers::pi_v<float> * 2) / primitiveResource_[SHAPE_Circle].offset.vertexCount * i),
			0.0f,1.0f
		};
		vertexData_[primitiveResource_[SHAPE_Circle].offset.vertexStart + i].texcoord = {
			0.5f + cosf((std::numbers::pi_v<float> *2) / primitiveResource_[SHAPE_Circle].offset.vertexCount * i) / 2,
			0.5f + sinf((std::numbers::pi_v<float> *2) / primitiveResource_[SHAPE_Circle].offset.vertexCount * i) / 2
		};
		vertexData_[primitiveResource_[SHAPE_Circle].offset.vertexStart + i].normal = { 0.0f,0.0f,-1.0f };
	}

	primitiveResource_[SHAPE_Circle].offset.indexStart =  primitiveResource_[SHAPE_Line].offset.indexStart +  primitiveResource_[SHAPE_Line].offset.indexCount;
	primitiveResource_[SHAPE_Circle].offset.indexCount = primitiveResource_[SHAPE_Circle].offset.vertexCount * 2;

	//最後のインデックスだけ最初に戻す
	for (UINT i = 0; i < primitiveResource_[SHAPE_Circle].offset.vertexCount; i++) {
		indexData_[primitiveResource_[SHAPE_Circle].offset.indexStart + i * 2] = primitiveResource_[SHAPE_Circle].offset.vertexStart + i;
		indexData_[primitiveResource_[SHAPE_Circle].offset.indexStart + i * 2 + 1] = primitiveResource_[SHAPE_Circle].offset.vertexStart + (i + 1) % primitiveResource_[SHAPE_Circle].offset.vertexCount;
	}*/

#pragma endregion

#pragma region OBB

	primitiveResource_[SHAPE_OBB].offset.vertexStart = primitiveResource_[SHAPE_Line].offset.vertexStart + primitiveResource_[SHAPE_Line].offset.vertexCount;
	primitiveResource_[SHAPE_OBB].offset.vertexCount = 8;

	//左後下
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 0].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 0].texcoord = { 0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 0].normal = { 0.0f,0.0f,-1.0f };
	//左後上
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 1].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 1].texcoord = { 0.0f,0.0f };
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 1].normal = { 0.0f,0.0f,-1.0f };
	//右後上
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 2].position = { 1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 2].texcoord = { 1.0f,0.0f };
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 2].normal = { 0.0f,0.0f,-1.0f };
	//右後下
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 3].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 3].texcoord = { 1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 3].normal = { 0.0f,0.0f,-1.0f };
	//左前下
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 4].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 4].texcoord = { 0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 4].normal = { 0.0f,0.0f,-1.0f };
	//左前上
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 5].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 5].texcoord = { 0.0f,0.0f };
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 5].normal = { 0.0f,0.0f,-1.0f };
	//右前上
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 6].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 6].texcoord = { 1.0f,0.0f };
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 6].normal = { 0.0f,0.0f,-1.0f };
	//右前下
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 7].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 7].texcoord = { 1.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_OBB].offset.vertexStart + 7].normal = { 0.0f,0.0f,-1.0f };

	primitiveResource_[SHAPE_OBB].offset.indexStart = primitiveResource_[SHAPE_Line].offset.indexStart + primitiveResource_[SHAPE_Line].offset.indexCount;
	primitiveResource_[SHAPE_OBB].offset.indexCount = 24;

	//頂点2つで線分を作る
	for (int i = 0; i < 4; i++) {
		indexData_[primitiveResource_[SHAPE_OBB].offset.indexStart + i * 2] = primitiveResource_[SHAPE_OBB].offset.vertexStart + i; 
		indexData_[primitiveResource_[SHAPE_OBB].offset.indexStart + i * 2 + 1] = primitiveResource_[SHAPE_OBB].offset.vertexStart + (i + 1) % 4;

		indexData_[primitiveResource_[SHAPE_OBB].offset.indexStart + i * 2 + 8] = primitiveResource_[SHAPE_OBB].offset.vertexStart + i + 4;
		indexData_[primitiveResource_[SHAPE_OBB].offset.indexStart + i * 2 + 9] = primitiveResource_[SHAPE_OBB].offset.vertexStart + (i + 1) % 4 + 4;

		indexData_[primitiveResource_[SHAPE_OBB].offset.indexStart + i * 2 + 16] = primitiveResource_[SHAPE_OBB].offset.vertexStart + i;
		indexData_[primitiveResource_[SHAPE_OBB].offset.indexStart + i * 2 + 17] = primitiveResource_[SHAPE_OBB].offset.vertexStart + i + 4;
	}
	
#pragma endregion

#pragma region 球
	
	//縦方向の分割数
	const int ringNum = 16;
	//横方向の分割数
	const int segmentNum = 32;

	primitiveResource_[SHAPE_Sphere].offset.vertexStart = primitiveResource_[SHAPE_OBB].offset.vertexStart + primitiveResource_[SHAPE_OBB].offset.vertexCount;
	primitiveResource_[SHAPE_Sphere].offset.vertexCount = ringNum * segmentNum + 2;

	//上下最大値は半径0なので複数作らない
	const int topVertexNum = 0;
	const int bottomVertexNum = 1;

	//y = 1の頂点
	vertexData_[primitiveResource_[SHAPE_Sphere].offset.vertexStart + topVertexNum].position = { 0.0f,1.0f,0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Sphere].offset.vertexStart + topVertexNum].texcoord = { 0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Sphere].offset.vertexStart + topVertexNum].normal = { 0.0f,0.0f,-1.0f };

	//y = -1の頂点
	vertexData_[primitiveResource_[SHAPE_Sphere].offset.vertexStart + bottomVertexNum].position = { 0.0f,-1.0f,0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Sphere].offset.vertexStart + bottomVertexNum].texcoord = { 0.0f,1.0f };
	vertexData_[primitiveResource_[SHAPE_Sphere].offset.vertexStart + bottomVertexNum].normal = { 0.0f,0.0f,-1.0f };

	//角度
	const float ringAngle = std::numbers::pi_v<float> / ringNum;
	const float segmentAngle = (std::numbers::pi_v<float> * 2) / segmentNum;

	//球の断面の円を回る頂点
	for (int y = 1; y <= ringNum - 1; y++) {
		for (int x = 0; x < segmentNum; x++) {
			vertexData_[primitiveResource_[SHAPE_Sphere].offset.vertexStart + (y - 1) * segmentNum + x + 2].position = {
				cosf(segmentAngle * x) * sinf(ringAngle * y),
				cosf(ringAngle * y),
				sinf(segmentAngle * x)* sinf(ringAngle * y),
				1.0f };
			vertexData_[primitiveResource_[SHAPE_Sphere].offset.vertexStart + (y - 1) * segmentNum + x + 2].texcoord = { 0.0f,1.0f };
			vertexData_[primitiveResource_[SHAPE_Sphere].offset.vertexStart + (y - 1) * segmentNum + x + 2].normal = { 0.0f,0.0f,-1.0f };
		}
	}

	primitiveResource_[SHAPE_Sphere].offset.indexStart = primitiveResource_[SHAPE_OBB].offset.indexStart + primitiveResource_[SHAPE_OBB].offset.indexCount;

	int verticalLines = ringNum * segmentNum;
	int horizontalLines = (ringNum - 1) * segmentNum;
	primitiveResource_[SHAPE_Sphere].offset.indexCount =(verticalLines + horizontalLines) * 2;

	//y = 1から伸びる線
	for (int x = 0; x < segmentNum; x++) {
		indexData_[primitiveResource_[SHAPE_Sphere].offset.indexStart + x * 2] = primitiveResource_[SHAPE_Sphere].offset.vertexStart + topVertexNum;
		indexData_[primitiveResource_[SHAPE_Sphere].offset.indexStart + x * 2 + 1] = primitiveResource_[SHAPE_Sphere].offset.vertexStart + x + 2;
	}
	//下に伸ばす線
	for (int y = 1; y <= ringNum - 2; y++) {
		for (int x = 0; x < segmentNum; x++) {
			indexData_[primitiveResource_[SHAPE_Sphere].offset.indexStart + y * (segmentNum * 2) + x * 2] = primitiveResource_[SHAPE_Sphere].offset.vertexStart + (y - 1) * segmentNum + x + 2;
			indexData_[primitiveResource_[SHAPE_Sphere].offset.indexStart + y * (segmentNum * 2) + x * 2 + 1] = primitiveResource_[SHAPE_Sphere].offset.vertexStart + y * segmentNum + x + 2;
		}
	}
	//y = -1に伸びる線
	for (int x = 0; x < segmentNum; x++) {
		indexData_[primitiveResource_[SHAPE_Sphere].offset.indexStart + (ringNum - 1) * (segmentNum * 2) + x * 2] = primitiveResource_[SHAPE_Sphere].offset.vertexStart + (ringNum - 2) * segmentNum + x + 2;
		indexData_[primitiveResource_[SHAPE_Sphere].offset.indexStart + (ringNum - 1) * (segmentNum * 2) + x * 2 + 1] = primitiveResource_[SHAPE_Sphere].offset.vertexStart + bottomVertexNum;
	}

	//横に伸ばす線
	for (int y = 1; y <= ringNum - 1; y++) {
		for (int x = 0; x < segmentNum; x++) {
			indexData_[primitiveResource_[SHAPE_Sphere].offset.indexStart + (ringNum - 1 + y) * (segmentNum * 2) + x * 2] = primitiveResource_[SHAPE_Sphere].offset.vertexStart + (y - 1) * segmentNum + x + 2;
			indexData_[primitiveResource_[SHAPE_Sphere].offset.indexStart + (ringNum - 1 + y) * (segmentNum * 2) + x * 2 + 1] = primitiveResource_[SHAPE_Sphere].offset.vertexStart + (y - 1) * segmentNum + (x + 1) % segmentNum + 2;
		}
	}


#pragma endregion

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
	std::list<PrimitiveLine> lines;
	for (int i = 0; i < lineIndex_; i++) {
		lines.push_back(line_[i]);
	}
	if (!lines.empty())GameEngine::DrawLine(lines,primitiveResource_[SHAPE_Line]);

	std::list<PrimitivePoint> points;
	for (int i = 0; i < pointIndex_; i++) {
		points.push_back(point_[i]);
	}
	if (!points.empty())GameEngine::DrawPoint(points, primitiveResource_[SHAPE_Point]);

	std::list<PrimitiveOBB> obbs;
	for (int i = 0; i < obbIndex_; i++) {
		obbs.push_back(obb_[i]);
	}
	if (!obbs.empty())GameEngine::DrawOBB(obbs, primitiveResource_[SHAPE_OBB]);

	std::list<PrimitiveSphere> spheres;
	for (int i = 0; i < sphereIndex_; i++) {
		spheres.push_back(sphere_[i]);
	}
	if (!spheres.empty())GameEngine::DrawSphere(spheres, primitiveResource_[SHAPE_Sphere]);

	Reset();
}

void PrimitiveManager::Reset() {
	lineIndex_ = 0;
	triangleIndex_ = 0;
	pointIndex_ = 0;
	obbIndex_ = 0;
	sphereIndex_ = 0;
}