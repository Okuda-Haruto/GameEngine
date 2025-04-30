#pragma once
#include <Windows.h>
#include <cstdint>
#include <fstream>

#pragma comment(lib,"dxcompiler.lib")
#include <dxcapi.h>

#include "D3DResourceLeakChecker.h"
#include "Object_3D.h"
#include "Texture.h"

class GameEngine {
private:

	//ウィンドウの幅
	int32_t kWindowWidth_;
	//ウィンドウの高さ
	int32_t kWindowHeight_;

#ifdef _DEBUG
	//リソースチェック
	D3DResourceLeakChecker leakCheck_;
#endif


	//ログファイルの生成
	std::ofstream logStream_;
	//DXGIファクトリー
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	//使用するアダプタ用変数
	Microsoft::WRL::ComPtr <IDXGIAdapter4> useAdapter_;
	//使用するデバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	//ウィンドウ
	HWND hwnd_;

	uint32_t descriptorSizeSRV_;
	uint32_t descriptorSizeRTV_;
	uint32_t descriptorSizeDSV_;

	//コマンドキュー
	Microsoft::WRL::ComPtr <ID3D12CommandQueue> commandQueue_;
	//コマンドアロケータ
	Microsoft::WRL::ComPtr <ID3D12CommandAllocator> commandAllocator_;
	//コマンドリスト
	Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList_;
	//RootSignature
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature_;

	//swapChain
	Microsoft::WRL::ComPtr <IDXGISwapChain4> swapChain_;
	//swapChainリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2];

	//RTVディスクリプタ
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	//フェンス
	Microsoft::WRL::ComPtr <ID3D12Fence> fence_ = nullptr;
	//フェンスの値
	uint64_t fenceValue_ = 0;
	//FenceのSignalを待つためのイベント
	HANDLE fenceEvent_ = nullptr;

	//インクルードハンドル
	IDxcIncludeHandler* includeHandler_ = nullptr;

	//SRV用のヒープディスクリプタ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> srvDescriptorHeap_;

	//DSV用のヒープディスクリプタ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> dsvDescriptorheap_;
	//DSV
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_{};

	//Windowのメッセージ
	MSG msg_{};
	//TransitionBarrier
	D3D12_RESOURCE_BARRIER barrier_{};

	//PSO
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState_;
	//WVP用のリソース
	Microsoft::WRL::ComPtr <ID3D12Resource> wvpResource_;
	//ビューポート
	D3D12_VIEWPORT viewport_{};
	//シザー矩形
	D3D12_RECT scissorRect_{};

	//CPUの最後尾Index
	uint32_t kLastCPUIndex_ = 0;
	//GPUの最後尾Index
	uint32_t kLastGPUIndex_ = 0;
public:
	//デストラクタ
	~GameEngine();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="WindowName">ウィンドウ名 (例:L"LE2A_00_ミョウジ_ナマエ")</param>
	/// <param name="kWindowWidth">ウィンドウの幅 (例:1280)</param>
	/// <param name="kWindowHeight">ウィンドウの高さ (例:720)</param>
	void Intialize(wchar_t* WindowName, int32_t kWindowWidth = 1280, int32_t kWindowHeight = 720);

	/// <summary>
	/// テクスチャのロード
	/// </summary>
	/// <param name="texture">ロードするTextureクラス (例:white1x1_texture)</param>
	/// <param name="filePath">テクスチャがあるフォルダへのパス (例:""resources/white1x1.png"")</param>
	void LoadTexture(Texture* texture, const std::string& filePath);

	/// <summary>
	/// 3Dオブジェクトのロード
	/// </summary>
	/// <param name="object">ロードするObject_3Dクラス</param>
	/// <param name="directoryPath">.objファイルのある階層 (例:"resource")</param>
	/// <param name="filename">ファイル名 (例:"plane.obj")</param>
	void LoadObject_3D(Object_3D* object, const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// フレームの開始
	/// </summary>
	/// <returns>Windowsのメッセージがあるか</returns>
	bool StartFlame();

	//描画前処理
	void PreDraw();

	//描画後処理
	void PostDraw();
};