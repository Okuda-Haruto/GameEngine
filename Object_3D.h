#pragma once
#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>
#include <wrl.h>
#include "ModelData.h"
#include "Material.h"

//3Dオブジェクト
class Object_3D {
private:
	//ロードしたモデルのデータ
	ModelData modelData;
	//
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	//
	Material* materialData;
public:

};