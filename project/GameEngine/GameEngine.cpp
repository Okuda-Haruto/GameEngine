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
#include "Animation/Animation.h"

int32_t GameEngine::kWindowWidth_;
int32_t GameEngine::kWindowHeight_;
unique_ptr<GameEngine> GameEngine::instance;

GameEngine* GameEngine::GetInstance() {
	if (!instance) {
		instance = make_unique<GameEngine>();
	}
	return instance.get();
}

void GameEngine::Finalize_() {
	if (masterVoice_) {
		masterVoice_->DestroyVoice();
		masterVoice_ = nullptr;
	}
	xAudio2_.Reset();

	object3DPipelineState_.Reset();
	object2DPipelineState_.Reset();
	noDepthObjectPipelineState_.Reset();
	instancingObjectPipelineState_.Reset();
	particlePipelineState_.Reset();
	spritePipelineState_.Reset();
	linePipelineState_.Reset();
	noDepthLinePipelineState_.Reset();

	AudioManager::GetInstance()->Finalize();
	TextureManager::GetInstance()->Finalize();
	ModelManager::GetInstance()->Finalize();
	SpriteManager::GetInstance()->Finalize();
	PrimitiveManager::GetInstance()->Finalize();
	Object::FinalizeDefaultCamera();
	ParticleManager::GetInstance()->Finalize();

	imguiManager_.reset();
	srvManager_.reset();
	dxCommon_.reset();
	winApp_.reset();

	textureData_.clear();
}

void GameEngine::Finalize() {
	if (instance) {
		instance->Finalize_(); // リソース解放（this はまだ有効）
		instance.reset();      // この呼び出しでオブジェクトを削除する（呼出し元スタック上なので安全）
	}
}

void GameEngine::Initialize_(const wchar_t* WindowName, int32_t kWindowWidth, int32_t kWindowHeight) {

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

	winApp_ = make_unique<WindowsAPI>();
	winApp_->Initialize(WindowName, kWindowWidth, kWindowHeight);

	dxCommon_ = make_unique<DirectXCommon>();
	dxCommon_->Initialize(winApp_.get());

	srvManager_ = make_unique<SRVManager>();
	srvManager_->Initialize(dxCommon_.get());

	imguiManager_ = make_unique<ImGuiManager>();
	imguiManager_->Initialize(dxCommon_.get(), winApp_.get(), srvManager_.get());

	dxCommon_->DepthBufferInitialize(srvManager_.get());
	
	TextureManager::GetInstance()->Initialize(dxCommon_.get(), srvManager_.get());
	ModelManager::GetInstance()->Initialize(dxCommon_.get());
	SpriteManager::GetInstance()->Initialize(dxCommon_.get());
	PrimitiveManager::GetInstance()->Initialize(dxCommon_.get(), srvManager_.get());

	//カメラ初期値
	std::shared_ptr<Camera> DefaultCamera = std::make_shared<Camera>();
	DefaultCamera->Initialize(dxCommon_.get());
	Object::SetDefaultCamera(DefaultCamera);
	ParticleManager::GetInstance()->Initialize(dxCommon_.get(), srvManager_.get());

	device_ = dxCommon_->GetDevice();
	commandList_ = dxCommon_->GetCommandList();


	//RootSignature作成
	objectRootSignature_ = dxCommon_->ObjectRootSignatureInitialvalue();
	spriteRootSignature_ = dxCommon_->SpriteRootSignatureInitialvalue();
	instancingObjectRootSignature_ = dxCommon_->InstancingObjectRootSignatureInitialvalue();
	particleRootSignature_ = dxCommon_->ParticleRootSignatureInitialvalue();

	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> Object3DVertexShaderBlob = dxCommon_->CompileShader(L"./resources/Shader/Object3D.VS.hlsl", L"vs_6_0");
	assert(Object3DVertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> Object3DPixelShaderBlob = dxCommon_->CompileShader(L"./resources/Shader/Object3D.PS.hlsl", L"ps_6_0");
	assert(Object3DPixelShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> Object2DVertexShaderBlob = dxCommon_->CompileShader(L"./resources/Shader/Object2D.VS.hlsl", L"vs_6_0");
	assert(Object2DVertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> Object2DPixelShaderBlob = dxCommon_->CompileShader(L"./resources/Shader/Object2D.PS.hlsl", L"ps_6_0");
	assert(Object2DPixelShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> instancingObjectVertexShaderBlob = dxCommon_->CompileShader(L"./resources/Shader/InstanceObject3D.VS.hlsl", L"vs_6_0");
	assert(instancingObjectVertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> instancingObjectPixelShaderBlob = dxCommon_->CompileShader(L"./resources/Shader/InstanceObject3D.PS.hlsl", L"ps_6_0");
	assert(instancingObjectPixelShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> instancingLinePixelShaderBlob = dxCommon_->CompileShader(L"./resources/Shader/InstanceLine.PS.hlsl", L"ps_6_0");
	assert(instancingLinePixelShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> Sprite2DVertexShaderBlob = dxCommon_->CompileShader(L"./resources/Shader/Sprite2D.VS.hlsl", L"vs_6_0");
	assert(Sprite2DVertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> Sprite2DPixelShaderBlob = dxCommon_->CompileShader(L"./resources/Shader/Sprite2D.PS.hlsl", L"ps_6_0");
	assert(Sprite2DPixelShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> particleVSBlob = dxCommon_->CompileShader(L"./resources/Shader/Particle.VS.hlsl", L"vs_6_0");
	assert(particleVSBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> particlePSBlob = dxCommon_->CompileShader(L"./resources/Shader/Particle.PS.hlsl", L"ps_6_0");
	assert(particlePSBlob != nullptr);

	//PSOを生成
	object3DPipelineState_ = TrianglePipelineStateInitialvalue(device_, objectRootSignature_, Object3DVertexShaderBlob.Get(), Object3DPixelShaderBlob.Get());
	object2DPipelineState_ = TrianglePipelineStateInitialvalue(device_, objectRootSignature_, Object2DVertexShaderBlob.Get(), Object2DPixelShaderBlob.Get());
	noDepthObjectPipelineState_ = NoDepthTrianglePipelineStateInitialvalue(device_, objectRootSignature_, Object3DVertexShaderBlob.Get(), Object3DPixelShaderBlob.Get());
	instancingObjectPipelineState_ = InstancingTrianglePipelineStateInitialvalue(device_, instancingObjectRootSignature_, instancingObjectVertexShaderBlob.Get(), instancingObjectPixelShaderBlob.Get());
	particlePipelineState_ = ParticlePipelineStateInitialvalue(device_, particleRootSignature_, particleVSBlob.Get(), particlePSBlob.Get());
	particleAddBrendPipelineState_ = AddBlendParticlePipelineStateInitialvalue(device_, particleRootSignature_, particleVSBlob.Get(), particlePSBlob.Get());
	spritePipelineState_ = SpritePipelineStateInitialvalue(device_, spriteRootSignature_, Sprite2DVertexShaderBlob.Get(), Sprite2DPixelShaderBlob.Get());
	linePipelineState_ = LinePipelineStateInitialvalue(device_, instancingObjectRootSignature_, particleVSBlob.Get(), instancingLinePixelShaderBlob.Get());
	noDepthLinePipelineState_ = NoDepthLinePipelineStateInitialvalue(device_, instancingObjectRootSignature_, particleVSBlob.Get(), instancingLinePixelShaderBlob.Get());
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

	//初期化
	for (int i = 0; i < kMaxIndex; i++) {
		objectMaterialResource_[i] = dxCommon_->CreateBufferResources(sizeof(Material));
		objectWvpResource_[i] = dxCommon_->CreateBufferResources(sizeof(TransformationMatrix));
		objectBoneResource_[i] = dxCommon_->CreateBufferResources(sizeof(BoneMatrix));
		spriteMaterialResource_[i] = dxCommon_->CreateBufferResources(sizeof(Material));
		spriteWvpResource_[i] = dxCommon_->CreateBufferResources(sizeof(TransformationMatrix));
	}

	for (int i = 0; i < kMaxInstanceIndex; i++) {
		instancingObjectMaterialResource_[i] = dxCommon_->CreateBufferResources(sizeof(Material));
		instancingObjectResource_[i] = dxCommon_->CreateBufferResources(sizeof(InstancingTransformationMatrix) * kMaxNumInstance);
		instancingObjectBoneResource_[i] = dxCommon_->CreateBufferResources(sizeof(BoneMatrix));
		uint32_t index = srvManager_->Allocate();
		//SRVを作成
		srvManager_->CreateSRVforStructuredBuffer(index, instancingObjectResource_[i].Get(), kMaxNumInstance, sizeof(InstancingTransformationMatrix));
		if (i == 0)startInstancingObjectIndex_ = index;
	}
	for (int i = 0; i < kMaxInstanceIndex; i++) {
		particleMaterialResource_[i] = dxCommon_->CreateBufferResources(sizeof(Material));
		particleResource_[i] = dxCommon_->CreateBufferResources(sizeof(InstancingTransformationMatrix) * kMaxNumInstance);
	}
	for (int i = 0; i < kMaxInstanceIndex; i++) {
		instancingSpriteMaterialResource_[i] = dxCommon_->CreateBufferResources(sizeof(Material));
		instancingSpriteResource_[i] = dxCommon_->CreateBufferResources(sizeof(InstancingTransformationMatrix) * kMaxNumInstance);
		uint32_t index = srvManager_->Allocate();
		//SRVを作成
		srvManager_->CreateSRVforStructuredBuffer(index, instancingSpriteResource_[i].Get(), kMaxNumInstance, sizeof(InstancingTransformationMatrix));
		if (i == 0)startInstancingSpriteIndex_ = index;
	}

	for (int i = 0; i < PrimitiveManager::SHAPE_count; i++) {
		primitiveMaterialResource_[i] = dxCommon_->CreateBufferResources(sizeof(Material));
		primitiveResource_[i] = dxCommon_->CreateBufferResources(sizeof(InstancingTransformationMatrix) * PrimitiveManager::kMaxNumPrimitive);
	}

	fogResource_ = dxCommon_->CreateBufferResources(sizeof(Fog));
}


float GameEngine::randomFloat_(float minFloat, float maxFloat) {
	assert(minFloat <= maxFloat);
	std::uniform_real_distribution<float> distribution(minFloat, maxFloat);
	return distribution(randomEngine_);
}
int32_t GameEngine::randomInt_(int32_t minInt, int32_t maxInt) {
	assert(minInt <= maxInt);
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
	objectIndex_ = 0;
	instancingObjectIndex_ = 0;
	particleIndex_ = 0;
	spriteIndex_ = 0;
	instancingSpriteIndex_ = 0;

	imguiManager_->End();

	srvManager_->PreDraw();

}

void GameEngine::PostDraw_() {

	PrimitiveManager::GetInstance()->Draw();

	imguiManager_->Draw();

	dxCommon_->PostDraw();
}

void GameEngine::DrawObject_3D_(Object* object, shared_ptr<DirectionalLight> directionalLight, shared_ptr<PointLight> pointLight, shared_ptr<SpotLight> spotLight, UINT animationIndex, float time) {

	//上限に達していたら描画しない
	if (objectIndex_ >= kMaxIndex)return;

	std::vector<Parts> parts = object->GetParts();
	std::vector<Offset> offsets = object->GetOffsets();

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(objectRootSignature_.Get());
	commandList_->SetPipelineState(object3DPipelineState_.Get());	//PSOを設定

	commandList_->IASetVertexBuffers(0, 1, &object->GetVBV());	//VBVを設定
	commandList_->IASetIndexBuffer(&object->GetIBV());	//IBVを設定

	//カメラのワールド座標をCBufferに送る
	commandList_->SetGraphicsRootConstantBufferView(4, object->GetCamera()->CameraResource()->GetGPUVirtualAddress());

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//オブジェクトのワールド座標
	Matrix4x4 worldMatrix = MakeAffineMatrix(object->GetTransform().scale, object->GetTransform().rotate, object->GetTransform().translate);

	//変更が必要な部分だけ変える
	for (int i = 0; i < parts.size();i++) {

		//WVPデータを更新
		objectWvpResource_[objectIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&objectWvpData_[objectIndex_]));

		Matrix4x4 partsMatrix = MakeAffineMatrix(parts[i].transform->scale, parts[i].transform->rotate, parts[i].transform->translate);
		if (parts[i].parent) {
			//親を持つPartsのローカル座標
			Matrix4x4 parentMatrix = MakeAffineMatrix(parts[i].parent->scale, parts[i].parent->rotate, parts[i].parent->translate);
			partsMatrix = partsMatrix * parentMatrix;
		} else {
			//ワールド座標を親に持つPartsのローカル座標
			partsMatrix = partsMatrix * worldMatrix;
		}

		objectWvpData_[objectIndex_]->World = partsMatrix;
		objectWvpData_[objectIndex_]->WorldInverseTranspose = Transpose(Inverse(partsMatrix));
		Matrix4x4 worldViewProjectionMatrix = partsMatrix * object->GetCamera()->GetViewMatrix() * object->GetCamera()->GetProjectionMatrix();
		objectWvpData_[objectIndex_]->WVP = worldViewProjectionMatrix;

		objectWvpResource_[objectIndex_]->Unmap(0, nullptr);

		//ボーンデータ
		objectBoneResource_[objectIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&objectBoneData_[objectIndex_]));

		std::vector<Bone> bones = object->GetBones();
		for (int i = 0; i < bones.size(); i++) {
			if (i > 128)break;
			objectBoneData_[objectIndex_]->matrix[i] = bones[i].finalMatrix;
		}

		objectBoneResource_[objectIndex_]->Unmap(0,nullptr);

		parts[i].material->uvTransform = MakeAffineMatrix(parts[i].UVtransform.scale, parts[i].UVtransform.rotate, parts[i].UVtransform.translate);
		parts[i].material->enableDirectionalLighting = directionalLight != nullptr;
		parts[i].material->enablePointLighting = pointLight != nullptr;
		parts[i].material->enableSpotLighting = spotLight != nullptr;

		//マテリアルデータを更新
		objectMaterialResource_[objectIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&objectMaterialData_[objectIndex_]));

		*objectMaterialData_[objectIndex_] = *parts[i].material;

		objectMaterialResource_[objectIndex_]->Unmap(0, nullptr);

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
		commandList_->SetGraphicsRootConstantBufferView(0, objectMaterialResource_[objectIndex_]->GetGPUVirtualAddress());
		//wvp用のCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView(1, objectWvpResource_[objectIndex_]->GetGPUVirtualAddress());
		//ボーンCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView(7, objectBoneResource_[objectIndex_]->GetGPUVirtualAddress());

		//描画(DrawCall)
		commandList_->DrawIndexedInstanced(offsets[i].indexCount, 1, 0, offsets[i].vertexStart, 0);

		objectIndex_++;
	}
}

void GameEngine::DrawParts_3D_(Object* object, uint32_t partsIndex, shared_ptr<DirectionalLight> directionalLight, shared_ptr<PointLight> pointLight, shared_ptr<SpotLight> spotLight) {
	//上限に達していたら描画しない
	if (objectIndex_ >= kMaxIndex)return;

	std::vector<Parts> parts = object->GetParts();
	std::vector<Offset> offsets = object->GetOffsets();

	//パーツのサイズを超えた位置を指定したらエラーを出す
	assert(partsIndex < parts.size());

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(objectRootSignature_.Get());
	commandList_->SetPipelineState(object3DPipelineState_.Get());	//PSOを設定

	commandList_->IASetVertexBuffers(0, 1, &object->GetVBV());	//VBVを設定
	commandList_->IASetIndexBuffer(&object->GetIBV());	//IBVを設定

	//カメラのワールド座標をCBufferに送る
	commandList_->SetGraphicsRootConstantBufferView(4, object->GetCamera()->CameraResource()->GetGPUVirtualAddress());

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//オブジェクトのワールド座標
	Matrix4x4 worldMatrix = MakeAffineMatrix(object->GetTransform().scale, object->GetTransform().rotate, object->GetTransform().translate);

	//WVPデータを更新
	objectWvpResource_[objectIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&objectWvpData_[objectIndex_]));

	Matrix4x4 partsMatrix = MakeAffineMatrix(parts[partsIndex].transform->scale, parts[partsIndex].transform->rotate, parts[partsIndex].transform->translate);
	if (parts[partsIndex].parent) {
		//親を持つPartsのローカル座標
		Matrix4x4 parentMatrix = MakeAffineMatrix(parts[partsIndex].parent->scale, parts[partsIndex].parent->rotate, parts[partsIndex].parent->translate);
		partsMatrix = partsMatrix * parentMatrix;
	} else {
		//ワールド座標を親に持つPartsのローカル座標
		partsMatrix = partsMatrix * worldMatrix;
	}

	objectWvpData_[objectIndex_]->World = partsMatrix;
	objectWvpData_[objectIndex_]->WorldInverseTranspose = Transpose(Inverse(partsMatrix));
	Matrix4x4 worldViewProjectionMatrix = partsMatrix * object->GetCamera()->GetViewMatrix() * object->GetCamera()->GetProjectionMatrix();
	objectWvpData_[objectIndex_]->WVP = worldViewProjectionMatrix;

	objectWvpResource_[objectIndex_]->Unmap(0, nullptr);

	//ボーンデータ
	objectBoneResource_[objectIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&objectBoneData_[objectIndex_]));

	std::vector<Bone> bones = object->GetBones();
	for (int i = 0; i < bones.size(); i++) {
		if (i > 128)break;
		objectBoneData_[objectIndex_]->matrix[i] = bones[i].finalMatrix;
	}

	objectBoneResource_[objectIndex_]->Unmap(0, nullptr);

	parts[partsIndex].material->uvTransform = MakeAffineMatrix(parts[partsIndex].UVtransform.scale, parts[partsIndex].UVtransform.rotate, parts[partsIndex].UVtransform.translate);
	parts[partsIndex].material->enableDirectionalLighting = directionalLight != nullptr;
	parts[partsIndex].material->enablePointLighting = pointLight != nullptr;
	parts[partsIndex].material->enableSpotLighting = spotLight != nullptr;

	//マテリアルデータを更新
	objectMaterialResource_[objectIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&objectMaterialData_[objectIndex_]));

	*objectMaterialData_[objectIndex_] = *parts[partsIndex].material;

	objectMaterialResource_[objectIndex_]->Unmap(0, nullptr);

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	commandList_->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(parts[partsIndex].textureIndex));

	//ライティングが必要な場合CBufferに送る
	if (parts[partsIndex].material->reflection != 0 && directionalLight != nullptr) {
		commandList_->SetGraphicsRootConstantBufferView(3, directionalLight->DirectionalLightElementResource()->GetGPUVirtualAddress());	//DirectionalLighting
	}
	if (parts[partsIndex].material->reflection != 0 && pointLight != nullptr) {
		commandList_->SetGraphicsRootConstantBufferView(5, pointLight->PointLightElementResource()->GetGPUVirtualAddress());	//PointLighting
	}
	if (parts[partsIndex].material->reflection != 0 && spotLight != nullptr) {
		commandList_->SetGraphicsRootConstantBufferView(6, spotLight->SpotLightElementResource()->GetGPUVirtualAddress());	//SpotLighting
	}

	//マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, objectMaterialResource_[objectIndex_]->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, objectWvpResource_[objectIndex_]->GetGPUVirtualAddress());
	//ボーンCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(7, objectBoneResource_[objectIndex_]->GetGPUVirtualAddress());

	//描画(DrawCall)
	commandList_->DrawIndexedInstanced(offsets[partsIndex].indexCount, 1, 0, offsets[partsIndex].vertexStart, 0);

	objectIndex_++;
}

void GameEngine::DrawObject_2D_(Object* object, shared_ptr<DirectionalLight> directionalLight) {

	//上限に達していたら描画しない
	if (objectIndex_ >= kMaxIndex)return;

	std::vector<Parts> parts = object->GetParts();
	std::vector<Offset> offsets = object->GetOffsets();

	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth_) / float(kWindowHeight_), 0.01f, 1.0f);

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(objectRootSignature_.Get());
	commandList_->SetPipelineState(object2DPipelineState_.Get());	//PSOを設定

	commandList_->IASetVertexBuffers(0, 1, &object->GetVBV());	//VBVを設定
	commandList_->IASetIndexBuffer(&object->GetIBV());	//IBVを設定

	//カメラのワールド座標をCBufferに送る
	commandList_->SetGraphicsRootConstantBufferView(4, object->GetCamera()->CameraResource()->GetGPUVirtualAddress());

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//オブジェクトのワールド座標
	Matrix4x4 worldMatrix = MakeAffineMatrix(object->GetTransform().scale, object->GetTransform().rotate, object->GetTransform().translate);

	//変更が必要な部分だけ変える
	for (int i = 0; i < parts.size(); i++) {

		//WVPデータを更新
		objectWvpResource_[objectIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&objectWvpData_[objectIndex_]));

		Matrix4x4 partsMatrix = MakeAffineMatrix(parts[i].transform->scale, parts[i].transform->rotate, parts[i].transform->translate);
		if (parts[i].parent) {
			//親を持つPartsのローカル座標
			Matrix4x4 parentMatrix = MakeAffineMatrix(parts[i].parent->scale, parts[i].parent->rotate, parts[i].parent->translate);
			partsMatrix = partsMatrix * parentMatrix;
		} else {
			//ワールド座標を親に持つPartsのローカル座標
			partsMatrix = partsMatrix * worldMatrix;
		}

		objectWvpData_[objectIndex_]->World = partsMatrix;
		objectWvpData_[objectIndex_]->WorldInverseTranspose = Transpose(Inverse(partsMatrix));
		Matrix4x4 worldViewProjectionMatrix = partsMatrix * viewMatrix * projectionMatrix;
		objectWvpData_[objectIndex_]->WVP = worldViewProjectionMatrix;

		objectWvpResource_[objectIndex_]->Unmap(0, nullptr);

		//ボーンデータ
		objectBoneResource_[objectIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&objectBoneData_[objectIndex_]));

		std::vector<Bone> bones = object->GetBones();
		for (int i = 0; i < bones.size(); i++) {
			if (i > 128)break;
			objectBoneData_[objectIndex_]->matrix[i] = bones[i].finalMatrix;
		}

		objectBoneResource_[objectIndex_]->Unmap(0, nullptr);

		parts[i].material->uvTransform = MakeAffineMatrix(parts[i].UVtransform.scale, parts[i].UVtransform.rotate, parts[i].UVtransform.translate);
		parts[i].material->enableDirectionalLighting = directionalLight != nullptr;
		parts[i].material->enablePointLighting = false;
		parts[i].material->enableSpotLighting = false;

		//マテリアルデータを更新
		objectMaterialResource_[objectIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&objectMaterialData_[objectIndex_]));

		*objectMaterialData_[objectIndex_] = *parts[i].material;

		objectMaterialResource_[objectIndex_]->Unmap(0, nullptr);

		//ライティングが必要な場合CBufferに送る
		if (parts[i].material->reflection != 0 && directionalLight != nullptr) {
			commandList_->SetGraphicsRootConstantBufferView(3, directionalLight->DirectionalLightElementResource()->GetGPUVirtualAddress());	//DirectionalLighting
		}

		//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
		commandList_->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(parts[i].textureIndex));

		//マテリアルCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView(0, objectMaterialResource_[objectIndex_]->GetGPUVirtualAddress());
		//wvp用のCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView(1, objectWvpResource_[objectIndex_]->GetGPUVirtualAddress());
		//ボーンCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView(7, objectBoneResource_[objectIndex_]->GetGPUVirtualAddress());

		//描画(DrawCall)
		commandList_->DrawIndexedInstanced(offsets[i].indexCount, 1, 0, offsets[i].vertexStart, 0);

		objectIndex_++;
	}
}

void GameEngine::DrawParts_2D_(Object* object, uint32_t partsIndex, shared_ptr<DirectionalLight> directionalLight) {

	//上限に達していたら描画しない
	if (objectIndex_ >= kMaxIndex)return;

	std::vector<Parts> parts = object->GetParts();
	std::vector<Offset> offsets = object->GetOffsets();

	//パーツのサイズを超えた位置を指定したらエラーを出す
	assert(partsIndex < parts.size());

	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth_) / float(kWindowHeight_), 0.01f, 1.0f);

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(objectRootSignature_.Get());
	commandList_->SetPipelineState(object2DPipelineState_.Get());	//PSOを設定

	commandList_->IASetVertexBuffers(0, 1, &object->GetVBV());	//VBVを設定
	commandList_->IASetIndexBuffer(&object->GetIBV());	//IBVを設定

	//カメラのワールド座標をCBufferに送る
	commandList_->SetGraphicsRootConstantBufferView(4, object->GetCamera()->CameraResource()->GetGPUVirtualAddress());

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//オブジェクトのワールド座標
	Matrix4x4 worldMatrix = MakeAffineMatrix(object->GetTransform().scale, object->GetTransform().rotate, object->GetTransform().translate);

	//WVPデータを更新
	objectWvpResource_[objectIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&objectWvpData_[objectIndex_]));

	Matrix4x4 partsMatrix = MakeAffineMatrix(parts[partsIndex].transform->scale, parts[partsIndex].transform->rotate, parts[partsIndex].transform->translate);
	if (parts[partsIndex].parent) {
		//親を持つPartsのローカル座標
		Matrix4x4 parentMatrix = MakeAffineMatrix(parts[partsIndex].parent->scale, parts[partsIndex].parent->rotate, parts[partsIndex].parent->translate);
		partsMatrix = partsMatrix * parentMatrix;
	} else {
		//ワールド座標を親に持つPartsのローカル座標
		partsMatrix = partsMatrix * worldMatrix;
	}

	objectWvpData_[objectIndex_]->World = partsMatrix;
	objectWvpData_[objectIndex_]->WorldInverseTranspose = Transpose(Inverse(partsMatrix));
	Matrix4x4 worldViewProjectionMatrix = partsMatrix * viewMatrix * projectionMatrix;
	objectWvpData_[objectIndex_]->WVP = worldViewProjectionMatrix;

	objectWvpResource_[objectIndex_]->Unmap(0, nullptr);

	//ボーンデータ
	objectBoneResource_[objectIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&objectBoneData_[objectIndex_]));

	std::vector<Bone> bones = object->GetBones();
	for (int i = 0; i < bones.size(); i++) {
		if (i > 128)break;
		objectBoneData_[objectIndex_]->matrix[i] = bones[i].finalMatrix;
	}

	objectBoneResource_[objectIndex_]->Unmap(0, nullptr);

	parts[partsIndex].material->uvTransform = MakeAffineMatrix(parts[partsIndex].UVtransform.scale, parts[partsIndex].UVtransform.rotate, parts[partsIndex].UVtransform.translate);
	parts[partsIndex].material->enableDirectionalLighting = directionalLight != nullptr;
	parts[partsIndex].material->enablePointLighting = false;
	parts[partsIndex].material->enableSpotLighting = false;

	//マテリアルデータを更新
	objectMaterialResource_[objectIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&objectMaterialData_[objectIndex_]));

	*objectMaterialData_[objectIndex_] = *parts[partsIndex].material;

	objectMaterialResource_[objectIndex_]->Unmap(0, nullptr);

	//ライティングが必要な場合CBufferに送る
	if (parts[partsIndex].material->reflection != 0 && directionalLight != nullptr) {
		commandList_->SetGraphicsRootConstantBufferView(3, directionalLight->DirectionalLightElementResource()->GetGPUVirtualAddress());	//DirectionalLighting
	}

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	commandList_->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(parts[partsIndex].textureIndex));

	//マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, objectMaterialResource_[objectIndex_]->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, objectWvpResource_[objectIndex_]->GetGPUVirtualAddress());
	//ボーンCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(7, objectBoneResource_[objectIndex_]->GetGPUVirtualAddress());

	//描画(DrawCall)
	commandList_->DrawIndexedInstanced(offsets[partsIndex].indexCount, 1, 0, offsets[partsIndex].vertexStart, 0);

	objectIndex_++;
}

void GameEngine::DrawInstancingObject_3D_(std::list<Object*> objects, shared_ptr<DirectionalLight> directionalLight, shared_ptr<PointLight> pointLight, shared_ptr<SpotLight> spotLight) {
	
	//上限に達していたら描画しない
	if (instancingObjectIndex_ > kMaxInstanceIndex)return;

	std::list<Object*>::iterator objectIterator = objects.begin();
	Camera* camera = (*objectIterator)->GetCamera().get();

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(instancingObjectRootSignature_.Get());
	commandList_->SetPipelineState(instancingObjectPipelineState_.Get());	//PSOを設定

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
		Matrix4x4 worldMatrix = MakeAffineMatrix((*objectIterator)->GetTransform().scale, (*objectIterator)->GetTransform().rotate, (*objectIterator)->GetTransform().translate);

		worldMatries.push_back(worldMatrix);

		++numInstance;
		++objectIterator;
	}

	//インスタシング描画とボーンアニメーションの両立は構造体の大きさ故に難しい
	instancingObjectBoneResource_[instancingObjectIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&instancingObjectBoneData_));
	std::vector<Bone> bones = (*objectIterator)->GetBones();
	for (int i = 0; i < bones.size(); i++) {
		instancingObjectBoneData_[instancingObjectIndex_]->matrix[i] = bones[i].finalMatrix;
	}
	instancingObjectBoneResource_[instancingObjectIndex_]->Unmap(0, nullptr);
	commandList_->SetGraphicsRootConstantBufferView(7, instancingObjectBoneResource_[instancingObjectIndex_]->GetGPUVirtualAddress());
	
	//パーツごとにインスタシング描画
	for (uint32_t i = 0; i < numParts; i++) {

		//WVPデータを更新
		InstancingTransformationMatrix* mappedBase = nullptr;
		instancingObjectResource_[instancingObjectIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&mappedBase));
		// mappedBase が nullptr でないかチェック
		if (mappedBase == nullptr) {
			assert(0);
		}
		// 各要素ポインタを mappedBase に初期化
		for (uint32_t j = 0; j < kMaxNumInstance; ++j) {
			instancingObjectData_[instancingObjectIndex_][j] = mappedBase + j;
		}

		numInstance = 0;
		for (objectIterator = objects.begin();
			objectIterator != objects.end(); ++objectIterator) {

			if (numInstance >= kMaxNumInstance)break;

			//ワールド座標を親に持つPartsのローカル座標
			Matrix4x4 partsMatrix = MakeAffineMatrix(parts[numInstance][i].transform->scale, parts[numInstance][i].transform->rotate, parts[numInstance][i].transform->translate);

			if (parts[numInstance][i].parent) {
				//親を持つPartsのローカル座標
				Matrix4x4 parentMatrix = MakeAffineMatrix(parts[numInstance][i].parent->scale, parts[numInstance][i].parent->rotate, parts[numInstance][i].parent->translate);
				partsMatrix = partsMatrix * parentMatrix;
			} else {
				//ワールド座標を親に持つPartsのローカル座標
				partsMatrix = partsMatrix * worldMatries[numInstance];
			}

			instancingObjectData_[instancingObjectIndex_][numInstance]->World = partsMatrix;
			instancingObjectData_[instancingObjectIndex_][numInstance]->WorldInverseTranspose = Transpose(Inverse(partsMatrix));
			Matrix4x4 worldViewProjectionMatrix = partsMatrix * camera->GetViewMatrix() * camera->GetProjectionMatrix();
			instancingObjectData_[instancingObjectIndex_][numInstance]->WVP = worldViewProjectionMatrix;
			instancingObjectData_[instancingObjectIndex_][numInstance]->color = parts[numInstance][i].material->color;

			++numInstance;
		}

		instancingObjectResource_[instancingObjectIndex_]->Unmap(0, nullptr);

		//インスタシング描画の都合上マテリアルは先頭のもの全てに適応
		parts[0][i].material->uvTransform = MakeAffineMatrix(parts[0][i].UVtransform.scale, parts[0][i].UVtransform.rotate, parts[0][i].UVtransform.translate);
		parts[0][i].material->enableDirectionalLighting = directionalLight != nullptr;
		parts[0][i].material->enablePointLighting = pointLight != nullptr;
		parts[0][i].material->enableSpotLighting = spotLight != nullptr;
		parts[0][i].material->color = Vector4{ 1.0f,1.0f,1.0f,1.0f };

		//マテリアルデータを更新
		objectMaterialResource_[instancingObjectIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&objectMaterialData_[instancingObjectIndex_]));

		*objectMaterialData_[instancingObjectIndex_] = *parts[0][i].material;

		objectMaterialResource_[instancingObjectIndex_]->Unmap(0, nullptr);

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

		commandList_->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(startInstancingObjectIndex_ + instancingObjectIndex_));

		//マテリアルCBufferの場所を設定
		commandList_->SetGraphicsRootConstantBufferView(0, objectMaterialResource_[instancingObjectIndex_]->GetGPUVirtualAddress());
		
		//描画(DrawCall)
		commandList_->DrawIndexedInstanced(offsets[i].indexCount, numInstance, 0, offsets[i].vertexStart, 0);

		instancingObjectIndex_++;
	}
}

void GameEngine::DrawParticle_(ParticleGroup particleGroup) {

	//上限に達していたら描画しない
	if (particleIndex_ > kMaxInstanceIndex)return;

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(particleRootSignature_.Get());
	commandList_->SetPipelineState(particlePipelineState_.Get());	//PSOを設定

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList_->IASetVertexBuffers(0, 1, &ParticleManager::GetInstance()->GetVertexBufferView());	//VBVを設定
	commandList_->IASetIndexBuffer(&ParticleManager::GetInstance()->GetIndexBufferView());	//IBVを設定

	Camera* camera = Object::GetDefaultCamera().get();

	//WVPデータを更新
	InstancingTransformationMatrix* mappedBase = nullptr;
	particleGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedBase));
	// mappedBase が nullptr でないかチェック
	if (mappedBase == nullptr) {
		assert(0);
	}
	// 各要素ポインタを mappedBase に初期化
	for (uint32_t j = 0; j < kMaxNumInstance; ++j) {
		particleData_[particleIndex_][j] = mappedBase + j;
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

		particleData_[particleIndex_][numInstance]->World = worldMatrix;
		particleData_[particleIndex_][numInstance]->WorldInverseTranspose = Transpose(Inverse(worldMatrix));
		Matrix4x4 worldViewProjectionMatrix = worldMatrix * camera->GetViewMatrix() * camera->GetProjectionMatrix();
		particleData_[particleIndex_][numInstance]->WVP = worldViewProjectionMatrix;
		particleData_[particleIndex_][numInstance]->color = (*particleIterator).color;

		++numInstance;
		++particleIterator;
	}

	particleGroup.instancingResource->Unmap(0, nullptr);

	//マテリアルデータを更新
	particleMaterialResource_[particleIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&particleMaterialData_[particleIndex_]));

	particleMaterialData_[particleIndex_]->uvTransform = MakeIdentity4x4();
	particleMaterialData_[particleIndex_]->enableDirectionalLighting = false;
	particleMaterialData_[particleIndex_]->enablePointLighting = false;
	particleMaterialData_[particleIndex_]->enableSpotLighting = false;
	particleMaterialData_[particleIndex_]->reflection = 0;
	particleMaterialData_[particleIndex_]->shininess = 0;
	particleMaterialData_[particleIndex_]->color = {1.0f,1.0f,1.0f,1.0f};

	particleMaterialResource_[particleIndex_]->Unmap(0, nullptr);

	fogResource_->Map(0, nullptr, reinterpret_cast<void**>(&fogData_));

	fogData_->windowSize = { float(winApp_->kClientWidth_),float(winApp_->kClientHeight_) };

	fogResource_->Unmap(0, nullptr);

	//マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, particleMaterialResource_[particleIndex_]->GetGPUVirtualAddress());

	commandList_->SetGraphicsRootConstantBufferView(3, fogResource_->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	commandList_->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(particleGroup.textureIndex));

	commandList_->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(particleGroup.instancingIndex));

	dxCommon_->SetDepthTexture(4);

	//描画(DrawCall)
	commandList_->DrawIndexedInstanced(6, numInstance, 0, 0, 0);

	particleIndex_++;
}

void GameEngine::DrawSprite_2D_(Sprite* sprite) {

	//上限に達していたら描画しない
	if (spriteIndex_ > kMaxIndex)return;

	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(kWindowWidth_), float(kWindowHeight_), 0.0f, 100.0f);

	//WVPデータを更新
	spriteWvpResource_[spriteIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&spriteWvpData_[spriteIndex_]));

	Matrix4x4 worldMatrix = MakeAffineMatrix(sprite->GetTransform().scale, sprite->GetTransform().rotate, sprite->GetTransform().translate);
	spriteWvpData_[spriteIndex_]->World = worldMatrix;
	spriteWvpData_[spriteIndex_]->WorldInverseTranspose = Transpose(Inverse(worldMatrix));

	Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
	spriteWvpData_[spriteIndex_]->WVP = worldViewProjectionMatrix;

	spriteWvpResource_[spriteIndex_]->Unmap(0, nullptr);

	//マテリアルデータを更新
	spriteMaterialResource_[spriteIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&spriteMaterialData_[spriteIndex_]));

	spriteMaterialData_[spriteIndex_]->color = sprite->GetColor();
	spriteMaterialData_[spriteIndex_]->uvTransform = MakeAffineMatrix(sprite->GetUVTransform().scale, sprite->GetUVTransform().rotate, sprite->GetUVTransform().translate);
	spriteMaterialData_[spriteIndex_]->reflection = 0;
	spriteMaterialData_[spriteIndex_]->enableDirectionalLighting = false;
	spriteMaterialData_[spriteIndex_]->enablePointLighting = false;
	spriteMaterialData_[spriteIndex_]->enableSpotLighting = false;
	spriteMaterialData_[spriteIndex_]->shininess = 0.0f;

	spriteMaterialResource_[spriteIndex_]->Unmap(0, nullptr);

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(spriteRootSignature_.Get());
	commandList_->SetPipelineState(spritePipelineState_.Get());	//PSOを設定
	commandList_->IASetVertexBuffers(0, 1, &sprite->GetVBV());	//VBVを設定
	commandList_->IASetIndexBuffer(&sprite->GetIBV());	//IBVを設定
	commandList_->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(sprite->GetTextureIndex()));
	//マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, spriteMaterialResource_[spriteIndex_]->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, spriteWvpResource_[spriteIndex_]->GetGPUVirtualAddress());
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//ドローコール
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);

	spriteIndex_++;
}

void GameEngine::DrawInstancingSprite_2D_(std::list<Sprite*> sprits) {

	//上限に達していたら描画しない
	if (instancingSpriteIndex_ > kMaxInstanceIndex)return;

	std::list<Sprite*>::iterator SpriteIterator = sprits.begin();
	Sprite* startSprite = (*SpriteIterator);

	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(kWindowWidth_), float(kWindowHeight_), 0.0f, 100.0f);

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(instancingObjectRootSignature_.Get());
	commandList_->SetPipelineState(instancingObjectPipelineState_.Get());	//PSOを設定

	commandList_->IASetVertexBuffers(0, 1, &startSprite->GetVBV());	//VBVを設定
	commandList_->IASetIndexBuffer(&startSprite->GetIBV());	//IBVを設定

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//WVPデータを更新
	InstancingTransformationMatrix* mappedBase = nullptr;
	instancingSpriteResource_[instancingSpriteIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&mappedBase));
	// mappedBase が nullptr でないかチェック
	if (mappedBase == nullptr) {
		assert(0);
	}
	// 各要素ポインタを mappedBase に初期化
	for (uint32_t j = 0; j < kMaxNumInstance; ++j) {
		instancingSpriteData_[instancingSpriteIndex_][j] = mappedBase + j;
	}

	uint32_t numInstance = 0;
	//それぞれの情報をまとめる
	for (SpriteIterator = sprits.begin();
		SpriteIterator != sprits.end();) {

		if (numInstance >= kMaxNumInstance)break;

		Matrix4x4 worldMatrix = MakeAffineMatrix((*SpriteIterator)->GetTransform().translate, (*SpriteIterator)->GetTransform().rotate, (*SpriteIterator)->GetTransform().translate);
		instancingSpriteData_[instancingSpriteIndex_][numInstance]->World = worldMatrix;
		instancingSpriteData_[instancingSpriteIndex_][numInstance]->WorldInverseTranspose = Transpose(Inverse(worldMatrix));
		Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
		instancingSpriteData_[instancingSpriteIndex_][numInstance]->WVP = worldViewProjectionMatrix;

		instancingSpriteResource_[instancingSpriteIndex_]->Unmap(0, nullptr);

		++numInstance;
		++SpriteIterator;
	}

	Material material = startSprite->GetMaterial();
	SRT UVTransform = startSprite->GetUVTransform();

	material.uvTransform = MakeAffineMatrix(UVTransform.scale, UVTransform.rotate, UVTransform.translate);
	material.reflection = 0;
	material.enableDirectionalLighting = false;
	material.enablePointLighting = false;
	material.enableSpotLighting = false;
	material.shininess = 0.0f;
	material.color = Vector4{ 1.0f,1.0f,1.0f,1.0f };

	//マテリアルデータを更新
	spriteMaterialResource_[instancingSpriteIndex_]->Map(0, nullptr, reinterpret_cast<void**>(&spriteMaterialData_[instancingSpriteIndex_]));

	*spriteMaterialData_[instancingSpriteIndex_] = material;

	spriteMaterialResource_[instancingSpriteIndex_]->Unmap(0, nullptr);

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	commandList_->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(startSprite->GetTextureIndex()));

	commandList_->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(startInstancingSpriteIndex_ + instancingSpriteIndex_));

	//マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, spriteMaterialResource_[instancingSpriteIndex_]->GetGPUVirtualAddress());

	//描画(DrawCall)
	commandList_->DrawIndexedInstanced(6, numInstance, 0, 0, 0);

	instancingSpriteIndex_++;
}

void GameEngine::DrawLine_(std::list<Line> lines, PrimitiveManager::PrimitiveResource primitiveResource) {
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(instancingObjectRootSignature_.Get());
	commandList_->SetPipelineState(noDepthLinePipelineState_.Get());	//PSOを設定

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	commandList_->IASetVertexBuffers(0, 1, &PrimitiveManager::GetInstance()->GetVertexBufferView());	//VBVを設定
	commandList_->IASetIndexBuffer(&PrimitiveManager::GetInstance()->GetIndexBufferView());	//IBVを設定

	Camera* camera = Object::GetDefaultCamera().get();

	//カメラのワールド座標をCBufferに送る
	commandList_->SetGraphicsRootConstantBufferView(4, camera->CameraResource()->GetGPUVirtualAddress());

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

		Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

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
	commandList_->SetGraphicsRootSignature(instancingObjectRootSignature_.Get());
	commandList_->SetPipelineState(noDepthLinePipelineState_.Get());	//PSOを設定

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	commandList_->IASetVertexBuffers(0, 1, &PrimitiveManager::GetInstance()->GetVertexBufferView());	//VBVを設定
	commandList_->IASetIndexBuffer(&PrimitiveManager::GetInstance()->GetIndexBufferView());	//IBVを設定

	Camera* camera = Object::GetDefaultCamera().get();

	//カメラのワールド座標をCBufferに送る
	commandList_->SetGraphicsRootConstantBufferView(4, camera->CameraResource()->GetGPUVirtualAddress());

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

		Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

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
	commandList_->SetGraphicsRootSignature(instancingObjectRootSignature_.Get());
	commandList_->SetPipelineState(noDepthLinePipelineState_.Get());	//PSOを設定

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	commandList_->IASetVertexBuffers(0, 1, &PrimitiveManager::GetInstance()->GetVertexBufferView());	//VBVを設定
	commandList_->IASetIndexBuffer(&PrimitiveManager::GetInstance()->GetIndexBufferView());	//IBVを設定

	Camera* camera = Object::GetDefaultCamera().get();

	//カメラのワールド座標をCBufferに送る
	commandList_->SetGraphicsRootConstantBufferView(4, camera->CameraResource()->GetGPUVirtualAddress());

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

		Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

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