#include "GameEngine.h"

#include <Windows.h>
#include <cstdint>
#include <format>
#include <sstream>

#define _USE_MATH_DEFINES
#include <cmath>

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "externals/DirectXTex/DirectXTex.h"

#include "SRT.h"
#include "ModelData.h"
#include "Material.h"
#include "MaterialData.h"
#include "TransformationMatrix.h"
#include "DirectionalLight.h"
#include "ResourceObject.h"
#include "LoadTexture.h"
#include "LoadObjFile.h"
#include "CreateBufferResource.h"
#include "CompileShader.h"
#include "CreateDescriptorHeap.h"
#include "CreateTextureResource.h"
#include "CreateDepthStencilTextureResource.h"
#include "UploadTextureData.h"
#include "GetDescriptorHandle.h"
#include "LoadMaterialTemplateFile.h"
#include "D3DResourceLeakChecker.h"
#include "ExportDump.h"

#include "Window.h"
#include "Log.h"
#include "ConvertString.h"
#include "Initialvalue.h"

#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>
#pragma comment(lib,"dxgi.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxcompiler.lib")
#include <dxcapi.h>
#pragma comment(lib,"Dbghelp.lib")
#include <DbgHelp.h>
#include <cassert>
#include <wrl.h>
#include <strsafe.h>

GameEngine::~GameEngine() {

	xAudio2_.Reset();

    Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        debugController->EnableDebugLayer();
    }
    commandQueue_->Signal(fence_.Get(), fenceValue_);
    if (fence_->GetCompletedValue() < fenceValue_) {
        fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
        WaitForSingleObject(fenceEvent_, INFINITE);
    }
    commandQueue_.Reset();
    commandAllocator_.Reset();
    commandList_.Reset();
    swapChain_.Reset();
    device_.Reset();
	directInput_->Release();
	keyboardDevice_->Release();
	mouseDevice_->Release();

	//ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CloseHandle(fenceEvent_);
	CloseWindow(hwnd_);
}

void GameEngine::Intialize(const wchar_t* WindowName, int32_t kWindowWidth, int32_t kWindowHeight) {

	HRESULT hr;

	//画面サイズを入力
	kWindowWidth_ = kWindowWidth;
	kWindowHeight_ = kWindowHeight;

	//誰も捕捉しなかった場合に(Unhandled),捕捉する関数を登録
	SetUnhandledExceptionFilter(ExportDump);

#ifdef _DEBUG
	//リソースチェック
	D3DResourceLeakChecker leakCheck;
#endif

	CoInitializeEx(0, COINIT_MULTITHREADED);

	//ウィンドウクラスの生成
	w_ = WindowClass();

	//ウィンドウの生成
	hwnd_ = WindowInitialvalue(WindowName, kWindowWidth_, kWindowHeight_,w_);

	//DirectInputの初期化
	hr = DirectInput8Create(w_.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
	assert(SUCCEEDED(hr));

	//キーボードデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboardDevice_, NULL);
	assert(SUCCEEDED(hr));
	//入力データ形式のセット
	hr = keyboardDevice_->SetDataFormat(&c_dfDIKeyboard);	//標準形式
	assert(SUCCEEDED(hr));
	//排他制御レベルのセット
	hr = keyboardDevice_->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	//マウスデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysMouse, &mouseDevice_, NULL);
	assert(SUCCEEDED(hr));
	//入力データ形式のセット
	hr = mouseDevice_->SetDataFormat(&c_dfDIMouse);	//標準形式
	assert(SUCCEEDED(hr));
	//排他制御レベルのセット
	hr = mouseDevice_->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	//ログファイルの生成
	logStream_ = CreateLogFile();

	//エラーコード
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(hr));

	//使用するアダプタ用変数
	useAdapter_ = useAdapterInitialvalue(dxgiFactory_, logStream_);

	//使用するデバイスの初期値
	device_ = deviceInitialvalue(useAdapter_, logStream_);

	//DescriptorSizeを取得しておく
	descriptorSizeSRV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeDSV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//コマンドキューを生成する
	commandQueue_ = commandQueueInitialvalue(device_);

	//コマンドアロケータを生成する
	commandAllocator_ = commandAllocatorInitialvalue(device_);

	//コマンドリストを生成する
	commandList_ = commandListInitialvalue(commandAllocator_, device_);

	swapChain_ = nullptr;

	//画面の幅
	swapChainDesc_.Width = kWindowWidth_;
	//画面の高さ
	swapChainDesc_.Height = kWindowHeight_;
	//色の形式
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//マルチサンプルしない
	swapChainDesc_.SampleDesc.Count = 1;
	//描画のターゲットとして利用する
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//ダブルバッファ
	swapChainDesc_.BufferCount = 2;
	//モニタにうつしたら、中身を破棄
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), hwnd_, &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));

	//RTV用のヒープでディスクリプタ数は2。RTVはShader内で触る物ではないので、ShaderVisibleはfalse
	rtvDescriptorHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	//SRV用のディスクリプタの数は128。SRVはShader内で触る物なので、ShaderVisibleはtrue
	srvDescriptorHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	//SwapChainからResourcesを引っ張ってくる
	for (int i = 0; i < 2; i++) {
		hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&swapChainResources_[i]));
		//うまく取得できなければ起動しない
		assert(SUCCEEDED(hr));
	}

	//RTVの設定
	//出力結果をSRGBに変換して書き込む
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//2dテクスチャとして書き込む
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	//まず1つ目を作る。1つ目は最初のところに作る。作る場所はこちらで指定する。
	rtvHandles_[0] = GetCPUDescriptorHandle(rtvDescriptorHeap_, descriptorSizeRTV_, 0);
	device_->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc_, rtvHandles_[0]);
	//2つ目を作る
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	device_->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc_, rtvHandles_[1]);

	//初期値0でFenceを作る
	fence_ = nullptr;
	fenceValue_ = 0;

	hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	//FenceのSignalを待つためのイベントを生成する
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);

	//dxcCompilerを初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));

	//RootSignature作成
	rootSignature_ = rootSignatureInitialvalue(device_, logStream_);

	//Shaderをコンパイルする
	IDxcBlob* vertexShaderBlob = CompileShader(L"Object3D.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler_.Get(), logStream_);
	assert(vertexShaderBlob != nullptr);
	IDxcBlob* pixelShaderBlob = CompileShader(L"OBject3D.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler_.Get(), logStream_);
	assert(pixelShaderBlob != nullptr);

	//DepthStencilTextureをウィンドウのサイズで作成
	depthStencilResource_ = CreateDepthStencilTextureResource(device_, kWindowWidth_, kWindowHeight_);
	//DSV用のヒープれディスクリプタの数は1。DSVはShader内で触る物ではないので、ShaderVisibleはfalse
	dsvDescriptorheap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	//DSVの設定
	dsvDesc_.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//Format。基本的にはResourceに合わせる
	dsvDesc_.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;	//2dTexture
	//DSVHeapの先頭にDSVを作る
	device_->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc_, dsvDescriptorheap_->GetCPUDescriptorHandleForHeapStart());

	//PSOを生成
	graphicsPipelineState_ = graphicsPipelineStateInitialvalue(device_, logStream_, rootSignature_, vertexShaderBlob, pixelShaderBlob);

	//WVP用のリソースを作る
	wvpResource_ = CreateBufferResource(device_, sizeof(TransformationMatrix));

	//ビューポート
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = FLOAT(kWindowWidth_);
	viewport_.Height = FLOAT(kWindowHeight_);
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	//シザー矩形
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = kWindowWidth_;
	scissorRect_.top = 0;
	scissorRect_.bottom = kWindowHeight_;

	//ImGuiが0番を使用しているため、1番から使用する
	kLastCPUIndex_ = 1;
	kLastGPUIndex_ = 1;

	//XAudioエンジンのインスタンスを生成
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));

	//マスターボイスを生成
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));


	//ImGui初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd_);
	ImGui_ImplDX12_Init(device_.Get(), swapChainDesc_.BufferCount,
		rtvDesc_.Format,
		srvDescriptorHeap_.Get(),
		srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart());
}

void GameEngine::LoadLight(Light* light) {

	light->Initialize(device_);

}

void GameEngine::LoadTexture(Texture* texture, const std::string& filePath) {

	texture->Initialize(filePath, device_, commandQueue_, commandAllocator_, commandList_, fence_, fenceValue_, fenceEvent_, srvDescriptorHeap_, descriptorSizeSRV_, kLastCPUIndex_, kLastGPUIndex_);

}

void GameEngine::LoadObject(Object_3D* object, const std::string& directoryPath, const std::string& filename) {

	object->Initialize(directoryPath, filename, device_);

}

void GameEngine::LoadObject(Object_2D* object) {

	object->Initialize(device_);

}

void GameEngine::LoadAudio(Audio* audio, const char* filename,bool isLoop) {

	audio->Initialize(filename, xAudio2_.Get(),isLoop);

}

bool GameEngine::StartFlame() {
	//Windowのメッセージが来てたら最優先で処理させる
	if (PeekMessage(&msg_, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg_);
		DispatchMessage(&msg_);
		return false;
	}
#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		//解放
		infoQueue->Release();

		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
#endif

	keyboardDevice_->Acquire();
	//前frame処理
	memcpy(preKeys_, keys_, sizeof(BYTE) * 256);
	//キーボード入力
	keyboardDevice_->GetDeviceState(sizeof(BYTE) * 256, keys_);

	mouseDevice_->Acquire();
	//前frame処理
	memcpy(&preMouse_, &mouse_, sizeof(DIMOUSESTATE));
	//マウス入力
	mouseDevice_->GetDeviceState(sizeof(DIMOUSESTATE),&mouse_);

	//パッド入力
	//0~4個のパッドから接続されているパッド入力を得る
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++) {

		//前frame処理
		memcpy(&prePad_[i], &pad_[i], sizeof(XINPUT_STATE));
		ZeroMemory(&pad_[i], sizeof(XINPUT_STATE));

		//パッド入力を入手
		dwResult_[i] = XInputGetState(i, &pad_[i]);
	}

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	return true;
}

bool GameEngine::WiodowState() {
	if (msg_.message != WM_QUIT) {
		return true;
	}
	return false;
}

void GameEngine::PreDraw() {

	//描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap_.Get() };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);

	//ImGuiの内部コマンドを生成する
	ImGui::Render();

	//これから書き込むバックバッファのインデックスを取得
	UINT backBafferIndex = swapChain_->GetCurrentBackBufferIndex();

	//TransitionBarrierの設定
	//今回のバリアはTransition
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier_.Transition.pResource = swapChainResources_[backBafferIndex].Get();
	//遷移前(現在)のResourcesState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);

	//描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetCPUDescriptorHandle(dsvDescriptorheap_, descriptorSizeDSV_, 0);
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBafferIndex], false, &dsvHandle);
	//指定した色で画面全体をクリアする
	float crearColor[] = { 0.1f,0.25f,0.5f,1.0f };//青っぽい色。RGBAの順
	commandList_->ClearRenderTargetView(rtvHandles_[backBafferIndex], crearColor, 0, nullptr);
	//指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//オブジェクト描画前処理
	commandList_->RSSetViewports(1, &viewport_);			//Viewportを設定
	commandList_->RSSetScissorRects(1, &scissorRect_);	//Scirssorを設定
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->SetPipelineState(graphicsPipelineState_.Get());	//PSOを設定
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void GameEngine::PostDraw() {

	//実際のcommandListのImGuiの描画コマンドを詰む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList_.Get());

	//RenderTargetからPresentにする
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);

	//コマンドリストの内容を確定させる。すべてのコマンドを詰んでからCloseすること
	HRESULT hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	//GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
	//GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);

	//Fenceの値を更新
	fenceValue_++;
	//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);

	//Fenceの値が指定したSignal値に辿り着いているか確認する
	//GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_) {
		//指定したSignalに辿り着いていないので、辿り着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		//イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	//次のフレーム用のコマンドリストを準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& GameEngine::GetCommandList() {
	return commandList_;
}

Keybord GameEngine::GetKeybord() {

	Keybord returnKeybord{};

	for (int i = 0; i < 256; i++) {
		returnKeybord.trigger[i] = keys_[i];
		returnKeybord.leave[i] = ~keys_[i];
		returnKeybord.hit[i] = keys_[i] & ~preKeys_[i];
		returnKeybord.release[i] = ~keys_[i] & preKeys_[i];
	}

	return returnKeybord;
}

Mouse GameEngine::GetMouse() {

	Mouse returnMouse{};

	//マウス座標
	POINT p;
	GetCursorPos(&p);
	//スクリーン上からウィンドウ上へ
	ScreenToClient(FindWindowW(w_.lpszClassName, nullptr) ,&p);

	returnMouse.Position = { float(p.x),float(p.y) };
	returnMouse.Movement = { float(mouse_.lX),float(mouse_.lY),float(mouse_.lZ) };
	for (int i = 0; i < 3; i++) {
		returnMouse.click[i] = mouse_.rgbButtons[i];
	}

	return returnMouse;
}

Pad GameEngine::GetPad(int usePadNum) {

	Pad returnPad{};

	//接続されているか
	if (dwResult_[usePadNum] == ERROR_SUCCESS) {
		//接続されている
		returnPad.isConnected = true;

		//スティックの傾きを得る
		//デッドゾーンチェック
		float LX = pad_[usePadNum].Gamepad.sThumbLX;
		float LY = pad_[usePadNum].Gamepad.sThumbLY;

		float magnitude = sqrtf(powf(LX, 2) + powf(LY, 2));

		float normalizedLX = LX / magnitude;
		float normalizedLY = LY / magnitude;

		float normalizedMagnitude = 0;

		if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
			if (magnitude > 32767) {
				magnitude = 32767;
			}
			magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

			normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		} else {
			magnitude = 0.0f;
			normalizedMagnitude = 0.0f;
		}

		returnPad.LeftStick = {
			normalizedMagnitude,
			{ normalizedLX, normalizedLY}
		};

		//Rスティックも
		float RX = pad_[usePadNum].Gamepad.sThumbRX;
		float RY = pad_[usePadNum].Gamepad.sThumbRY;

		magnitude = sqrtf(powf(RX, 2) + powf(RY, 2));

		float normalizedRX = RX / magnitude;
		float normalizedRY = RY / magnitude;

		normalizedMagnitude = 0;

		if (magnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
			if (magnitude > 32767) {
				magnitude = 32767;
			}
			magnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

			normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		} else {
			magnitude = 0.0f;
			normalizedMagnitude = 0.0f;
		}

		returnPad.RightStick = {
			normalizedMagnitude,
			{ normalizedRX, normalizedRY}
		};

		//ボタンの入力変換
		for (int i = 0; i <= 16; i++) {
			int trigger = 0x0001 << i;
			if (i != PAD_BOTTON_LT && i != PAD_BOTTON_RT) {
				returnPad.Button[i].trigger = pad_[usePadNum].Gamepad.wButtons & trigger;
				returnPad.Button[i].leave = ~(pad_[usePadNum].Gamepad.wButtons & trigger);
				returnPad.Button[i].hit = (pad_[usePadNum].Gamepad.wButtons & trigger) & ~(prePad_[usePadNum].Gamepad.wButtons & trigger);
				returnPad.Button[i].hit = ~(pad_[usePadNum].Gamepad.wButtons & trigger) & (prePad_[usePadNum].Gamepad.wButtons & trigger);
			} else {
				if (i == PAD_BOTTON_LT) {
					returnPad.Button[i].trigger = pad_[usePadNum].Gamepad.bLeftTrigger >= 0x80;
					returnPad.Button[i].leave = !(pad_[usePadNum].Gamepad.bLeftTrigger >= 0x80);
					returnPad.Button[i].hit = (pad_[usePadNum].Gamepad.bLeftTrigger >= 0x80) && !(prePad_[usePadNum].Gamepad.bLeftTrigger >= 0x80);
					returnPad.Button[i].hit = !(pad_[usePadNum].Gamepad.bLeftTrigger >= 0x80) && (prePad_[usePadNum].Gamepad.bLeftTrigger >= 0x80);
				} else {
					returnPad.Button[i].trigger = pad_[usePadNum].Gamepad.bRightTrigger >= 0x80;
					returnPad.Button[i].leave = !(pad_[usePadNum].Gamepad.bRightTrigger >= 0x80);
					returnPad.Button[i].hit = (pad_[usePadNum].Gamepad.bRightTrigger >= 0x80) && !(prePad_[usePadNum].Gamepad.bRightTrigger >= 0x80);
					returnPad.Button[i].hit = !(pad_[usePadNum].Gamepad.bRightTrigger >= 0x80) && (prePad_[usePadNum].Gamepad.bRightTrigger >= 0x80);
				}
			}
		}
	} else {
		//接続されていない
		returnPad.isConnected = false;
	}

	ImGui::Begin("pad");
	if (returnPad.isConnected) {
		ImGui::Text("true");
	} else {
		ImGui::Text("false");
	}
	ImGui::Text("LStick : %f,%f,%f", returnPad.LeftStick.vector.x, returnPad.LeftStick.vector.y, returnPad.LeftStick.magnitude);
	ImGui::Text("RStick : %f,%f,%f", returnPad.RightStick.vector.x, returnPad.RightStick.vector.y, returnPad.RightStick.magnitude);
	if (returnPad.Button[PAD_BOTTON_A].trigger) {
		ImGui::Text("A");
	}
	if (returnPad.Button[PAD_BOTTON_RT].trigger) {
		ImGui::Text("RT");
		XINPUT_VIBRATION vibration;
		ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
		vibration.wLeftMotorSpeed = 32000; // use any value between 0-65535 here
		vibration.wRightMotorSpeed = 16000; // use any value between 0-65535 here
		XInputSetState(usePadNum, &vibration);
	} else {
		XINPUT_VIBRATION vibration{};
		ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
		XInputSetState(usePadNum, &vibration);
	}
	ImGui::End();

	return returnPad;
}