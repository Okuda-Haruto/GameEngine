#pragma once

#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>

#include <wrl.h>
#include "ModelData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "ObjectData.h"
#include "Texture.h"
#include "Text.h"

#pragma region Sprite_2D

//2Dスプライト
class Sprite_2D {
private:
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

	//マテリアルリソース
	std::vector <Microsoft::WRL::ComPtr<ID3D12Resource>> materialResource_;
	//マテリアルデータ
	std::vector <Material*> materialData_;
	//WVP用リソース
	std::vector <Microsoft::WRL::ComPtr<ID3D12Resource>> wvpResource_;
	//WVPデータ
	std::vector <TransformationMatrix*> wvpData_;

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;

	//Windowのサイズ
	uint32_t kWindowWidth_ = 1280;
	uint32_t kWindowHeight_ = 720;

	//Spriteの表示サイズ
	float spriteWidth_ = 640.0f;
	float spriteHeight_ = 360.0f;

	//テクスチャデータ
	Texture* texture_ = nullptr;

	//リソース番号の最大値
	static const int kMaxIndex_ = 128;
	//使用するリソースの番号
	int index_ = 0;

public:

	~Sprite_2D();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="kWindowWidth">ウィンドウの幅</param>
	/// <param name="kWindowHeight">ウィンドウの高さ</param>
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t kWindowWidth, uint32_t kWindowHeight);

	// Texture入力
	void SetTexture(Texture* texture) { texture_ = texture; };
	// スプライトの表示サイズ入力
	void SetSpriteSize(float width, float height) { spriteWidth_ = width; spriteHeight_ = height;};

	/// <summary>
	/// 2Dオブジェクト描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	void Draw(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& commandList, Object_2D_Data& data);
	//リソース初期化
	void Reset();

};

#pragma endregion

#pragma region Text_2D

//2Dテキスト
class Text_2D {
private:
	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	//頂点バッファビュー
	std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferView_{};
	//頂点リソースデータ
	VertexData* vertexData_ = nullptr;
	//インデックスリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	//インデックスデータ
	uint32_t* indexData_ = nullptr;

	//マテリアルリソース
	std::vector <Microsoft::WRL::ComPtr<ID3D12Resource>> materialResource_;
	//マテリアルデータ
	std::vector <Material*> materialData_;
	//WVP用リソース
	std::vector <Microsoft::WRL::ComPtr<ID3D12Resource>> wvpResource_;
	//WVPデータ
	std::vector <TransformationMatrix*> wvpData_;

	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;

	//Windowのサイズ
	uint32_t kWindowWidth_;
	uint32_t kWindowHeight_;

	//Spriteの表示サイズ
	float spriteWidth_ = 640.0f;
	float spriteHeight_ = 360.0f;

	//テキスト
	std::vector<TextData> textData_;

	//リソース番号の最大値
	static const int kMaxIndex_ = 128;
	//使用するリソースの番号
	int index_ = 0;

public:

	~Text_2D();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="kWindowWidth">ウィンドウの幅</param>
	/// <param name="kWindowHeight">ウィンドウの高さ</param>
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t kWindowWidth, uint32_t kWindowHeight);

	// Text入力
	void SetText(std::vector<TextData> textData) { textData_ = textData; };
	// スプライトの表示サイズ入力
	void SetSpriteSize(float width, float height) { spriteWidth_ = width; spriteHeight_ = height; };

	/// <summary>
	/// 2Dテキスト描画
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="data">オブジェクトの各種データ</param>
	void Draw(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& commandList, Object_2D_Data& data);

	/// <summary>
	/// 2Dテキスト描画 (描画範囲指定)
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="data">オブジェクトの各種データ</param>
	/// <param name="index">テキストの描画範囲</param>
	void Draw(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& commandList, Object_2D_Data& data, int index);

	//リソース初期化
	void Reset();

};

#pragma endregion