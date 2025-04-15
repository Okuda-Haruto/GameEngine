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
	ModelData modelData_;
	//マテリアル用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	//マテリアルデータ
	Material* materialData_;
public:
	Object_3D(Microsoft::WRL::ComPtr<ID3D12Device>& device);
};