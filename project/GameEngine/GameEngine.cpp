#include "GameEngine.h"

#define _USE_MATH_DEFINES
#include <cmath>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxcompiler.lib")
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
#include "Matrix4x4_operation.h"

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

	delete winApp_;
	delete dxCommon_;
	TextureManager::GetInstance()->Finalize();
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

	TextureManager::GetInstance()->Initialize(dxCommon_);

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
	instancingTrianglePipelineState_ = TrianglePipelineStateInitialvalue(device_, instancingRootSignature_, instancingVertexShaderBlob.Get(), pixelShaderBlob.Get());
	particlePipelineState_ = NoDepthAddBlendTrianglePipelineStateInitialvalue(device_, instancingRootSignature_, particleVSBlob.Get(), particlePSBlob.Get());
	linePipelineState_ = LinePipelineStateInitialvalue(device_, rootSignature_, vertexShaderBlob.Get(), pixelShaderBlob.Get());
	spritePipelineState_ = SpritePipelineStateInitialvalue(device_, rootSignature_, vertexShaderBlob.Get(), pixelShaderBlob.Get());

	//XAudioエンジンのインスタンスを生成
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));

	//マスターボイスを生成
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));

	//乱数シード初期化
	std::random_device seedGenerator;
	randomEngine_.seed(seedGenerator());

	//テクスチャ初期値としてwhite2x2を読み込む
	TextureManager::GetInstance()->LoadTexture("resources/DebugResources/white2x2.png");

	//初期化
	for (int i = 0; i < kMaxIndex; i++) {
		objectMaterialResource_[i] = dxCommon_->CreateBufferResources(sizeof(Material));
		objectWvpResource_[i] = dxCommon_->CreateBufferResources(sizeof(TransformationMatrix));
		spriteMaterialResource_[i] = dxCommon_->CreateBufferResources(sizeof(Material));
		spriteWvpResource_[i] = dxCommon_->CreateBufferResources(sizeof(TransformationMatrix));
		instancingSpriteMaterialResource_[i] = dxCommon_->CreateBufferResources(sizeof(Material));
		instancingSpriteResource_[i] = dxCommon_->CreateBufferResources(sizeof(TransformationMatrix));
	}

}

D3D12_GPU_DESCRIPTOR_HANDLE GameEngine::GetInstancingSRV_(Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource, int32_t numInstance) {
	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;	//バッファ
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = numInstance;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);

	//SRVを作成するDescriptorHeapの場所を決める。ImGuiが最初を使うのでその次を使う
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon_->GetSRVCPUDescriptorHandle(1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxCommon_->GetSRVGPUDescriptorHandle(1);
	//SRVの生成
	device_->CreateShaderResourceView(instancingResource.Get(), &instancingSrvDesc, textureSrvHandleCPU);

	return textureSrvHandleGPU;

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

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

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
	spriteIndex = 0;

	dxCommon_->PreDraw();

}

void GameEngine::PostDraw_() {

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

void GameEngine::DrawObject_3D_(Object* object, Camera* camera, int reflection, float shininess, DirectionalLight* directionalLight, PointLight* pointLight, SpotLight* spotLight) {
	std::vector<Parts> parts = object->GetParts();
	SRT transform = object->GetTransform();
	
	for (INT i = 0; i < parts.size(); i++) {
		//WVPデータを更新
		objectWvpResource_[objectIndex]->Map(0, nullptr, reinterpret_cast<void**>(&objectWvpData_[objectIndex]));

		Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		objectWvpData_[objectIndex]->World = worldMatrix;
		objectWvpData_[objectIndex]->WorldInverseTranspose = Inverse(worldMatrix);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera->GetViewMatrix(), camera->GetProjectionMatrix()));
		objectWvpData_[objectIndex]->WVP = worldViewProjectionMatrix;

		objectWvpResource_[objectIndex]->Unmap(0, nullptr);

		parts[i].material.color = {1.0f,1.0f,1.0f,1.0f};
		parts[i].material.uvTransform = MakeIdentity4x4();
		parts[i].material.reflection = reflection;
		if (directionalLight != nullptr) {
			parts[i].material.enableDirectionalLighting = true;
		} else {
			parts[i].material.enableDirectionalLighting = false;
		}
		if (pointLight != nullptr) {
			parts[i].material.enablePointLighting = true;
		} else {
			parts[i].material.enablePointLighting = false;
		}
		if (spotLight != nullptr) {
			parts[i].material.enableSpotLighting = true;
		} else {
			parts[i].material.enableSpotLighting = false;
		}
		parts[i].material.shininess = shininess;

		//マテリアルデータを更新
		objectMaterialResource_[objectIndex]->Map(0, nullptr, reinterpret_cast<void**>(&objectMaterialData_[objectIndex]));

		*(objectMaterialData_[objectIndex]) = parts[i].material;

		objectMaterialResource_[objectIndex]->Unmap(0, nullptr);

		//RootSignatureを設定。PSOに設定しているけど別途設定が必要
		commandList_->SetGraphicsRootSignature(rootSignature_.Get());
		commandList_->SetPipelineState(trianglePipelineState_.Get());	//PSOを設定

		commandList_->IASetVertexBuffers(0, 1, &parts[i].model.vertexBufferView_);	//VBVを設定
		commandList_->IASetIndexBuffer(&parts[i].model.indexBufferView_);	//IBVを設定
		//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
		commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(parts[i].textureIndex));
		if (reflection != 0 && directionalLight != nullptr) {
			commandList_->SetGraphicsRootConstantBufferView(3, directionalLight->DirectionalLightElementResource()->GetGPUVirtualAddress());	//DirectionalLighting
		}

		if (reflection != 0 && pointLight != nullptr) {
			commandList_->SetGraphicsRootConstantBufferView(5, pointLight->PointLightElementResource()->GetGPUVirtualAddress());	//PointLighting
		}

		if (reflection != 0 && spotLight != nullptr) {
			commandList_->SetGraphicsRootConstantBufferView(6, spotLight->SpotLightElementResource()->GetGPUVirtualAddress());	//SpotLighting
		}

		//カメラのワールド座標をCBufferに送る
		commandList_->SetGraphicsRootConstantBufferView(4, camera->CameraResource()->GetGPUVirtualAddress());

		//マテリアルCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView(0, objectMaterialResource_[objectIndex]->GetGPUVirtualAddress());
		//wvp用のCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView(1, objectWvpResource_[objectIndex]->GetGPUVirtualAddress());
		//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
		commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//描画(DrawCall)
		commandList_->DrawIndexedInstanced(UINT(parts[i].model.vertexIndex_), 1, 0, 0, 0);

		objectIndex++;

	}
}

void GameEngine::DrawInstancingObject_3D_(InstancingObject* objects, Camera* camera, int reflection, DirectionalLight* directionalLight, PointLight* pointLight, SpotLight* spotLight) {
	std::vector<Parts> parts = objects->GetParts();
	std::list<SRT> transforms = objects->GetTransforms();
	
	for (INT i = 0; i < parts.size(); i++) {
		//WVPデータを更新
		instancingObjectResource_[instancingObjectIndex]->Map(0, nullptr, reinterpret_cast<void**>(&instancingObjectData_[instancingObjectIndex]));

		uint32_t numInstance = 0;
		for (std::list<SRT>::iterator objectIterator = transforms.begin();
			objectIterator != transforms.end(); ++objectIterator) {

			if (numInstance >= kMaxNumInstance)break;

			Matrix4x4 worldMatrix = MakeAffineMatrix((*objectIterator).scale, (*objectIterator).rotate, (*objectIterator).translate);
			instancingObjectData_[instancingObjectIndex][numInstance]->World = worldMatrix;
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera->GetViewMatrix(), camera->GetProjectionMatrix()));
			instancingObjectData_[instancingObjectIndex][numInstance]->WVP = worldViewProjectionMatrix;

			++numInstance;
		}

		instancingObjectResource_[instancingObjectIndex]->Unmap(0, nullptr);

		parts[i].material.color = { 1.0f,1.0f,1.0f,1.0f };
		parts[i].material.uvTransform = MakeIdentity4x4();
		parts[i].material.reflection = reflection;
		if (directionalLight != nullptr) {
			parts[i].material.enableDirectionalLighting = true;
		} else {
			parts[i].material.enableDirectionalLighting = false;
		}
		if (pointLight != nullptr) {
			parts[i].material.enablePointLighting = true;
		} else {
			parts[i].material.enablePointLighting = false;
		}
		parts[i].material.shininess = 40;

		//マテリアルデータを更新
		instancingObjectMaterialResource_[instancingObjectIndex]->Map(0, nullptr, reinterpret_cast<void**>(&instancingObjectMaterialData_[instancingObjectIndex]));

		for (uint32_t i = 0; i < numInstance; i++) {
			instancingObjectMaterialData_[instancingObjectIndex][i] = &parts[i].material;
		}

		instancingObjectMaterialResource_[instancingObjectIndex]->Unmap(0, nullptr);

		//RootSignatureを設定。PSOに設定しているけど別途設定が必要
		commandList_->SetGraphicsRootSignature(rootSignature_.Get());
		commandList_->SetPipelineState(trianglePipelineState_.Get());	//PSOを設定

		commandList_->IASetVertexBuffers(0, 1, &parts[i].model.vertexBufferView_);	//VBVを設定
		commandList_->IASetIndexBuffer(&parts[i].model.indexBufferView_);	//IBVを設定
		//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
		commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(parts[i].textureIndex));
		if (reflection != 0 && directionalLight != nullptr) {
			commandList_->SetGraphicsRootConstantBufferView(3, directionalLight->DirectionalLightElementResource()->GetGPUVirtualAddress());	//Lighting
		}

		//カメラのワールド座標をCBufferに送る
		commandList_->SetGraphicsRootConstantBufferView(4, camera->CameraResource()->GetGPUVirtualAddress());

		//マテリアルCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView(0, instancingObjectMaterialResource_[instancingObjectIndex]->GetGPUVirtualAddress());
		//wvp用のCBufferの場所を設定
		//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
		commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//instancing用のData読むためにStructuredBufferのSRVを設定する
		D3D12_GPU_DESCRIPTOR_HANDLE handle = GameEngine::GetInstancingSRV(instancingObjectResource_[instancingObjectIndex], numInstance);
		commandList_->SetGraphicsRootDescriptorTable(1, handle);
		//描画(DrawCall)
		commandList_->DrawIndexedInstanced(UINT(parts[i].model.vertexIndex_), numInstance, 0, 0, 0);

		instancingObjectIndex++;
	}
}



void GameEngine::DrawSprite_2D_(Sprite* sprite) {

	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(kWindowWidth_), float(kWindowHeight_), 0.0f, 100.0f);

	//WVPデータを更新
	spriteWvpResource_[spriteIndex]->Map(0, nullptr, reinterpret_cast<void**>(&spriteWvpData_[spriteIndex]));

	Matrix4x4 worldMatrix = MakeAffineMatrix(sprite->GetTransform().scale, sprite->GetTransform().rotate, sprite->GetTransform().translate);
	spriteWvpData_[spriteIndex]->World = worldMatrix;
	spriteWvpData_[spriteIndex]->WorldInverseTranspose = Inverse(worldMatrix);

	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	spriteWvpData_[spriteIndex]->WVP = worldViewProjectionMatrix;

	spriteWvpResource_[spriteIndex]->Unmap(0, nullptr);

	//マテリアルデータを更新
	spriteMaterialResource_[spriteIndex]->Map(0, nullptr, reinterpret_cast<void**>(&spriteMaterialData_[spriteIndex]));

	spriteMaterialData_[spriteIndex]->color = sprite->GetColor();
	spriteMaterialData_[spriteIndex]->uvTransform = MakeAffineMatrix(sprite->GetUVTransform().scale, sprite->GetUVTransform().rotate, sprite->GetUVTransform().translate);
	spriteMaterialData_[spriteIndex]->reflection = 0;
	spriteMaterialData_[spriteIndex]->enableDirectionalLighting = false;
	spriteMaterialData_[spriteIndex]->enablePointLighting = false;
	spriteMaterialData_[spriteIndex]->shininess = 0.0f;

	spriteMaterialResource_[spriteIndex]->Unmap(0, nullptr);

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->SetPipelineState(spritePipelineState_.Get());	//PSOを設定
	commandList_->IASetVertexBuffers(0, 1, &sprite->GetVertexBufferView());	//VBVを設定
	commandList_->IASetIndexBuffer(&sprite->GetIndexBufferView());	//IBVを設定
	commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(sprite->GetTextureIndex()));
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

void GameEngine::DrawInstancingSprite_2D_(std::vector<Sprite*> sprits) {

	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(kWindowWidth_), float(kWindowHeight_), 0.0f, 100.0f);

	//WVPデータを更新
	instancingObjectResource_[instancingSpriteIndex]->Map(0, nullptr, reinterpret_cast<void**>(&instancingObjectData_[instancingSpriteIndex]));

	uint32_t numInstance = 0;
	for (auto i = 0; i < sprits.size();i++) {

		if (numInstance >= kMaxNumInstance)break;

		Matrix4x4 worldMatrix = MakeAffineMatrix(sprits[i]->GetTransform().translate, sprits[i]->GetTransform().rotate, sprits[i]->GetTransform().translate);
		instancingSpriteData_[instancingSpriteIndex][numInstance]->World = worldMatrix;
		instancingSpriteData_[instancingSpriteIndex][numInstance]->WorldInverseTranspose = Inverse(worldMatrix);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		instancingSpriteData_[instancingSpriteIndex][numInstance]->WVP = worldViewProjectionMatrix;

		instancingSpriteResource_[instancingSpriteIndex]->Unmap(0, nullptr);

		//マテリアルデータを更新
		instancingSpriteMaterialResource_[instancingSpriteIndex]->Map(0, nullptr, reinterpret_cast<void**>(&instancingObjectMaterialData_[instancingSpriteIndex]));

		for (uint32_t j = 0; j < numInstance; j++) {
			instancingSpriteMaterialData_[instancingSpriteIndex][j] = const_cast<Material*>(&sprits[i]->GetMaterial());
		}

		instancingSpriteMaterialResource_[instancingSpriteIndex]->Unmap(0, nullptr);

		++numInstance;
	}

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->SetPipelineState(spritePipelineState_.Get());	//PSOを設定
	commandList_->IASetVertexBuffers(0, 1, &sprits[0]->GetVertexBufferView());	//VBVを設定
	commandList_->IASetIndexBuffer(&sprits[0]->GetIndexBufferView());	//IBVを設定
	commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(sprits[0]->GetTextureIndex()));
	//マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, spriteMaterialResource_[spriteIndex]->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, spriteWvpResource_[spriteIndex]->GetGPUVirtualAddress());
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//instancing用のData読むためにStructuredBufferのSRVを設定する
	D3D12_GPU_DESCRIPTOR_HANDLE handle = GameEngine::GetInstancingSRV(instancingObjectResource_[instancingObjectIndex], numInstance);
	commandList_->SetGraphicsRootDescriptorTable(1, handle);
	//描画(DrawCall)
	commandList_->DrawIndexedInstanced(6, numInstance, 0, 0, 0);

	instancingObjectIndex++;
}