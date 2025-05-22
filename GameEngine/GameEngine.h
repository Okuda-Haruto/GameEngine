#pragma once
#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#pragma comment(lib,"dxcompiler.lib")
#include <dxcapi.h>

#define DIRECTINPUT_VERSION 0x0800	//DirectInputのバージョン指定
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#include <dinput.h>

#pragma comment(lib,"xinput.lib")
#include <Xinput.h>

#include "D3DResourceLeakChecker.h"
#include "Light.h"
#include "Object_3D.h"
#include "Object_2D.h"
#include "Texture.h"
#include "Text.h"
#include "Audio.h"
#include "Input.h"
#include "DebugCamera.h"

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
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState_;
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

	//CPUの最後尾Index
	uint32_t kLastCPUIndex_;
	//GPUの最後尾Index
	uint32_t kLastGPUIndex_;

	BYTE keys_[256]{};
	BYTE preKeys_[256]{};

	DIMOUSESTATE preMouse_;
	DIMOUSESTATE mouse_;

	XINPUT_STATE pad_[4];
	DWORD dwResult_[4];
	XINPUT_STATE prePad_[4];
public:
	//デストラクタ
	~GameEngine();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="WindowName">ウィンドウ名 (例:L"LE2A_00_ミョウジ_ナマエ")</param>
	/// <param name="kWindowWidth">ウィンドウの幅 (例:1280)</param>
	/// <param name="kWindowHeight">ウィンドウの高さ (例:720)</param>
	void Intialize(const wchar_t* WindowName, int32_t kWindowWidth = 1280, int32_t kWindowHeight = 720);

	/// <summary>
	/// 光源のロード
	/// </summary>
	/// <param name="light">ロードするLightクラス</param>
	void LoadLight(Light* light);

	/// <summary>
	/// テクスチャのロード
	/// </summary>
	/// <param name="texture">ロードするTextureクラス (例:white1x1_texture)</param>
	/// <param name="filePath">テクスチャがあるフォルダへのパス (例:""resources/white1x1.png"")</param>
	void LoadTexture(Texture* texture, const std::string& filePath);

	void LoadText(Text* text, LONG fontSize, LONG fontWeight, std::wstring str, const std::string& filePath, const std::string& fontName);

	/// <summary>
	/// 3Dオブジェクトのロード
	/// </summary>
	/// <param name="object">ロードするObject_3Dクラス</param>
	/// <param name="directoryPath">.objファイルのある階層 (例:"resource")</param>
	/// <param name="filename">ファイル名 (例:"plane.obj")</param>
	void LoadObject(Object_3D* object, const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// 2Dオブジェクトのロード
	/// </summary>
	/// /// <param name="object">ロードするSprote_2Dクラス</param>
	void LoadObject(Sprite_2D* sprite);

	void LoadObject(Text_2D* text);

	/// <summary>
	/// 音声のロード
	/// </summary>
	/// <param name="audio">ロードするAudioクラス</param>
	/// <param name="filename">.wavファイル名 (例:resources/Audio.wav)</param>
	/// <param name="isLoop">ループ再生するか</param>
	void LoadAudio(Audio* audio, const char* filename,bool isLoop);

	/// <summary>
	/// カメラ変換
	/// </summary>
	/// <param name="transform">カメラSRT</param>
	/// <returns></returns>
	[[nodiscard]]
	Camera UpdateCamera(Vector3 rotate,Vector3 Translate);

	/// <summary>
	/// フレームの開始
	/// </summary>
	/// <returns>Windowsのメッセージがあるか</returns>
	bool StartFlame();

	/// <summary>
	/// ウィンドウ状態
	/// </summary>
	/// <returns>ウィンドウを閉じているか</returns>
	bool WiodowState();

	//描画前処理
	void PreDraw();

	//描画後処理
	void PostDraw();

	//コマンドリスト
	[[nodiscard]]
	Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& GetCommandList();

	//キーボード入力
	[[nodiscard]]
	Keybord GetKeybord();

	//マウス入力
	[[nodiscard]]
	Mouse GetMouse();

	/// <summary>
	/// パッド入力
	/// </summary>
	/// <param name="usePadNum">参照するパッドの番号。1つ目なら0を入力</param>
	[[nodiscard]]
	Pad GetPad(int usePadNum = 0);
};