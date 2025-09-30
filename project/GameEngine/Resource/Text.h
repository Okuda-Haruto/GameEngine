#pragma once
#include <Windows.h>
#include <Wingdi.h>
#include <wrl.h>
#include <string>
#include "Vector2.h"
#include <vector>

#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>

#include "DirectXTex/DirectXTex.h"
#include <format>

//1文字描画するのに必要なデータ
struct TextData {
	//文字のサイズ
	Vector2 textSize{};
	//文字の描画開始位置
	Vector2 textOrigin{};
	//テクスチャのGPUデスクリプタハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
	//テクスチャリソース
	Microsoft::WRL::ComPtr <ID3D12Resource> textureResource_;
	//この文字は空白か
	bool isBlank = false;
	//この文字は改行か
	bool isLineBreak = false;
};

//テクスチャ
class Text {
private:
	//論理フォント
	HFONT hFont_;
	//元のフォントの状態
	HFONT oldFont_;
	//ウィンドウハンドル
	HWND hwnd_;
	//デバイスコンテキスト
	HDC hdc_;
	//文字のデータ
	std::vector<TextData> textData_;

	//フォントファイルへのパス
	std::wstring filePath_;
	//使用するフォント
	LOGFONTW lf_;

	uint8_t R_ = 0xFF;
	uint8_t G_ = 0xFF;
	uint8_t B_ = 0xFF;

	int length_ = 0;

public:
	~Text();

	//初期化
	void Initialize(LONG fontSize, LONG fontWeight, const std::string& directoryPath, const std::string& filename, HWND hwnd);

	//文字の入力	コマンドリストを使用するために必要な物はGameEngineから引っ張ってくること
	void GetTextData(wchar_t text,
		Microsoft::WRL::ComPtr <ID3D12Device> device,
		Microsoft::WRL::ComPtr <ID3D12CommandQueue>& commandQueue,
		Microsoft::WRL::ComPtr <ID3D12CommandAllocator>& commandAllocator,
		Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& commandList,
		Microsoft::WRL::ComPtr <ID3D12Fence>& fence,
		uint64_t& fenceValue,
		HANDLE& fenceEvent,
		Microsoft::WRL::ComPtr <ID3D12DescriptorHeap>& srvDescriptorHeap,
		uint32_t descriptorSizeSRV,
		uint32_t& CPUIndex,
		uint32_t& GPUIndex);
	
	//Color入力
	void SetColor(uint8_t R, uint8_t G, uint8_t B);

	//文字列長の出力
	int GetTextlength() { return length_; };

	//LOGFONTW出力
	LOGFONTW GetLogfont() { return lf_; }
	//LOGFONTW入力
	void SetLogfont(LOGFONTW lf);

	//描画するテキストのデータ
	[[nodiscard]]
	std::vector<TextData> textData() { return textData_; }
};
