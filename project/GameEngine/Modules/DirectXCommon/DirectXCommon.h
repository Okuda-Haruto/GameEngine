#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <wrl.h>
#include <array>
#include <chrono>
#include <vector>

#include "DirectXTex/DirectXTex.h"

#include <Log.h>

#include <WindowsAPI/WindowsAPI.h>
#include <Vector4.h>

class SRVManager;

//DirectX基盤
class DirectXCommon {
public:
	//スワップチェーンで使うRTVデスクリプタサイズ
	static const uint32_t kSwapChainDescriptorSize_ = 2;
	//オフスクリーンレンダリングで使うRTVデスクリプタサイズ
	static const uint32_t kOffscreenDescriptorSize_ = 1;
	//全体RTVデスクリプタサイズ
	static const uint32_t kRTVHandleSize_ = 3;
private:
	//WindowsAPI
	WindowsAPI* winApp_ = nullptr;

	//ログファイル
	std::ofstream logStream_;
	//DXGIファクトリー
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	//使用するアダプタ用変数
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;
	//使用するデバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_;

	//コマンドアロケータ
	Microsoft::WRL::ComPtr <ID3D12CommandAllocator> commandAllocator_;
	//コマンドリスト
	Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList_;
	//コマンドキュー
	Microsoft::WRL::ComPtr <ID3D12CommandQueue> commandQueue_;

	//swapChain
	Microsoft::WRL::ComPtr <IDXGISwapChain4> swapChain_;
	//swapChainのバッファ
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	//swapChainリソース
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources_;

	//DepthStencilTexture
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
	//深度描画用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> depthWriteTextureResource_;
	//深度バッファ位置
	uint32_t depthBufferIndex_;

	//デスクリプタサイズ
	uint32_t descriptorSizeRTV_;
	uint32_t descriptorSizeDSV_;

	//RTV用のヒープディスクリプタ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> rtvDescriptorHeap_;
	//RTVのバッファ
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	//RTVディスクリプタ
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[kRTVHandleSize_];

	//DSV用のヒープディスクリプタ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> dsvDescriptorheap_;

	//DSV
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_{};

	//SRVManager
	SRVManager* srvManager_ = nullptr;

	//フェンス
	Microsoft::WRL::ComPtr <ID3D12Fence> fence_;
	//フェンスの値
	uint64_t fenceValue_ = 0;
	//FenceのSignalを待つためのイベント
	HANDLE fenceEvent_ = nullptr;

	//ビューポート
	D3D12_VIEWPORT viewport_{};
	//シザー矩形
	D3D12_RECT scissorRect_{};

	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	//インクルードハンドル
	Microsoft::WRL::ComPtr <IDxcIncludeHandler> includeHandler_ = nullptr;

	//TransitionBarrier
	D3D12_RESOURCE_BARRIER barrier_{};

	//1fあたりの経過時間
	float deltaTime_;

public:

	~DirectXCommon();

	//初期化
	void Initialize(WindowsAPI* winApp);

	//描画前処理
	void PreDraw();
	//描画後処理
	void PostDraw();

	//描画前処理
	void RenderPreDraw(std::string textureName);
	//描画後処理
	void RenderPostDraw();

	//RootSignature作成
	Microsoft::WRL::ComPtr <ID3D12RootSignature> Object_RootSignatureInitialvalue();
	Microsoft::WRL::ComPtr <ID3D12RootSignature> Object_Instancing_RootSignatureInitialvalue();
	Microsoft::WRL::ComPtr <ID3D12RootSignature> Sprite_RootSignatureInitialvalue();
	Microsoft::WRL::ComPtr <ID3D12RootSignature> Particle_RootSignatureInitialvalue();
	Microsoft::WRL::ComPtr <ID3D12RootSignature> Fog_RootSignatureInitialvalue();
	Microsoft::WRL::ComPtr <ID3D12RootSignature> Screen_RootSignatureInitialvalue();
	Microsoft::WRL::ComPtr <ID3D12RootSignature> Screen_ColorChange_RootSignatureInitialvalue();

	//シェーダーのコンパイル
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath,const wchar_t* profile);
	//バッファリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResources(size_t sizeInBytes);
	//テクスチャリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);
	//レンダーテクスチャリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(uint32_t width,uint32_t height,DXGI_FORMAT format, const Vector4 clearColor);
	//テクスチャデータの転送
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	//デスクリプタヒープの生成
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	ID3D12Device* GetDevice()const { return device_.Get(); };
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }

	UINT SwapChainBufferCount() { return swapChainDesc_.BufferCount; }
	DXGI_FORMAT GetRTVFormat() { return rtvDesc_.Format; }

	float GetDeltaTime() { return deltaTime_; }

	//SRVManagerはDirectXCommonの後に初期化しないといけないので分ける
	void DepthBufferInitialize(SRVManager* srvManager);
	//深度バッファSRVをセット
	void SetDepthTexture(UINT RootParameterIndex);

private:
	//ログファイルの生成
	void LogInitilaize();
	//使用するアダプターの初期値
	void useAdapterInitialvalue();
	//使用するデバイスの初期値
	void deviceInitialvalue();
	//使用するコマンドアロケータの初期値
	void commandAllocatorInitialvalue();
	//使用するコマンドリストの初期値
	void commandListInitialvalue();
	//使用するコマンドキューの初期値
	void commandQueueInitialvalue();
	//使用するスワップチェーンの初期値
	void swapChainInitialize();
	//深度バッファの生成
	void CreateDepthStencilTextureResource();
	//深度描画テクスチャの生成
	void CreateDepthWriteTextureResource();
	//各種デスクリプタヒープの生成
	void descriptorHeapInitialize();
	//レンダーターゲットビューの初期値
	void RenderTargetViewInitialize();
	//指定番号のCPUデスクリプタハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
	//指定番号のGPUデスクリプタハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
	//深度ステンシルビューの初期値
	void depthStencilInitialize();
	//フェンスの初期値
	void fenceInitialize();
	//ビューポート矩形の初期値
	void viewportInitilaize();
	//シザリング矩形の初期値
	void scissorRectInitialize();
	//DXCコンパイラの生成
	void dxcCompilerInitialize();

	//FPS固定初期化
	void InitializeFixFPS();
	//FPS固定更新
	void UpdateFixFPS();
	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;

	//コマンドキュー実行
	void ExecuteCommandQueue();
};