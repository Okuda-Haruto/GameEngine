#pragma once
#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>
#include <wrl.h>
#include "ModelData.h"
#include "Material.h"
#include "TransformationMatrix.h"

//3Dオブジェクト
class Object_3D {
private:
	//WVPデータ
	TransformationMatrix* wvpData_ = nullptr;
	//ロードしたモデルのデータ
	ModelData modelData_;
	//マテリアル用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	//マテリアルデータ
	Material* materialData_;
public:
	~Object_3D();
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directoryPath">.objファイルのある階層 (例:"resource")</param>
	/// <param name="filename">ファイル名 (例:"plane.obj")</param>
	/// <param name="device">デバイス</param>
	void Initialize(const std::string& directoryPath, const std::string& filename, Microsoft::WRL::ComPtr<ID3D12Device>& device);

	void Draw();
};