#pragma once
#include <string>
#include <array>
#include <list>
#include "DirectXCommon/DirectXCommon.h"
#include "SRVManager/SRVManager.h"

#include "VertexData.h"
#include "Vector2.h"
#include "Offset.h"

#include "Line.h"
#include "Plane.h"
#include <OBB.h>
#include "AABB.h"
#include "Triangle.h"
#include "Circle.h"
#include "Sphere.h"

#include <Operation/Operation.h>

using namespace std;

class Primitive3DManager {
public:
	//プリミティブで使用可能な形
	enum Primitive_SHAPE {
		SHAPE_Point,
		SHAPE_Line,
		//SHAPE_Triangle,
		//SHAPE_Plane,
		//SHAPE_Circle,
		//SHAPE_Arrow

		SHAPE_OBB,
		SHAPE_Sphere,

		SHAPE_count
	};
	//プリミティブのインスタシング用リソース
	struct PrimitiveResource {
		Offset offset;
		uint32_t instancingIndex = 0;
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
	};
	//描画可能なプリミティブの数
	static const uint32_t kMaxNumPrimitive = 1024;

	//色の要素を持つ点
	struct PrimitivePoint {
		Vector3 point;
		Vector4 color;
	};
	//色の要素を持つ直線
	struct PrimitiveLine {
		Line line;
		Vector4 color;
	};
	//色の要素を持つ三角形
	struct PrimitiveTriangle {
		Triangle triangle;
		Vector4 color;
	};
	//色の要素を持つ円
	struct PrimitiveCircle {
		Circle circle;
		Vector4 color;
	};
	//色の要素を持つOBB
	struct PrimitiveOBB {
		OBB obb;
		Vector4 color;
	};
	//色の要素を持つ球
	struct PrimitiveSphere {
		Sphere sphere;
		Vector4 color;
	};

private:

	static unique_ptr<Primitive3DManager> instance;

	DirectXCommon* dxCommon_ = nullptr;

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

	//インスタシング用リソース
	std::array<PrimitiveResource, SHAPE_count> primitiveResource_;
	//点データ
	std::array<PrimitivePoint, kMaxNumPrimitive> point_;
	int32_t pointIndex_ = 0;
	//直線データ
	std::array<PrimitiveLine, kMaxNumPrimitive> line_;
	int32_t lineIndex_ = 0;
	//三角形データ
	std::array<PrimitiveTriangle, kMaxNumPrimitive> triangle_;
	int32_t triangleIndex_ = 0;
	//円データ
	std::array<PrimitiveCircle, kMaxNumPrimitive> circle_;
	int32_t circleIndex_ = 0;
	//AABBデータ
	std::array<PrimitiveOBB, kMaxNumPrimitive> obb_;
	int32_t obbIndex_ = 0;
	//球データ
	std::array<PrimitiveSphere, kMaxNumPrimitive> sphere_;
	int32_t sphereIndex_ = 0;

public:

	Primitive3DManager() = default;
	~Primitive3DManager() = default;
	Primitive3DManager(Primitive3DManager&) = delete;
	Primitive3DManager& operator=(Primitive3DManager&) = delete;

	static Primitive3DManager* GetInstance();

	//初期化
	void Initialize(DirectXCommon* dxCommon, SRVManager* srvManager);

	void Finalize();

	void Draw();

	//直線
	void AddLine(Line line, Vector4 color = {1,0,0,1}) {
		line_[lineIndex_].line = line;
		line_[lineIndex_].color = color;
		lineIndex_++; 
	}

	//半直線
	void AddRay(Ray ray, Vector4 color = { 1,0,0,1 }) {
		Line line = {
			.origin = ray.origin,
			.diff = ray.diff 
		}; 
		AddLine(line, color); 
	}

	//線分
	void AddSegment(Segment segment, Vector4 color = { 1,0,0,1 }) {
		Line line = {
			.origin = segment.origin,
			.diff = segment.diff 
		};
		AddLine(line, color);
	}

	//点
	void AddPoint(Vector3 point, Vector4 color = { 1,0,0,1 }) {
		point_[pointIndex_].point = point;
		point_[pointIndex_].color = color;
		pointIndex_++;
	}

	//OBB
	void AddOBB(OBB obb, Vector4 color = { 1,0,0,1 }) {
		obb_[obbIndex_].obb = obb;
		obb_[obbIndex_].color = color;
		obbIndex_++;
	}

	//AABB
	void AddAABB(AABB aabb, Vector4 color = { 1,0,0,1 }) {
		OBB obb = {
			.center = {(aabb.min + aabb.max) / 2},
			.orientations = {
				{1.0f,0.0f,0.0f},
				{0.0f,1.0f,0.0f},
				{0.0f,0.0f,1.0f},
			},
			.size = {(aabb.max - aabb.min) / 2}
		};
		AddOBB(obb, color);
	}

	//球
	void AddSphere(Sphere sphere, Vector4 color = { 1,0,0,1 }) {
		sphere_[sphereIndex_].sphere = sphere;
		sphere_[sphereIndex_].color = color;
		sphereIndex_++; 
	}

	//リセット
	void Reset();

	D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return vertexBufferView_; }
	D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return indexBufferView_; }
};