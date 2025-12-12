#include "GameEngine.h"

#define _USE_MATH_DEFINES
#include <cmath>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"Dbghelp.lib")
#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")

#include "ExportDump.h"
#include "ConvertString.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <DbgHelp.h>
#include <cassert>
#include <wrl.h>
#include <strsafe.h>

#include "Log.h"
#include "Initialvalue.h"

int32_t GameEngine::kWindowWidth_;
int32_t GameEngine::kWindowHeight_;

GameEngine::GameEngine() {

}

GameEngine::~GameEngine() {

	xAudio2_.Reset();

	directInput_->Release();
	keyboardDevice_->Release();
	mouseDevice_->Release();

	trianglePipelineState_.Reset();
	instancingTrianglePipelineState_.Reset();
	particlePipelineState_.Reset();
	linePipelineState_.Reset();

	delete imguiManager_;
	delete srvManager_;
	delete dxCommon_;
	delete winApp_;
	AudioManager::GetInstance()->Finalize();
	TextureManager::GetInstance()->Finalize();
	ModelManager::GetInstance()->Finalize();
	SpriteManager::GetInstance()->Finalize();
	PrimitiveManager::GetInstance()->Finalize();
	Object::FinalizeDefaultCamera();
	ParticleManager::GetInstance()->Finalize();
}

GameEngine* GameEngine::getInstance() {
	static GameEngine* instance = new GameEngine();
	return instance;
}

void GameEngine::Intialize_(const wchar_t* WindowName, int32_t kWindowWidth, int32_t kWindowHeight) {

	HRESULT hr;

	//画面サイズを入力
	kWindowWidth_ = winApp_->kClientWidth_;
	kWindowHeight_ = winApp_->kClientHeight_;

	//誰も捕捉しなかった場合に(Unhandled),捕捉する関数を登録
	SetUnhandledExceptionFilter(ExportDump);

	//COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	//メディアファンデーションの初期化
	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);

	winApp_ = new WindowsAPI;
	winApp_->Initialize(WindowName, kWindowWidth, kWindowHeight);

	dxCommon_ = new DirectXCommon;
	dxCommon_->Initialize(winApp_);

	srvManager_ = new SRVManager;
	srvManager_->Initialize(dxCommon_);

	imguiManager_ = new ImGuiManager();
	imguiManager_->Initialize(dxCommon_, winApp_, srvManager_);
	
	TextureManager::GetInstance()->Initialize(dxCommon_, srvManager_);
	ModelManager::GetInstance()->Initialize(dxCommon_);
	SpriteManager::GetInstance()->Initialize(dxCommon_);
	PrimitiveManager::GetInstance()->Initialize(dxCommon_, srvManager_);

	//カメラ初期値
	Camera* DefaultCamera = new Camera;
	DefaultCamera->Initialize(dxCommon_);
	Object::SetDefaultCamera(DefaultCamera);
	ParticleManager::GetInstance()->Initialize(dxCommon_, srvManager_);

	device_ = dxCommon_->GetDevice();
	commandList_ = dxCommon_->GetCommandList();

	//DirectInputの初期化
	hr = DirectInput8Create(winApp_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
	assert(SUCCEEDED(hr));

	//キーボードデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboardDevice_, NULL);
	assert(SUCCEEDED(hr));
	//入力データ形式のセット
	hr = keyboardDevice_->SetDataFormat(&c_dfDIKeyboard);	//標準形式
	assert(SUCCEEDED(hr));
	//排他制御レベルのセット
	hr = keyboardDevice_->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	//マウスデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysMouse, &mouseDevice_, NULL);
	assert(SUCCEEDED(hr));
	//入力データ形式のセット
	hr = mouseDevice_->SetDataFormat(&c_dfDIMouse);	//標準形式
	assert(SUCCEEDED(hr));
	//排他制御レベルのセット
	hr = mouseDevice_->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);



	//RootSignature作成
	rootSignature_ = dxCommon_->TriangleRootSignatureInitialvalue();
	instancingRootSignature_ = dxCommon_->InstancingRootSignatureInitialvalue();

	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxCommon_->CompileShader(L"./resources/Shader/Object3D.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = dxCommon_->CompileShader(L"./resources/Shader/OBject3D.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> instancingVertexShaderBlob = dxCommon_->CompileShader(L"./resources/Shader/InstanceObject3D.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> particleVSBlob = dxCommon_->CompileShader(L"./resources/Shader/Particle.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> particlePSBlob = dxCommon_->CompileShader(L"./resources/Shader/Particle.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//PSOを生成
	trianglePipelineState_ = TrianglePipelineStateInitialvalue(device_, rootSignature_, vertexShaderBlob.Get(), pixelShaderBlob.Get());
	noDepthTrianglePipelineState_ = NoDepthTrianglePipelineStateInitialvalue(device_, rootSignature_, vertexShaderBlob.Get(), pixelShaderBlob.Get());
	instancingTrianglePipelineState_ = InstancingTrianglePipelineStateInitialvalue(device_, instancingRootSignature_, particleVSBlob.Get(), particlePSBlob.Get());
	particlePipelineState_ = NoDepthAddBlendTrianglePipelineStateInitialvalue(device_, instancingRootSignature_, particleVSBlob.Get(), particlePSBlob.Get());
	spritePipelineState_ = SpritePipelineStateInitialvalue(device_, rootSignature_, vertexShaderBlob.Get(), pixelShaderBlob.Get());
	linePipelineState_ = LinePipelineStateInitialvalue(device_, instancingRootSignature_, particleVSBlob.Get(), particlePSBlob.Get());
	noDepthLinePipelineState_ = NoDepthLinePipelineStateInitialvalue(device_, instancingRootSignature_, particleVSBlob.Get(), particlePSBlob.Get());
	//XAudioエンジンのインスタンスを生成
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));

	//マスターボイスを生成
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));

	AudioManager::GetInstance()->Initialize(xAudio2_);

	//乱数シード初期化
	std::random_device seedGenerator;
	randomEngine_.seed(seedGenerator());

	StructuredBufferIndex_ = srvManager_->Allocate();

	//初期化
	for (int i = 0; i < kMaxIndex; i++) {
		objectMaterialResource_[i] = dxCommon_->CreateBufferResources(sizeof(Material));
		objectWvpResource_[i] = dxCommon_->CreateBufferResources(sizeof(TransformationMatrix));
		spriteMaterialResource_[i] = dxCommon_->CreateBufferResources(sizeof(Material));
		spriteWvpResource_[i] = dxCommon_->CreateBufferResources(sizeof(TransformationMatrix));
	}

	for (int i = 0; i < kMaxInstanceIndex; i++) {
		instancingObjectMaterialResource_[i] = dxCommon_->CreateBufferResources(sizeof(Material));
		instancingObjectResource_[i] = dxCommon_->CreateBufferResources(sizeof(InstancingTransformationMatrix) * kMaxNumInstance);
		uint32_t index = srvManager_->Allocate();
		// SRV を作成（NumElements と stride は一致させる）
		srvManager_->CreateSRVforStructuredBuffer(index, instancingObjectResource_[i].Get(), kMaxNumInstance, sizeof(InstancingTransformationMatrix));
		if (i == 0)startInstancingObjectIndex = index;
	}
	for (int i = 0; i < kMaxInstanceIndex; i++) {
		particleMaterialResource_[i] = dxCommon_->CreateBufferResources(sizeof(Material));
		particleResource_[i] = dxCommon_->CreateBufferResources(sizeof(InstancingTransformationMatrix) * kMaxNumInstance);
	}
	for (int i = 0; i < kMaxInstanceIndex; i++) {
		instancingSpriteMaterialResource_[i] = dxCommon_->CreateBufferResources(sizeof(Material));
		instancingSpriteResource_[i] = dxCommon_->CreateBufferResources(sizeof(InstancingTransformationMatrix) * kMaxNumInstance);
		uint32_t index = srvManager_->Allocate();
		// SRV を作成（NumElements と stride は一致させる）
		srvManager_->CreateSRVforStructuredBuffer(index, instancingSpriteResource_[i].Get(), kMaxNumInstance, sizeof(InstancingTransformationMatrix));
		if (i == 0)startInstancingSpriteIndex = index;
	}

	for (int i = 0; i < PrimitiveManager::SHAPE_count; i++) {
		primitiveMaterialResource_[i] = dxCommon_->CreateBufferResources(sizeof(Material));
		primitiveResource_[i] = dxCommon_->CreateBufferResources(sizeof(InstancingTransformationMatrix) * PrimitiveManager::kMaxNumPrimitive);
	}

}

/*void GameEngine::LoadText(Text* text, LONG fontSize, LONG fontWeight, std::wstring str, const std::string& filePath, const std::string& fontName) {

	text->Initialize(fontSize, fontWeight, filePath, fontName, hwnd_);

	for (int i = 0; i < str.length(); i++) {
		//#で特殊な設定にする
		if (str.c_str()[i] == L'#') {
			//次の文字で設定の種類を見る
			i++;
			switch (str.c_str()[i])
			{
			case L'#':	//#自体も入力できるようにする
				text->GetTextData(str.c_str()[i], device_, commandQueue_, commandAllocator_, commandList_, fence_, fenceValue_, fenceEvent_, srvDescriptorHeap_, descriptorSizeSRV_, kLastCPUIndex_, kLastGPUIndex_);
				break;
			case L'C':	//ColorのC	例:#C[0xFFFFFF]
				i++;
				if (str.c_str()[i] == L'[') {	//L'['が入力されていたらColor入力に移行
					i++;
					std::wstring settingStr{};
					while (str.c_str()[i] != L']' || settingStr.size() > 10)	//L']'が入力されるまでループ
					{
						settingStr += str.c_str()[i];
						i++;
					}
					if ((settingStr.size() == 8 || settingStr.size() == 10) && settingStr[0] == L'0' && (settingStr[1] == L'x' || settingStr[1] == L'X')) {	//正しく入力されていたら設定を変更する
						int R = 0;
						int G = 0;
						int B = 0;
						for (int j = 2; j < 8; j++) {
							if (isxdigit(settingStr[j])) {	//16進数か
								switch ((j - 2) / 2)
								{
								case 0:
									R = R << 4;
									if (isdigit(settingStr[j])) {
										R += settingStr[j] - L'0';
									} else {
										if (isupper(settingStr[j])) {
											R += (settingStr[j] - L'A') + 10;
										} else {
											R += (settingStr[j] - L'a') + 10;
										}
									}
									break;
								case 1:
									G = G << 4;
									if (isdigit(settingStr[j])) {
										G += settingStr[j] - L'0';
									} else {
										if (isupper(settingStr[j])) {
											G += (settingStr[j] - L'A') + 10;
										} else {
											G += (settingStr[j] - L'a') + 10;
										}
									}
									break;
								case 2:
									B = B << 4;
									if (isdigit(settingStr[j])) {
										B += settingStr[j] - L'0';
									} else {
										if (isupper(settingStr[j])) {
											B += (settingStr[j] - L'A') + 10;
										} else {
											B += (settingStr[j] - L'a') + 10;
										}
									}
									break;
								default:
									break;
								}
							} else {	//16進数以外が入力されたら白にする
								R = 0xFF; G = 0xFF; B = 0xFF;
								break;
							}
						}
						text->SetColor(R, G, B);
					} else {
						break;
					}
				} else {
					i--;
				}
				break;
			case L'S':	//SizeのS	例:#S[64]
				i++;
				if (str.c_str()[i] == L'[') {	//L'['が入力されていたらSize入力に移行
					i++;
					std::wstring settingStr{};
					while (str.c_str()[i] != L']' || settingStr.size() > 4)	//L']'が入力されるまでループ	サイズが4桁を超えるサイズになるのはおかしいのでループを抜ける
					{
						settingStr += str.c_str()[i];
						i++;
					}
					LONG settingLong = 0;
					for (int j = 0; j < settingStr.size(); j++) {
						if (!isdigit(settingStr[j])) {	//数字以外が入力されたらそのままにする
							LOGFONTW lf = text->GetLogfont();
							settingLong = lf.lfHeight;
						}
						settingLong *= 10;
						settingLong += settingStr[j] - L'0';
					}
					LOGFONTW lf = text->GetLogfont();
					lf.lfHeight = settingLong;
					text->SetLogfont(lf);
				}
				break;
			case L'W':	//WeightのW	例:#W[400]
				i++;
				if (str.c_str()[i] == L'[') {	//L'['が入力されていたらWeight入力に移行
					i++;
					std::wstring settingStr{};
					while (str.c_str()[i] != L']' || settingStr.size() > 4)	//L']'が入力されるまでループ	サイズが4桁を超えるサイズになるのはおかしいのでループを抜ける
					{
						settingStr += str.c_str()[i];
						i++;
					}
					LONG settingLong = 0;
					for (int j = 0; j < settingStr.size(); j++) {
						if (!isdigit(settingStr[j])) {	//数字以外が入力されたらそのままにする
							LOGFONTW lf = text->GetLogfont();
							settingLong = lf.lfWeight;
						}
						settingLong *= 10;
						settingLong += settingStr[j] - L'0';
					}
					LOGFONTW lf = text->GetLogfont();
					lf.lfWeight = settingLong;
					text->SetLogfont(lf);
				}
				break;
			default:	//どれでもないなら戻す
				i--;
				break;
			}
		} else {
			text->GetTextData(str.c_str()[i], device_, commandQueue_, commandAllocator_, commandList_, fence_, fenceValue_, fenceEvent_, srvDescriptorHeap_, descriptorSizeSRV_, kLastCPUIndex_, kLastGPUIndex_);
		}
	}
}*/

float GameEngine::randomFloat_(float minFloat, float maxFloat) {
	assert(minFloat <= maxFloat);
	std::uniform_real_distribution<float> distribution(minFloat, maxFloat);
	return distribution(randomEngine_);
}
int32_t GameEngine::randomInt_(int32_t minInt, int32_t maxInt) {
	assert(minInt > maxInt);
	float minFloat = float(minInt);
	float maxFloat = float(maxInt);
	std::uniform_real_distribution<float> distribution(minFloat, maxFloat);
	float randomNum = distribution(randomEngine_);
	return int32_t(randomNum);
}

[[nodiscard]]
bool GameEngine::StartFlame_() {
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
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
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

	imguiManager_->Begin();

	return true;
}

[[nodiscard]]
bool GameEngine::WindowState_() {
	if (msg_.message != WM_QUIT) {

		return true;
	}
	return false;
}

void GameEngine::PreDraw_() {

	//Index初期化
	objectIndex = 0;
	instancingObjectIndex = 0;
	particleIndex = 0;
	spriteIndex = 0;
	instancingSpriteIndex = 0;

	imguiManager_->End();

	srvManager_->PreDraw();

}

void GameEngine::PostDraw_() {

	PrimitiveManager::GetInstance()->Draw();

	imguiManager_->Draw();

	dxCommon_->PostDraw();
}

[[nodiscard]]
Keybord GameEngine::GetKeybord_() {

	Keybord returnKeybord{};

	for (int i = 0; i < 256; i++) {
		returnKeybord.hold[i] = keys_[i];
		returnKeybord.idle[i] = ~keys_[i];
		returnKeybord.trigger[i] = keys_[i] & ~preKeys_[i];
		returnKeybord.release[i] = ~keys_[i] & preKeys_[i];
	}

	return returnKeybord;
}

[[nodiscard]]
Mouse GameEngine::GetMouse_() {

	Mouse returnMouse{};

	//マウス座標
	POINT p;
	GetCursorPos(&p);
	//スクリーン上からウィンドウ上へ
	ScreenToClient(winApp_->GetHwnd(), &p);

	returnMouse.Position = { float(p.x),float(p.y) };
	returnMouse.Movement = { float(mouse_.lX),float(mouse_.lY),float(mouse_.lZ) };
	for (int i = 0; i < 3; i++) {
		returnMouse.click[i] = mouse_.rgbButtons[i];
	}

	return returnMouse;
}

[[nodiscard]]
Pad GameEngine::GetPad_(int usePadNum) {

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
			int hold = 0x0001 << i;
			if (i != PAD_BUTTON_LT && i != PAD_BUTTON_RT) {
				returnPad.Button[i].hold = pad_[usePadNum].Gamepad.wButtons & hold;
				returnPad.Button[i].idle = ~(pad_[usePadNum].Gamepad.wButtons & hold);
				returnPad.Button[i].trigger = (pad_[usePadNum].Gamepad.wButtons & hold) & ~(prePad_[usePadNum].Gamepad.wButtons & hold);
				returnPad.Button[i].release = ~(pad_[usePadNum].Gamepad.wButtons & hold) & (prePad_[usePadNum].Gamepad.wButtons & hold);
			} else {
				if (i == PAD_BUTTON_LT) {
					returnPad.Button[i].hold = pad_[usePadNum].Gamepad.bLeftTrigger >= 0x80;
					returnPad.Button[i].idle = !(pad_[usePadNum].Gamepad.bLeftTrigger >= 0x80);
					returnPad.Button[i].trigger = (pad_[usePadNum].Gamepad.bLeftTrigger >= 0x80) && !(prePad_[usePadNum].Gamepad.bLeftTrigger >= 0x80);
					returnPad.Button[i].release = !(pad_[usePadNum].Gamepad.bLeftTrigger >= 0x80) && (prePad_[usePadNum].Gamepad.bLeftTrigger >= 0x80);
				} else {
					returnPad.Button[i].hold = pad_[usePadNum].Gamepad.bRightTrigger >= 0x80;
					returnPad.Button[i].idle = !(pad_[usePadNum].Gamepad.bRightTrigger >= 0x80);
					returnPad.Button[i].trigger = (pad_[usePadNum].Gamepad.bRightTrigger >= 0x80) && !(prePad_[usePadNum].Gamepad.bRightTrigger >= 0x80);
					returnPad.Button[i].release = !(pad_[usePadNum].Gamepad.bRightTrigger >= 0x80) && (prePad_[usePadNum].Gamepad.bRightTrigger >= 0x80);
				}
			}
		}
	} else {
		//接続されていない
		returnPad.isConnected = false;
	}

	return returnPad;
}

void GameEngine::DrawObject_3D_(Object* object, DirectionalLight* directionalLight, PointLight* pointLight, SpotLight* spotLight) {

	//上限に達していたら描画しない
	if (objectIndex >= kMaxIndex)return;

	std::vector<Parts> parts = object->GetParts();
	std::vector<Offset> offsets = object->GetOffsets();

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->SetPipelineState(trianglePipelineState_.Get());	//PSOを設定

	commandList_->IASetVertexBuffers(0, 1, &object->GetVBV());	//VBVを設定
	commandList_->IASetIndexBuffer(&object->GetIBV());	//IBVを設定

	//カメラのワールド座標をCBufferに送る
	commandList_->SetGraphicsRootConstantBufferView(4, object->GetCamera()->CameraResource()->GetGPUVirtualAddress());

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//オブジェクトのワールド座標
	Matrix4x4 worldMatrix = MakeQuaternionMatrix(object->GetTransform().scale, object->GetTransform().rotate, object->GetTransform().translate);

	//変更が必要な部分だけ変える
	for (int i = 0; i < parts.size();i++) {

		//WVPデータを更新
		objectWvpResource_[objectIndex]->Map(0, nullptr, reinterpret_cast<void**>(&objectWvpData_[objectIndex]));

		Matrix4x4 partsMatrix = MakeQuaternionMatrix(parts[i].transform->scale, parts[i].transform->rotate, parts[i].transform->translate);
		if (parts[i].parent) {
			//親を持つPartsのローカル座標
			Matrix4x4 parentMatrix = MakeQuaternionMatrix(parts[i].parent->scale, parts[i].parent->rotate, parts[i].parent->translate);
			partsMatrix = partsMatrix * parentMatrix;
		} else {
			//ワールド座標を親に持つPartsのローカル座標
			partsMatrix = partsMatrix * worldMatrix;
		}

		objectWvpData_[objectIndex]->World = partsMatrix;
		objectWvpData_[objectIndex]->WorldInverseTranspose = Transpose(Inverse(partsMatrix));
		Matrix4x4 worldViewProjectionMatrix = partsMatrix * object->GetCamera()->GetViewMatrix() * object->GetCamera()->GetProjectionMatrix();
		objectWvpData_[objectIndex]->WVP = worldViewProjectionMatrix;

		objectWvpResource_[objectIndex]->Unmap(0, nullptr);

		parts[i].material->uvTransform = MakeQuaternionMatrix(parts[i].UVtransform.scale, parts[i].UVtransform.rotate, parts[i].UVtransform.translate);
		parts[i].material->enableDirectionalLighting = directionalLight != nullptr;
		parts[i].material->enablePointLighting = pointLight != nullptr;
		parts[i].material->enableSpotLighting = spotLight != nullptr;

		//マテリアルデータを更新
		objectMaterialResource_[objectIndex]->Map(0, nullptr, reinterpret_cast<void**>(&objectMaterialData_[objectIndex]));

		*objectMaterialData_[objectIndex] = *parts[i].material;

		objectMaterialResource_[objectIndex]->Unmap(0, nullptr);

		//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
		commandList_->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(parts[i].textureIndex));

		//ライティングが必要な場合CBufferに送る
		if (parts[i].material->reflection != 0 && directionalLight != nullptr) {
			commandList_->SetGraphicsRootConstantBufferView(3, directionalLight->DirectionalLightElementResource()->GetGPUVirtualAddress());	//DirectionalLighting
		}
		if (parts[i].material->reflection != 0 && pointLight != nullptr) {
			commandList_->SetGraphicsRootConstantBufferView(5, pointLight->PointLightElementResource()->GetGPUVirtualAddress());	//PointLighting
		}
		if (parts[i].material->reflection != 0 && spotLight != nullptr) {
			commandList_->SetGraphicsRootConstantBufferView(6, spotLight->SpotLightElementResource()->GetGPUVirtualAddress());	//SpotLighting
		}

		//マテリアルCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView(0, objectMaterialResource_[objectIndex]->GetGPUVirtualAddress());
		//wvp用のCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView(1, objectWvpResource_[objectIndex]->GetGPUVirtualAddress());

		//描画(DrawCall)
		commandList_->DrawIndexedInstanced(offsets[i].indexCount, 1, 0, offsets[i].vertexStart, 0);

		objectIndex++;
	}
}

void GameEngine::DrawObject_2D_(Object* object, DirectionalLight* directionalLight) {

	//上限に達していたら描画しない
	if (objectIndex >= kMaxIndex)return;

	std::vector<Parts> parts = object->GetParts();
	std::vector<Offset> offsets = object->GetOffsets();

	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth_) / float(kWindowHeight_), 0.01f, 1.0f);

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->SetPipelineState(trianglePipelineState_.Get());	//PSOを設定

	commandList_->IASetVertexBuffers(0, 1, &object->GetVBV());	//VBVを設定
	commandList_->IASetIndexBuffer(&object->GetIBV());	//IBVを設定

	//カメラのワールド座標をCBufferに送る
	commandList_->SetGraphicsRootConstantBufferView(4, object->GetCamera()->CameraResource()->GetGPUVirtualAddress());

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//オブジェクトのワールド座標
	Matrix4x4 worldMatrix = MakeQuaternionMatrix(object->GetTransform().scale, object->GetTransform().rotate, object->GetTransform().translate);

	//変更が必要な部分だけ変える
	for (int i = 0; i < parts.size(); i++) {

		//WVPデータを更新
		objectWvpResource_[objectIndex]->Map(0, nullptr, reinterpret_cast<void**>(&objectWvpData_[objectIndex]));

		Matrix4x4 partsMatrix = MakeQuaternionMatrix(parts[i].transform->scale, parts[i].transform->rotate, parts[i].transform->translate);
		if (parts[i].parent) {
			//親を持つPartsのローカル座標
			Matrix4x4 parentMatrix = MakeQuaternionMatrix(parts[i].parent->scale, parts[i].parent->rotate, parts[i].parent->translate);
			partsMatrix = partsMatrix * parentMatrix;
		} else {
			//ワールド座標を親に持つPartsのローカル座標
			partsMatrix = partsMatrix * worldMatrix;
		}

		objectWvpData_[objectIndex]->World = partsMatrix;
		objectWvpData_[objectIndex]->WorldInverseTranspose = Transpose(Inverse(partsMatrix));
		Matrix4x4 worldViewProjectionMatrix = partsMatrix * viewMatrix * projectionMatrix;
		objectWvpData_[objectIndex]->WVP = worldViewProjectionMatrix;

		objectWvpResource_[objectIndex]->Unmap(0, nullptr);

		parts[i].material->uvTransform = MakeQuaternionMatrix(parts[i].UVtransform.scale, parts[i].UVtransform.rotate, parts[i].UVtransform.translate);
		parts[i].material->enableDirectionalLighting = directionalLight != nullptr;
		parts[i].material->enablePointLighting = false;
		parts[i].material->enableSpotLighting = false;

		//マテリアルデータを更新
		objectMaterialResource_[objectIndex]->Map(0, nullptr, reinterpret_cast<void**>(&objectMaterialData_[objectIndex]));

		*objectMaterialData_[objectIndex] = *parts[i].material;

		objectMaterialResource_[objectIndex]->Unmap(0, nullptr);

		//ライティングが必要な場合CBufferに送る
		if (parts[i].material->reflection != 0 && directionalLight != nullptr) {
			commandList_->SetGraphicsRootConstantBufferView(3, directionalLight->DirectionalLightElementResource()->GetGPUVirtualAddress());	//DirectionalLighting
		}

		//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
		commandList_->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(parts[i].textureIndex));

		//マテリアルCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView(0, objectMaterialResource_[objectIndex]->GetGPUVirtualAddress());
		//wvp用のCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView(1, objectWvpResource_[objectIndex]->GetGPUVirtualAddress());

		//描画(DrawCall)
		commandList_->DrawIndexedInstanced(offsets[i].indexCount, 1, 0, offsets[i].vertexStart, 0);

		objectIndex++;
	}
}

void GameEngine::DrawInstancingObject_3D_(std::list<Object*> objects, DirectionalLight* directionalLight, PointLight* pointLight, SpotLight* spotLight) {
	
	//上限に達していたら描画しない
	if (instancingObjectIndex > kMaxInstanceIndex)return;

	std::list<Object*>::iterator objectIterator = objects.begin();
	Camera* camera = (*objectIterator)->GetCamera();

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(instancingRootSignature_.Get());
	commandList_->SetPipelineState(instancingTrianglePipelineState_.Get());	//PSOを設定

	commandList_->IASetVertexBuffers(0, 1, &(*objectIterator)->GetVBV());	//VBVを設定
	commandList_->IASetIndexBuffer(&(*objectIterator)->GetIBV());	//IBVを設定

	//カメラのワールド座標をCBufferに送る
	commandList_->SetGraphicsRootConstantBufferView(4, (*objectIterator)->GetCamera()->CameraResource()->GetGPUVirtualAddress());

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//パーツの数。共通なので無駄に参照しないように
	size_t numParts = (*objectIterator)->GetParts().size();

	uint32_t numInstance = 0;
	std::vector<Matrix4x4> worldMatries;
	std::vector<std::vector<Parts>> parts;
	std::vector<Offset> offsets = (*objectIterator)->GetOffsets();

	//それぞれの情報をまとめる
	for (objectIterator = objects.begin();
		objectIterator != objects.end();) {

		if (numInstance >= kMaxNumInstance)break;

		parts.push_back((*objectIterator)->GetParts());

		//オブジェクトのワールド座標
		Matrix4x4 worldMatrix = MakeQuaternionMatrix((*objectIterator)->GetTransform().scale, (*objectIterator)->GetTransform().rotate, (*objectIterator)->GetTransform().translate);

		worldMatries.push_back(worldMatrix);

		++numInstance;
		++objectIterator;
	}
	
	//パーツごとにインスタシング描画
	for (uint32_t i = 0; i < numParts; i++) {

		//WVPデータを更新
		InstancingTransformationMatrix* mappedBase = nullptr;
		instancingObjectResource_[instancingObjectIndex]->Map(0, nullptr, reinterpret_cast<void**>(&mappedBase));
		// mappedBase が nullptr でないかチェック
		if (mappedBase == nullptr) {
			assert(0);
		}
		// 各要素ポインタを mappedBase に初期化
		for (uint32_t j = 0; j < kMaxNumInstance; ++j) {
			instancingObjectData_[instancingObjectIndex][j] = mappedBase + j;
		}

		numInstance = 0;
		for (objectIterator = objects.begin();
			objectIterator != objects.end(); ++objectIterator) {

			if (numInstance >= kMaxNumInstance)break;

			//ワールド座標を親に持つPartsのローカル座標
			Matrix4x4 partsMatrix = MakeQuaternionMatrix(parts[numInstance][i].transform->scale, parts[numInstance][i].transform->rotate, parts[numInstance][i].transform->translate);

			if (parts[numInstance][i].parent) {
				//親を持つPartsのローカル座標
				Matrix4x4 parentMatrix = MakeQuaternionMatrix(parts[numInstance][i].parent->scale, parts[numInstance][i].parent->rotate, parts[numInstance][i].parent->translate);
				partsMatrix = partsMatrix * parentMatrix;
			} else {
				//ワールド座標を親に持つPartsのローカル座標
				partsMatrix = partsMatrix * worldMatries[numInstance];
			}

			instancingObjectData_[instancingObjectIndex][numInstance]->World = partsMatrix;
			instancingObjectData_[instancingObjectIndex][numInstance]->WorldInverseTranspose = Transpose(Inverse(partsMatrix));
			Matrix4x4 worldViewProjectionMatrix = partsMatrix * camera->GetViewMatrix() * camera->GetProjectionMatrix();
			instancingObjectData_[instancingObjectIndex][numInstance]->WVP = worldViewProjectionMatrix;
			instancingObjectData_[instancingObjectIndex][numInstance]->color = parts[numInstance][i].material->color;

			++numInstance;
		}

		instancingObjectResource_[instancingObjectIndex]->Unmap(0, nullptr);

		//インスタシング描画の都合上マテリアルは先頭のもの全てに適応
		parts[0][i].material->uvTransform = MakeQuaternionMatrix(parts[0][i].UVtransform.scale, parts[0][i].UVtransform.rotate, parts[0][i].UVtransform.translate);
		parts[0][i].material->enableDirectionalLighting = directionalLight != nullptr;
		parts[0][i].material->enablePointLighting = pointLight != nullptr;
		parts[0][i].material->enableSpotLighting = spotLight != nullptr;
		parts[0][i].material->color = Vector4{ 1.0f,1.0f,1.0f,1.0f };

		//マテリアルデータを更新
		objectMaterialResource_[instancingObjectIndex]->Map(0, nullptr, reinterpret_cast<void**>(&objectMaterialData_[instancingObjectIndex]));

		*objectMaterialData_[instancingObjectIndex] = *parts[0][i].material;

		objectMaterialResource_[instancingObjectIndex]->Unmap(0, nullptr);

		//ライティングが必要な場合CBufferに送る
		if (parts[0][i].material->reflection != 0 && directionalLight != nullptr) {
			commandList_->SetGraphicsRootConstantBufferView(3, directionalLight->DirectionalLightElementResource()->GetGPUVirtualAddress());	//DirectionalLighting
		}
		if (parts[0][i].material->reflection != 0 && pointLight != nullptr) {
			commandList_->SetGraphicsRootConstantBufferView(5, pointLight->PointLightElementResource()->GetGPUVirtualAddress());	//PointLighting
		}
		if (parts[0][i].material->reflection != 0 && spotLight != nullptr) {
			commandList_->SetGraphicsRootConstantBufferView(6, spotLight->SpotLightElementResource()->GetGPUVirtualAddress());	//SpotLighting
		}

		//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
		commandList_->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(parts[0][i].textureIndex));

		commandList_->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(startInstancingObjectIndex + instancingObjectIndex));

		//マテリアルCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView(0, objectMaterialResource_[instancingObjectIndex]->GetGPUVirtualAddress());
		
		//描画(DrawCall)
		commandList_->DrawIndexedInstanced(offsets[i].indexCount, numInstance, 0, offsets[i].vertexStart, 0);

		instancingObjectIndex++;
	}
}

void GameEngine::DrawParticle_(ParticleGroup particleGroup) {

	//上限に達していたら描画しない
	if (particleIndex > kMaxInstanceIndex)return;

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(instancingRootSignature_.Get());
	commandList_->SetPipelineState(particlePipelineState_.Get());	//PSOを設定

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList_->IASetVertexBuffers(0, 1, &ParticleManager::GetInstance()->GetVertexBufferView());	//VBVを設定
	commandList_->IASetIndexBuffer(&ParticleManager::GetInstance()->GetIndexBufferView());	//IBVを設定

	Camera* camera = Object::GetDefaultCamera();

	//WVPデータを更新
	InstancingTransformationMatrix* mappedBase = nullptr;
	particleGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedBase));
	// mappedBase が nullptr でないかチェック
	if (mappedBase == nullptr) {
		assert(0);
	}
	// 各要素ポインタを mappedBase に初期化
	for (uint32_t j = 0; j < kMaxNumInstance; ++j) {
		particleData_[particleIndex][j] = mappedBase + j;
	}

	uint32_t numInstance = 0;
	for (std::list<Particle>::iterator particleIterator = particleGroup.particles.begin();
		particleIterator != particleGroup.particles.end();) {

		if (numInstance >= kMaxNumInstance)break;

		Matrix4x4 cameraMatrix = Inverse(camera->GetViewMatrix());

		Matrix4x4 worldMatrix = cameraMatrix;
		worldMatrix.m[3][0] = (*particleIterator).transform.translate.x;
		worldMatrix.m[3][1] = (*particleIterator).transform.translate.y;
		worldMatrix.m[3][2] = (*particleIterator).transform.translate.z;
		for (int i = 0; i < 3; i++) {
			worldMatrix.m[0][i] *= (*particleIterator).transform.scale.x;
		}
		for (int i = 0; i < 3; i++) {
			worldMatrix.m[1][i] *= (*particleIterator).transform.scale.y;
		}
		for (int i = 0; i < 3; i++) {
			worldMatrix.m[2][i] *= (*particleIterator).transform.scale.z;
		}

		particleData_[particleIndex][numInstance]->World = worldMatrix;
		particleData_[particleIndex][numInstance]->WorldInverseTranspose = Transpose(Inverse(worldMatrix));
		Matrix4x4 worldViewProjectionMatrix = worldMatrix * camera->GetViewMatrix() * camera->GetProjectionMatrix();
		particleData_[particleIndex][numInstance]->WVP = worldViewProjectionMatrix;
		particleData_[particleIndex][numInstance]->color = (*particleIterator).color;

		++numInstance;
		++particleIterator;
	}

	particleGroup.instancingResource->Unmap(0, nullptr);

	//マテリアルデータを更新
	particleMaterialResource_[particleIndex]->Map(0, nullptr, reinterpret_cast<void**>(&primitiveMaterialData_[particleIndex]));

	primitiveMaterialData_[particleIndex]->uvTransform = MakeIdentity4x4();
	primitiveMaterialData_[particleIndex]->enableDirectionalLighting = false;
	primitiveMaterialData_[particleIndex]->enablePointLighting = false;
	primitiveMaterialData_[particleIndex]->enableSpotLighting = false;
	primitiveMaterialData_[particleIndex]->reflection = 0;
	primitiveMaterialData_[particleIndex]->shininess = 0;
	primitiveMaterialData_[particleIndex]->color = {1.0f,1.0f,1.0f,1.0f};

	particleMaterialResource_[particleIndex]->Unmap(0, nullptr);

	//マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, particleMaterialResource_[particleIndex]->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	commandList_->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(particleGroup.textureIndex));

	commandList_->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(particleGroup.instancingIndex));

	//描画(DrawCall)
	commandList_->DrawIndexedInstanced(6, numInstance, 0, 0, 0);

	particleIndex++;
}

void GameEngine::DrawSprite_2D_(Sprite* sprite) {

	//上限に達していたら描画しない
	if (spriteIndex > kMaxIndex)return;

	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(kWindowWidth_), float(kWindowHeight_), 0.0f, 100.0f);

	//WVPデータを更新
	spriteWvpResource_[spriteIndex]->Map(0, nullptr, reinterpret_cast<void**>(&spriteWvpData_[spriteIndex]));

	Matrix4x4 worldMatrix = MakeQuaternionMatrix(sprite->GetTransform().scale, sprite->GetTransform().rotate, sprite->GetTransform().translate);
	spriteWvpData_[spriteIndex]->World = worldMatrix;
	spriteWvpData_[spriteIndex]->WorldInverseTranspose = Transpose(Inverse(worldMatrix));

	Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
	spriteWvpData_[spriteIndex]->WVP = worldViewProjectionMatrix;

	spriteWvpResource_[spriteIndex]->Unmap(0, nullptr);

	//マテリアルデータを更新
	spriteMaterialResource_[spriteIndex]->Map(0, nullptr, reinterpret_cast<void**>(&spriteMaterialData_[spriteIndex]));

	spriteMaterialData_[spriteIndex]->color = sprite->GetColor();
	spriteMaterialData_[spriteIndex]->uvTransform = MakeQuaternionMatrix(sprite->GetUVTransform().scale, sprite->GetUVTransform().rotate, sprite->GetUVTransform().translate);
	spriteMaterialData_[spriteIndex]->reflection = 0;
	spriteMaterialData_[spriteIndex]->enableDirectionalLighting = false;
	spriteMaterialData_[spriteIndex]->enablePointLighting = false;
	spriteMaterialData_[spriteIndex]->enableSpotLighting = false;
	spriteMaterialData_[spriteIndex]->shininess = 0.0f;

	spriteMaterialResource_[spriteIndex]->Unmap(0, nullptr);

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->SetPipelineState(spritePipelineState_.Get());	//PSOを設定
	commandList_->IASetVertexBuffers(0, 1, &sprite->GetVBV());	//VBVを設定
	commandList_->IASetIndexBuffer(&sprite->GetIBV());	//IBVを設定
	commandList_->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(sprite->GetTextureIndex()));
	//マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, spriteMaterialResource_[spriteIndex]->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, spriteWvpResource_[spriteIndex]->GetGPUVirtualAddress());
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//ドローコール
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);

	spriteIndex++;
}

void GameEngine::DrawInstancingSprite_2D_(std::list<Sprite*> sprits) {

	//上限に達していたら描画しない
	if (instancingSpriteIndex > kMaxInstanceIndex)return;

	std::list<Sprite*>::iterator SpriteIterator = sprits.begin();
	Sprite* startSprite = (*SpriteIterator);

	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(kWindowWidth_), float(kWindowHeight_), 0.0f, 100.0f);

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(instancingRootSignature_.Get());
	commandList_->SetPipelineState(instancingTrianglePipelineState_.Get());	//PSOを設定

	commandList_->IASetVertexBuffers(0, 1, &startSprite->GetVBV());	//VBVを設定
	commandList_->IASetIndexBuffer(&startSprite->GetIBV());	//IBVを設定

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//WVPデータを更新
	InstancingTransformationMatrix* mappedBase = nullptr;
	instancingSpriteResource_[instancingSpriteIndex]->Map(0, nullptr, reinterpret_cast<void**>(&mappedBase));
	// mappedBase が nullptr でないかチェック
	if (mappedBase == nullptr) {
		assert(0);
	}
	// 各要素ポインタを mappedBase に初期化
	for (uint32_t j = 0; j < kMaxNumInstance; ++j) {
		instancingSpriteData_[instancingSpriteIndex][j] = mappedBase + j;
	}

	uint32_t numInstance = 0;
	//それぞれの情報をまとめる
	for (SpriteIterator = sprits.begin();
		SpriteIterator != sprits.end();) {

		if (numInstance >= kMaxNumInstance)break;

		Matrix4x4 worldMatrix = MakeQuaternionMatrix((*SpriteIterator)->GetTransform().translate, (*SpriteIterator)->GetTransform().rotate, (*SpriteIterator)->GetTransform().translate);
		instancingSpriteData_[instancingSpriteIndex][numInstance]->World = worldMatrix;
		instancingSpriteData_[instancingSpriteIndex][numInstance]->WorldInverseTranspose = Transpose(Inverse(worldMatrix));
		Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
		instancingSpriteData_[instancingSpriteIndex][numInstance]->WVP = worldViewProjectionMatrix;

		instancingSpriteResource_[instancingSpriteIndex]->Unmap(0, nullptr);

		++numInstance;
		++SpriteIterator;
	}

	Material material = startSprite->GetMaterial();
	SRT UVTransform = startSprite->GetUVTransform();

	material.uvTransform = MakeQuaternionMatrix(UVTransform.scale, UVTransform.rotate, UVTransform.translate);
	material.reflection = 0;
	material.enableDirectionalLighting = false;
	material.enablePointLighting = false;
	material.enableSpotLighting = false;
	material.shininess = 0.0f;
	material.color = Vector4{ 1.0f,1.0f,1.0f,1.0f };

	//マテリアルデータを更新
	spriteMaterialResource_[instancingSpriteIndex]->Map(0, nullptr, reinterpret_cast<void**>(&spriteMaterialData_[instancingSpriteIndex]));

	*spriteMaterialData_[instancingSpriteIndex] = material;

	spriteMaterialResource_[instancingSpriteIndex]->Unmap(0, nullptr);

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	commandList_->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(startSprite->GetTextureIndex()));

	commandList_->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(startInstancingSpriteIndex + instancingSpriteIndex));

	//マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, spriteMaterialResource_[instancingSpriteIndex]->GetGPUVirtualAddress());

	//描画(DrawCall)
	commandList_->DrawIndexedInstanced(6, numInstance, 0, 0, 0);

	instancingSpriteIndex++;
}

void GameEngine::DrawLine_(std::list<Line> lines, PrimitiveManager::PrimitiveResource primitiveResource) {
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(instancingRootSignature_.Get());
	commandList_->SetPipelineState(noDepthLinePipelineState_.Get());	//PSOを設定

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	commandList_->IASetVertexBuffers(0, 1, &PrimitiveManager::GetInstance()->GetVertexBufferView());	//VBVを設定
	commandList_->IASetIndexBuffer(&PrimitiveManager::GetInstance()->GetIndexBufferView());	//IBVを設定

	Camera* camera = Object::GetDefaultCamera();

	//WVPデータを更新
	InstancingTransformationMatrix* mappedBase = nullptr;
	primitiveResource_[PrimitiveManager::SHAPE_Plane]->Map(0, nullptr, reinterpret_cast<void**>(&mappedBase));
	// mappedBase が nullptr でないかチェック
	if (mappedBase == nullptr) {
		assert(0);
	}
	// 各要素ポインタを mappedBase に初期化
	for (uint32_t j = 0; j < PrimitiveManager::kMaxNumPrimitive; ++j) {
		primitiveData_[PrimitiveManager::SHAPE_Plane][j] = mappedBase + j;
	}

	uint32_t numInstance = 0;
	for (std::list<Line>::iterator lineIterator = lines.begin();
		lineIterator != lines.end(); ++lineIterator) {

		if (numInstance >= PrimitiveManager::kMaxNumPrimitive)break;

		SRT transform{};
		transform.scale.x = Length((*lineIterator).diff);	//直線の長さ
		transform.scale.y = Length((*lineIterator).diff);	//直線の長さ
		transform.scale.z = Length((*lineIterator).diff);	//直線の長さ
		//  Y軸回り回転(θy)
		transform.rotate.y = std::atan2((*lineIterator).diff.x, (*lineIterator).diff.z);
		float length = Length(Vector3{(*lineIterator).diff.x, 0.0f, (*lineIterator).diff.z });
		// X軸回り回転(θx)
		transform.rotate.x = std::atan2(-(*lineIterator).diff.y, length);
		transform.translate = (*lineIterator).origin;	//直線の開始地点

		Matrix4x4 worldMatrix = MakeQuaternionMatrix(transform.scale, transform.rotate, transform.translate);

		primitiveData_[PrimitiveManager::SHAPE_Plane][numInstance]->World = worldMatrix;
		primitiveData_[PrimitiveManager::SHAPE_Plane][numInstance]->WorldInverseTranspose = Transpose(Inverse(worldMatrix));
		Matrix4x4 worldViewProjectionMatrix = worldMatrix * camera->GetViewMatrix() * camera->GetProjectionMatrix();
		primitiveData_[PrimitiveManager::SHAPE_Plane][numInstance]->WVP = worldViewProjectionMatrix;
		primitiveData_[PrimitiveManager::SHAPE_Plane][numInstance]->color = Vector4{1.0f,0.0f,0.0f,1.0f};

		++numInstance;
	}

	primitiveResource_[PrimitiveManager::SHAPE_Plane]->Unmap(0, nullptr);

	//マテリアルデータを更新
	primitiveMaterialResource_[PrimitiveManager::SHAPE_Plane]->Map(0, nullptr, reinterpret_cast<void**>(&primitiveMaterialData_[PrimitiveManager::SHAPE_Plane]));

	primitiveMaterialData_[PrimitiveManager::SHAPE_Plane]->uvTransform = MakeIdentity4x4();
	primitiveMaterialData_[PrimitiveManager::SHAPE_Plane]->enableDirectionalLighting = false;
	primitiveMaterialData_[PrimitiveManager::SHAPE_Plane]->enablePointLighting = false;
	primitiveMaterialData_[PrimitiveManager::SHAPE_Plane]->enableSpotLighting = false;
	primitiveMaterialData_[PrimitiveManager::SHAPE_Plane]->reflection = 0;
	primitiveMaterialData_[PrimitiveManager::SHAPE_Plane]->shininess = 0;
	primitiveMaterialData_[PrimitiveManager::SHAPE_Plane]->color = { 1.0f,1.0f,1.0f,1.0f };

	primitiveMaterialResource_[PrimitiveManager::SHAPE_Plane]->Unmap(0, nullptr);

	//マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, primitiveMaterialResource_[PrimitiveManager::SHAPE_Plane]->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	commandList_->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(TextureManager::GetInstance()->GetWhite2x2()));

	commandList_->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(primitiveResource.instancingIndex));

	// SRV を作成（NumElements と stride は一致させる）
	srvManager_->CreateSRVforStructuredBuffer(primitiveResource.instancingIndex, primitiveResource_[PrimitiveManager::SHAPE_Plane].Get(), PrimitiveManager::kMaxNumPrimitive, sizeof(InstancingTransformationMatrix));
	//描画(DrawCall)
	commandList_->DrawIndexedInstanced(primitiveResource.offset.indexCount, numInstance, primitiveResource.offset.indexStart, 0, 0);
}

void GameEngine::DrawPoint_(std::list<Vector3> points, PrimitiveManager::PrimitiveResource primitiveResource) {
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(instancingRootSignature_.Get());
	commandList_->SetPipelineState(noDepthLinePipelineState_.Get());	//PSOを設定

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	commandList_->IASetVertexBuffers(0, 1, &PrimitiveManager::GetInstance()->GetVertexBufferView());	//VBVを設定
	commandList_->IASetIndexBuffer(&PrimitiveManager::GetInstance()->GetIndexBufferView());	//IBVを設定

	Camera* camera = Object::GetDefaultCamera();

	//WVPデータを更新
	InstancingTransformationMatrix* mappedBase = nullptr;
	primitiveResource_[PrimitiveManager::SHAPE_Point]->Map(0, nullptr, reinterpret_cast<void**>(&mappedBase));
	// mappedBase が nullptr でないかチェック
	if (mappedBase == nullptr) {
		assert(0);
	}
	// 各要素ポインタを mappedBase に初期化
	for (uint32_t j = 0; j < PrimitiveManager::kMaxNumPrimitive; ++j) {
		primitiveData_[PrimitiveManager::SHAPE_Point][j] = mappedBase + j;
	}

	uint32_t numInstance = 0;
	for (std::list<Vector3>::iterator pointIterator = points.begin();
		pointIterator != points.end(); ++pointIterator) {

		if (numInstance >= PrimitiveManager::kMaxNumPrimitive)break;

		SRT transform{};
		transform.scale = Vector3{ 1.0f,1.0f,1.0f };
		transform.translate = (*pointIterator);	//特に言うことはない

		Matrix4x4 worldMatrix = MakeQuaternionMatrix(transform.scale, transform.rotate, transform.translate);

		primitiveData_[PrimitiveManager::SHAPE_Point][numInstance]->World = worldMatrix;
		primitiveData_[PrimitiveManager::SHAPE_Point][numInstance]->WorldInverseTranspose = Transpose(Inverse(worldMatrix));
		Matrix4x4 worldViewProjectionMatrix = worldMatrix * camera->GetViewMatrix() * camera->GetProjectionMatrix();
		primitiveData_[PrimitiveManager::SHAPE_Point][numInstance]->WVP = worldViewProjectionMatrix;
		primitiveData_[PrimitiveManager::SHAPE_Point][numInstance]->color = Vector4{ 1.0f,0.0f,0.0f,1.0f };

		++numInstance;
	}

	primitiveResource_[PrimitiveManager::SHAPE_Point]->Unmap(0, nullptr);

	//マテリアルデータを更新
	primitiveMaterialResource_[PrimitiveManager::SHAPE_Point]->Map(0, nullptr, reinterpret_cast<void**>(&primitiveMaterialData_[PrimitiveManager::SHAPE_Point]));

	primitiveMaterialData_[PrimitiveManager::SHAPE_Point]->uvTransform = MakeIdentity4x4();
	primitiveMaterialData_[PrimitiveManager::SHAPE_Point]->enableDirectionalLighting = false;
	primitiveMaterialData_[PrimitiveManager::SHAPE_Point]->enablePointLighting = false;
	primitiveMaterialData_[PrimitiveManager::SHAPE_Point]->enableSpotLighting = false;
	primitiveMaterialData_[PrimitiveManager::SHAPE_Point]->reflection = 0;
	primitiveMaterialData_[PrimitiveManager::SHAPE_Point]->shininess = 0;
	primitiveMaterialData_[PrimitiveManager::SHAPE_Point]->color = { 1.0f,1.0f,1.0f,1.0f };

	primitiveMaterialResource_[PrimitiveManager::SHAPE_Point]->Unmap(0, nullptr);

	//マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, primitiveMaterialResource_[PrimitiveManager::SHAPE_Point]->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	commandList_->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(TextureManager::GetInstance()->GetWhite2x2()));

	commandList_->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(primitiveResource.instancingIndex));

	// SRV を作成（NumElements と stride は一致させる）
	srvManager_->CreateSRVforStructuredBuffer(primitiveResource.instancingIndex, primitiveResource_[PrimitiveManager::SHAPE_Point].Get(), PrimitiveManager::kMaxNumPrimitive, sizeof(InstancingTransformationMatrix));
	//描画(DrawCall)
	commandList_->DrawIndexedInstanced(primitiveResource.offset.indexCount, numInstance, primitiveResource.offset.indexStart, 0, 0);
}

void GameEngine::DrawAABB_(std::list<AABB> aabbs, PrimitiveManager::PrimitiveResource primitiveResource) {
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(instancingRootSignature_.Get());
	commandList_->SetPipelineState(noDepthLinePipelineState_.Get());	//PSOを設定

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	commandList_->IASetVertexBuffers(0, 1, &PrimitiveManager::GetInstance()->GetVertexBufferView());	//VBVを設定
	commandList_->IASetIndexBuffer(&PrimitiveManager::GetInstance()->GetIndexBufferView());	//IBVを設定

	Camera* camera = Object::GetDefaultCamera();

	//WVPデータを更新
	InstancingTransformationMatrix* mappedBase = nullptr;
	primitiveResource_[PrimitiveManager::SHAPE_AABB]->Map(0, nullptr, reinterpret_cast<void**>(&mappedBase));
	// mappedBase が nullptr でないかチェック
	if (mappedBase == nullptr) {
		assert(0);
	}
	// 各要素ポインタを mappedBase に初期化
	for (uint32_t j = 0; j < PrimitiveManager::kMaxNumPrimitive; ++j) {
		primitiveData_[PrimitiveManager::SHAPE_AABB][j] = mappedBase + j;
	}

	uint32_t numInstance = 0;
	for (std::list<AABB>::iterator aabbIterator = aabbs.begin();
		aabbIterator != aabbs.end(); ++aabbIterator) {

		if (numInstance >= PrimitiveManager::kMaxNumPrimitive)break;

		SRT transform{};
		transform.scale = Vector3{ 1.0f,1.0f,1.0f };
		transform.translate = (*aabbIterator).min;	//AABBの開始地点
		transform.scale = (*aabbIterator).max - (*aabbIterator).min;	//AABBの終了地点

		Matrix4x4 worldMatrix = MakeQuaternionMatrix(transform.scale, transform.rotate, transform.translate);

		primitiveData_[PrimitiveManager::SHAPE_AABB][numInstance]->World = worldMatrix;
		primitiveData_[PrimitiveManager::SHAPE_AABB][numInstance]->WorldInverseTranspose = Transpose(Inverse(worldMatrix));
		Matrix4x4 worldViewProjectionMatrix = worldMatrix * camera->GetViewMatrix() * camera->GetProjectionMatrix();
		primitiveData_[PrimitiveManager::SHAPE_AABB][numInstance]->WVP = worldViewProjectionMatrix;
		primitiveData_[PrimitiveManager::SHAPE_AABB][numInstance]->color = Vector4{ 1.0f,0.0f,0.0f,1.0f };

		++numInstance;
	}

	primitiveResource_[PrimitiveManager::SHAPE_AABB]->Unmap(0, nullptr);

	//マテリアルデータを更新
	primitiveMaterialResource_[PrimitiveManager::SHAPE_AABB]->Map(0, nullptr, reinterpret_cast<void**>(&primitiveMaterialData_[PrimitiveManager::SHAPE_AABB]));

	primitiveMaterialData_[PrimitiveManager::SHAPE_AABB]->uvTransform = MakeIdentity4x4();
	primitiveMaterialData_[PrimitiveManager::SHAPE_AABB]->enableDirectionalLighting = false;
	primitiveMaterialData_[PrimitiveManager::SHAPE_AABB]->enablePointLighting = false;
	primitiveMaterialData_[PrimitiveManager::SHAPE_AABB]->enableSpotLighting = false;
	primitiveMaterialData_[PrimitiveManager::SHAPE_AABB]->reflection = 0;
	primitiveMaterialData_[PrimitiveManager::SHAPE_AABB]->shininess = 0;
	primitiveMaterialData_[PrimitiveManager::SHAPE_AABB]->color = { 1.0f,1.0f,1.0f,1.0f };

	primitiveMaterialResource_[PrimitiveManager::SHAPE_AABB]->Unmap(0, nullptr);

	//マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, primitiveMaterialResource_[PrimitiveManager::SHAPE_AABB]->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	commandList_->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(TextureManager::GetInstance()->GetWhite2x2()));

	commandList_->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(primitiveResource.instancingIndex));

	// SRV を作成（NumElements と stride は一致させる）
	srvManager_->CreateSRVforStructuredBuffer(primitiveResource.instancingIndex, primitiveResource_[PrimitiveManager::SHAPE_AABB].Get(), PrimitiveManager::kMaxNumPrimitive, sizeof(InstancingTransformationMatrix));
	//描画(DrawCall)
	commandList_->DrawIndexedInstanced(primitiveResource.offset.indexCount, numInstance, primitiveResource.offset.indexStart, 0, 0);
}