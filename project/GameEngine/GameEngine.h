#pragma once
#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include <dxcapi.h>

#define DIRECTINPUT_VERSION 0x0800	//DirectInputのバージョン指定
#include <dinput.h>

#include <Xinput.h>
#include <random>

#include "D3DResourceLeakChecker.h"
#include "DirectionalLight.h"
#include "Object_3D.h"
#include "Object_2D.h"
#include "Text.h"
#include "Audio.h"
#include "Input/Input.h"
#include "DebugCamera.h"

#include "TextureData.h"
#include "ParticleForGPU.h"

#include <vector>

class GameEngine {
private:

	//ウィンドウの幅
	static int32_t kWindowWidth_;
	//ウィンドウの高さ
	static int32_t kWindowHeight_;

#ifdef _DEBUG
	//リソースチェック
	D3DResourceLeakChecker leakCheck_;
#endif
	//ウィンドウクラス
	WNDCLASS w_;

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
	Microsoft::WRL::ComPtr <ID3D12RootSignature> instancingRootSignature_;

	//swapChain
	Microsoft::WRL::ComPtr <IDXGISwapChain4> swapChain_;
	//swapChainのバッファ
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	//swapChainリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2] = {nullptr};

	//RTVのバッファ
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	//RTVディスクリプタ
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	//フェンス
	Microsoft::WRL::ComPtr <ID3D12Fence> fence_ = nullptr;
	//フェンスの値
	uint64_t fenceValue_ = 0;
	//FenceのSignalを待つためのイベント
	HANDLE fenceEvent_ = nullptr;

	//インクルードハンドル
	Microsoft::WRL::ComPtr <IDxcIncludeHandler> includeHandler_ = nullptr;

	//DepthStencilTexture
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;

	//SRV用のヒープディスクリプタ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> srvDescriptorHeap_;
	//RTV用のヒープディスクリプタ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> rtvDescriptorHeap_;
	//DSV用のヒープディスクリプタ
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> dsvDescriptorheap_;
	//DSV
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_{};

	//Windowのメッセージ
	MSG msg_{};
	//TransitionBarrier
	D3D12_RESOURCE_BARRIER barrier_{};

	//PSO
	Microsoft::WRL::ComPtr <ID3D12PipelineState> trianglePipelineState_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> instancingTrianglePipelineState_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> particlePipelineState_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> linePipelineState_ = nullptr;
	//WVP用のリソース
	Microsoft::WRL::ComPtr <ID3D12Resource> wvpResource_;

	//ビューポート
	D3D12_VIEWPORT viewport_{};
	//シザー矩形
	D3D12_RECT scissorRect_{};

	//XAudio2インスタンス
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	//オーディオ宛先
	IXAudio2MasteringVoice* masterVoice_ = nullptr;

	//DirectInput
	IDirectInput8* directInput_ = nullptr;
	//キーボードデバイス
	IDirectInputDevice8* keyboardDevice_ = nullptr;
	//マウスデバイス
	IDirectInputDevice8* mouseDevice_ = nullptr;

	std::mt19937 randomEngine_;

#pragma region 入力関係

	//キー入力
	BYTE keys_[256]{};
	BYTE preKeys_[256]{};

	//マウス入力
	DIMOUSESTATE preMouse_;
	DIMOUSESTATE mouse_;

	//パッド入力
	XINPUT_STATE pad_[4];
	DWORD dwResult_[4];
	XINPUT_STATE prePad_[4];
#pragma endregion

	//テクスチャデータ
	std::vector<TextureData> textureData_;

	//コンストラクタ
	GameEngine();
	// デストラクタ
	~GameEngine();

	void Intialize_(const wchar_t* WindowName, int32_t kWindowWidth = 1280, int32_t kWindowHeight = 720);

	D3D12_GPU_DESCRIPTOR_HANDLE GetInstancingSRV_(Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource, int32_t numInstance);

	static const UINT textureStart = 2;
	UINT TextureLoad_(const std::string& filePath);
	D3D12_GPU_DESCRIPTOR_HANDLE TextureGet_(UINT index);
	void TextureDelete_(UINT index);
	Microsoft::WRL::ComPtr<IXAudio2> GetXAudio2_() { return xAudio2_; }

	float randomFloat_(float minFloat, float maxFloat);
	int32_t randomInt_(int32_t minInt,int32_t maxInt);

	bool StartFlame_();
	bool WiodowState_();
	void PreDraw_();
	void PostDraw_();

	Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& GetCommandList_();
	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice_() { return device_; }

	Microsoft::WRL::ComPtr <ID3D12RootSignature> RootSignature_() { return rootSignature_; }
	Microsoft::WRL::ComPtr <ID3D12RootSignature> InstancingRootSignature_() { return instancingRootSignature_; }

	ID3D12PipelineState* TrianglePSO_() { return trianglePipelineState_.Get(); }
	ID3D12PipelineState* InstancingTrianglePSO_() { return particlePipelineState_.Get(); }
	ID3D12PipelineState* ParticlePSO_() { return particlePipelineState_.Get(); }
	ID3D12PipelineState* LinePSO_() { return linePipelineState_.Get(); }

	Keybord GetKeybord_();
	Mouse GetMouse_();
	Pad GetPad_(int usePadNum = 0);

	WNDCLASS GetWNDCLASS_() { return w_; }
	HWND GetHWND_() { return hwnd_; }

	// インスタンス生成
	static GameEngine* getInstance();

public:

	// コピー、代入を禁止する
	GameEngine(const GameEngine*) = delete;
	GameEngine* operator=(const GameEngine*) = delete;

	static void Delete() { delete getInstance(); }

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="WindowName">ウィンドウ名 (例:L"LE2A_00_ミョウジ_ナマエ")</param>
	/// <param name="kWindowWidth">ウィンドウの幅 (例:1280)</param>
	/// <param name="kWindowHeight">ウィンドウの高さ (例:720)</param>
	static void Intialize(const wchar_t* WindowName, int32_t kWindowWidth = 1280, int32_t kWindowHeight = 720) { getInstance()->Intialize_(WindowName, kWindowWidth, kWindowHeight); }

	/// <summary>
	/// インスタンスSRV
	/// </summary>
	/// <param name="instancingResource">インスタンスのリソース</param>
	/// <param name="instanceNum">インスタンスの大きさ</param>
	static D3D12_GPU_DESCRIPTOR_HANDLE GetInstancingSRV(Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource, int32_t numInstance) { return getInstance()->GetInstancingSRV_(instancingResource, numInstance); }

	/// <summary>
	/// テクスチャのロード
	/// </summary>
	/// <param name="filePath">テクスチャにする画像へのパス</param>
	/// <returns>テクスチャ番号</returns>
	static UINT TextureLoad(const std::string& filePath) { return getInstance()->TextureLoad_(filePath); }

	/// <summary>
	/// テクスチャのデスクリプタハンドル出力
	/// </summary>
	/// <param name="index">テクスチャ番号</param>
	/// <returns>テクスチャのデスクリプタハンドル</returns>
	static D3D12_GPU_DESCRIPTOR_HANDLE TextureGet(UINT index) { return getInstance()->TextureGet_(index); }

	/// <summary>
	/// テクスチャの無効化
	/// </summary>
	/// <param name="index">テクスチャ番号</param>
	static void TextureDelete(UINT index) { return getInstance()->TextureDelete_(index); }

	//void LoadText(Text* text, LONG fontSize, LONG fontWeight, std::wstring str, const std::string& filePath, const std::string& fontName);

	void LoadObject(Text_2D* text);

	[[nodiscard]]
	static Microsoft::WRL::ComPtr<IXAudio2> GetXAudio2() { return getInstance()->GetXAudio2_(); }

	static float randomFloat(float minFloat, float maxFloat) { return getInstance()->randomFloat_(minFloat, maxFloat); }
	static int32_t randomInt(int32_t minInt, int32_t maxInt) { return getInstance()->randomInt_(minInt, maxInt); }

	/// <summary>
	/// フレームの開始
	/// </summary>
	/// <returns>Windowsのメッセージがあるか</returns>
	[[nodiscard]]
	static bool StartFlame() { return getInstance()->StartFlame_(); }

	/// <summary>
	/// ウィンドウ状態
	/// </summary>
	/// <returns>ウィンドウを閉じているか</returns>
	[[nodiscard]]
	static bool WiodowState() { return getInstance()->WiodowState_(); }

	//描画前処理
	static void PreDraw() { getInstance()->PreDraw_(); }

	//描画後処理
	static void PostDraw() { getInstance()->PostDraw_(); }

	//コマンドリスト
	[[nodiscard]]
	static Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& GetCommandList() { return getInstance()->GetCommandList_(); }


	static Microsoft::WRL::ComPtr <ID3D12RootSignature> RootSignature() { return getInstance()->RootSignature_(); }
	static Microsoft::WRL::ComPtr <ID3D12RootSignature> InstancingRootSignature() { return getInstance()->InstancingRootSignature_(); }

	[[nodiscard]]
	static ID3D12PipelineState* TrianglePSO() { return getInstance()->TrianglePSO_(); }

	[[nodiscard]]
	static ID3D12PipelineState* ParticlePSO() { return getInstance()->ParticlePSO_(); }

	[[nodiscard]]
	static ID3D12PipelineState* LinePSO() { return getInstance()->LinePSO_(); }

	//ウィンドウ幅
	[[nodiscard]]
	static int32_t GetWindowWidth() { return kWindowWidth_; }

	//ウィンドウ高さ
	[[nodiscard]]
	static int32_t GetWindowHeight() { return kWindowHeight_; }

	//デバイス
	[[nodiscard]]
	static Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return getInstance()->GetDevice_(); }

	//キーボード入力
	[[nodiscard]]
	static Keybord GetKeybord() { return getInstance()->GetKeybord_(); }

	//マウス入力
	[[nodiscard]]
	static Mouse GetMouse() { return getInstance()->GetMouse_(); }

	/// <summary>
	/// パッド入力
	/// </summary>
	/// <param name="usePadNum">参照するパッドの番号。1つ目なら0を入力</param>
	[[nodiscard]]
	static Pad GetPad(int usePadNum = 0) { return getInstance()->GetPad_(usePadNum); }

	[[nodiscard]]
	static WNDCLASS GetWNDCLASS() { return getInstance()->GetWNDCLASS_(); }

	[[nodiscard]]
	static HWND GetHWND() { return getInstance()->GetHWND_(); }
};