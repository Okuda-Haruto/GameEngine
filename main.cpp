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

#include "Object_3D.h"
#include "Object_2D.h"
#include "Texture.h"

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

//ウィンドウの幅
const int32_t kWindowWidth = 1280;
//ウィンドウの高さ
const int32_t kWindowHeight = 720;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//誰も捕捉しなかった場合に(Unhandled),捕捉する関数を登録
	SetUnhandledExceptionFilter(ExportDump);

#ifdef _DEBUG
	//リソースチェック
	D3DResourceLeakChecker leakCheck;
#endif

	CoInitializeEx(0, COINIT_MULTITHREADED);

	//ウィンドウの生成
	HWND hwnd = WindowInitialvalue(L"CG2", kWindowWidth, kWindowHeight);

	//ログファイルの生成
	std::ofstream logStream = CreateLogFile();

	//DXGIファクトリーの生成
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory = nullptr;

	//エラーコード
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr));

	//使用するアダプタ用変数
	Microsoft::WRL::ComPtr <IDXGIAdapter4> useAdapter = useAdapterInitialvalue(dxgiFactory, logStream);

	//使用するデバイスの初期値
	Microsoft::WRL::ComPtr<ID3D12Device> device = deviceInitialvalue(useAdapter, logStream);

	//DescriptorSizeを取得しておく
	const uint32_t descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	const uint32_t descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	const uint32_t descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//コマンドキューを生成する
	Microsoft::WRL::ComPtr <ID3D12CommandQueue> commandQueue = commandQueueInitialvalue(device);

	//コマンドアロケータを生成する
	Microsoft::WRL::ComPtr <ID3D12CommandAllocator> commandAllocator = commandAllocatorInitialvalue(device);

	//コマンドリストを生成する
	Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList = commandListInitialvalue(commandAllocator, device);

	Microsoft::WRL::ComPtr <IDXGISwapChain4> swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	//画面の幅
	swapChainDesc.Width = kWindowWidth;
	//画面の高さ
	swapChainDesc.Height = kWindowHeight;
	//色の形式
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//マルチサンプルしない
	swapChainDesc.SampleDesc.Count = 1;
	//描画のターゲットとして利用する
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//ダブルバッファ
	swapChainDesc.BufferCount = 2;
	//モニタにうつしたら、中身を破棄
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));
	assert(SUCCEEDED(hr));

	//RTV用のヒープでディスクリプタ数は2。RTVはShader内で触る物ではないので、ShaderVisibleはfalse
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	//SRV用のディスクリプタの数は128。SRVはShader内で触る物なので、ShaderVisibleはtrue
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	//SwapChainからResourcesを引っ張ってくる
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };
	for (int i = 0; i < 2; i++) {
		hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainResources[i]));
		//うまく取得できなければ起動しない
		assert(SUCCEEDED(hr));
	}

	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//出力結果をSRGBに変換して書き込む
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//2dテクスチャとして書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	//RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//まず1つ目を作る。1つ目は最初のところに作る。作る場所はこちらで指定する。
	rtvHandles[0] = GetCPUDescriptorHandle(rtvDescriptorHeap, descriptorSizeRTV, 0);
	device->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
	//2つ目を作る
	rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	device->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);

	//初期値0でFenceを作る
	Microsoft::WRL::ComPtr <ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;

	hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	//FenceのSignalを待つためのイベントを生成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

	//dxcCompilerを初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	IDxcIncludeHandler* includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

	//RootSignature作成
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature = rootSignatureInitialvalue(device,logStream);

	//Shaderをコンパイルする
	IDxcBlob* vertexShaderBlob = CompileShader(L"Object3D.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler, logStream);
	assert(vertexShaderBlob != nullptr);
	IDxcBlob* pixelShaderBlob = CompileShader(L"OBject3D.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler, logStream);
	assert(pixelShaderBlob != nullptr);

	//DepthStencilTextureをウィンドウのサイズで作成
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = CreateDepthStencilTextureResource(device, kWindowWidth, kWindowHeight);
	//DSV用のヒープディスクリプタの数は1。DSVはShader内で触る物ではないので、ShaderVisibleはfalse
	Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> dsvDescriptorheap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;	//2dTexture
	//DSVHeapの先頭にDSVを作る
	device->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvDescriptorheap->GetCPUDescriptorHandleForHeapStart());

	//PSOを生成
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = graphicsPipelineStateInitialvalue(device, logStream, rootSignature, vertexShaderBlob, pixelShaderBlob);

	/*
	//実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = CreateBufferResource(device, sizeof(VertexData) * 6);

	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	//左下
	vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	//上
	vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.5f,0.0f };
	//右下
	vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };

	//左下2
	vertexData[3].position = { -0.5f,-0.5f,0.5f,1.0f };
	vertexData[3].texcoord = { 0.0f,1.0f };
	//上2
	vertexData[4].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[4].texcoord = { 0.5f,0.0f };
	//右下2
	vertexData[5].position = { 0.5f,-0.5f,-0.5f,1.0f };
	vertexData[5].texcoord = { 1.0f,1.0f };
	*/

	/*
	//分割数
	const uint32_t kSubdivision = 16;

	//実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = CreateBufferResource(device, sizeof(VertexData) * kSubdivision * kSubdivision * 4);

	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * kSubdivision * kSubdivision * 4;
	//1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	//経度分割1つ分の角度φd
	const float kLonEvery = float(M_PI) * 2.0f / float(kSubdivision);
	//緯度分割1つ分の角度θd
	const float kLatEvery = float(M_PI) / float(kSubdivision);
	//緯度の方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex;	//θ
		//経度の方向に分割しながら線を描く
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 4;
			float lon = lonIndex * kLonEvery;	//φ
			//頂点にデータを入力する
			//左下
			vertexData[start].position.x = cos(lat) * cos(lon);
			vertexData[start].position.y = sin(lat);
			vertexData[start].position.z = cos(lat) * sin(lon);
			vertexData[start].position.w = 1.0f;
			vertexData[start].texcoord.x = float(lonIndex) / float(kSubdivision);
			vertexData[start].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
			//左上
			vertexData[start + 1].position.x = cos(lat + kLatEvery) * cos(lon);
			vertexData[start + 1].position.y = sin(lat + kLatEvery);
			vertexData[start + 1].position.z = cos(lat + kLatEvery) * sin(lon);
			vertexData[start + 1].position.w = 1.0f;
			vertexData[start + 1].texcoord.x = float(lonIndex) / float(kSubdivision);
			vertexData[start + 1].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
			//右下
			vertexData[start + 2].position.x = cos(lat) * cos(lon + kLonEvery);
			vertexData[start + 2].position.y = sin(lat);
			vertexData[start + 2].position.z = cos(lat) * sin(lon + kLonEvery);
			vertexData[start + 2].position.w = 1.0f;
			vertexData[start + 2].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
			vertexData[start + 2].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision);
			//右上
			vertexData[start + 3].position.x = cos(lat + kLatEvery) * cos(lon + kLonEvery);
			vertexData[start + 3].position.y = sin(lat + kLatEvery);
			vertexData[start + 3].position.z = cos(lat + kLatEvery) * sin(lon + kLonEvery);
			vertexData[start + 3].position.w = 1.0f;
			vertexData[start + 3].texcoord.x = float(lonIndex + 1) / float(kSubdivision);
			vertexData[start + 3].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);

		}
	}

	//法線の定義の追加
	for (uint32_t index = 0; index < kSubdivision * kSubdivision * 4; ++index) {
		vertexData[index].normal.x = vertexData[index].position.x;
		vertexData[index].normal.y = vertexData[index].position.y;
		vertexData[index].normal.z = vertexData[index].position.z;
	}

	//Sprite用のインデックスリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = CreateBufferResource(device, sizeof(uint32_t) * kSubdivision * kSubdivision * 6);

	//インデックスバッファビューを作成する
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	//リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックスは6つサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * kSubdivision * kSubdivision * 6;
	//インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex;	//θ
		//経度の方向に分割しながら線を描く
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			uint32_t startVertex = (latIndex * kSubdivision + lonIndex) * 4;
			indexData[start] = 0 + startVertex;
			indexData[start + 1] = 1 + startVertex;
			indexData[start + 2] = 2 + startVertex;
			indexData[start + 3] = 1 + startVertex;
			indexData[start + 4] = 3 + startVertex;
			indexData[start + 5] = 2 + startVertex;
		}
	}
	*/


	Object_3D* object1 = new Object_3D();

	object1->Initialize("resources", "axis.obj", device);

	Object_3D* object2 = new Object_3D();

	object2->Initialize("resources", "plane.obj", device);

	Object_2D* sprite = new Object_2D();

	sprite->Initialize(device);

	//Sprite用の頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 4);


	//光源用のリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = CreateBufferResource(device, sizeof(DirectionalLight));
	//データを書き込む
	DirectionalLight* directionalLightData = nullptr;
	//書き込むためのアドレスを取得
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	directionalLightData->color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;

	//ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = kWindowWidth;
	viewport.Height = kWindowHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//シザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = kWindowWidth;
	scissorRect.top = 0;
	scissorRect.bottom = kWindowHeight;

	//Transform変数を作る
	Transform transform{ {1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f} };
	//Transform変数を作る
	Transform transform2{{1.0f,1.0f,1.0f},
		{0.0f,180.0f * float(M_PI) / 180,0.0f},
		{1.0f,0.0f,0.0f} };
	Transform transformSprite{ {1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f} };
	//カメラ変数を作る。zが-10の位置でz+の方向を向いている
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };


	//テクスチャ読み込み
	uint32_t kLastCPUIndex = 1;
	uint32_t kLastGPUIndex = 1;

	Texture* spriteTexture = new Texture();

	spriteTexture->Initialize("resources/monsterBall.png", device, commandQueue, commandAllocator, commandList, fence, fenceValue, fenceEvent, srvDescriptorHeap, descriptorSizeSRV, kLastCPUIndex, kLastGPUIndex);

	Texture* object3DTexture1 = new Texture();

	object3DTexture1->Initialize((object1->ModelData()).material.textureFilePath, device, commandQueue, commandAllocator, commandList, fence, fenceValue, fenceEvent, srvDescriptorHeap, descriptorSizeSRV, kLastCPUIndex, kLastGPUIndex);

	Texture* object3DTexture2 = new Texture();

	object3DTexture2->Initialize((object2->ModelData()).material.textureFilePath, device, commandQueue, commandAllocator, commandList, fence, fenceValue, fenceEvent, srvDescriptorHeap, descriptorSizeSRV, kLastCPUIndex, kLastGPUIndex);




	Transform uvTransformSprite{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	Vector4 Color1 = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 Color2 = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 ColorSprite = { 1.0f,1.0f,1.0f,1.0f };


	//uint32_t* p = nullptr;
	//*p = 100;

	//ImGui初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(device.Get(), swapChainDesc.BufferCount,
		rtvDesc.Format,
		srvDescriptorHeap.Get(),
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	MSG msg{};
	//ウィンドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		//Windowのメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
#ifdef _DEBUG
			ID3D12InfoQueue* infoQueue = nullptr;
			if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
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
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			//ImGui
			ImGui::Text("Camera");
			ImGui::DragFloat3("cameraTranslate", &(cameraTransform.translate).x);
			ImGui::SliderAngle("cameraRatateX", &(cameraTransform.rotate).x, -180.0f, 180.0f);
			ImGui::SliderAngle("cameraRatateY", &(cameraTransform.rotate).y, -180.0f, 180.0f);
			ImGui::SliderAngle("cameraRatateZ", &(cameraTransform.rotate).z, -180.0f, 180.0f);
			ImGui::Text("Light");
			ImGui::DragFloat3("LightDirection", &(*directionalLightData).direction.x,0.01f);
			ImGui::Text("material");
			ImGui::DragFloat3("materialTranslate", &(transform.translate).x,0.01f);
			ImGui::SliderAngle("materialRatateX", &(transform.rotate).x, -180.0f, 180.0f);
			ImGui::SliderAngle("materialRatateY", &(transform.rotate).y, -180.0f, 180.0f);
			ImGui::SliderAngle("materialRatateZ", &(transform.rotate).z, -180.0f, 180.0f);
			ImGui::ColorPicker4("materialTextureColor", &Color1.x);
			ImGui::Text("material2");
			ImGui::DragFloat3("material2Translate", &(transform2.translate).x, 0.01f);
			ImGui::SliderAngle("material2RatateX", &(transform2.rotate).x, -180.0f, 180.0f);
			ImGui::SliderAngle("material2RatateY", &(transform2.rotate).y, -180.0f, 180.0f);
			ImGui::SliderAngle("material2RatateZ", &(transform2.rotate).z, -180.0f, 180.0f);
			ImGui::ColorPicker4("material2TextureColor", &Color2.x);
			ImGui::Text("sprite");
			ImGui::DragFloat3("Position", &(transformSprite.translate).x);
			ImGui::ColorPicker4("SpriteColor", &ColorSprite.x);
			ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
			ImGui::Text("directionalLiight");
			ImGui::DragFloat("intensity", &(directionalLightData->intensity), 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat3("direction", &(directionalLightData->direction).x, 0.01f, -1.0f, 1.0f);
			ImGui::ColorPicker4("color", &(directionalLightData->color).x);
			float sqrtNumber = sqrtf(sqrtf(powf(directionalLightData->direction.x, 2) + powf(directionalLightData->direction.y, 2)) + powf(directionalLightData->direction.z, 2));
			directionalLightData->direction.x = directionalLightData->direction.x / sqrtNumber;
			directionalLightData->direction.y = directionalLightData->direction.y / sqrtNumber;
			directionalLightData->direction.z = directionalLightData->direction.z / sqrtNumber;

			object1->SetTransform(transform);
			object1->SetUVTransform(uvTransformSprite);
			object1->SetColor(Color1);
			object1->SetCamera(cameraTransform);
			object2->SetTransform(transform2);
			object2->SetUVTransform(uvTransformSprite);
			object2->SetColor(Color2);
			object2->SetCamera(cameraTransform);
			sprite->SetTransform(transformSprite);
			sprite->SetUVTransform(uvTransformSprite);
			sprite->SetColor(ColorSprite);

			//描画用のDescriptorHeapの設定
			ID3D12DescriptorHeap* descriptorHeaps[] = {srvDescriptorHeap.Get()};
			commandList->SetDescriptorHeaps(1, descriptorHeaps);

			//ImGuiの内部コマンドを生成する
			ImGui::Render();

			//ゲーム処理部分

			//これから書き込むバックバッファのインデックスを取得
			UINT backBafferIndex = swapChain->GetCurrentBackBufferIndex();

			//TransitionBarrierの設定
			D3D12_RESOURCE_BARRIER barrier{};
			//今回のバリアはTransition
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			//Noneにしておく
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			//バリアを張る対象のリソース。現在のバックバッファに対して行う
			barrier.Transition.pResource = swapChainResources[backBafferIndex].Get();
			//遷移前(現在)のResourcesState
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			//遷移後のResourceState
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			//TransitionBarrierを張る
			commandList->ResourceBarrier(1, &barrier);

			//描画先のRTVとDSVを設定する
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetCPUDescriptorHandle(dsvDescriptorheap, descriptorSizeDSV, 0);
			commandList->OMSetRenderTargets(1, &rtvHandles[backBafferIndex], false, &dsvHandle);
			//指定した色で画面全体をクリアする
			float crearColor[] = { 0.1f,0.25f,0.5f,1.0f };//青っぽい色。RGBAの順
			commandList->ClearRenderTargetView(rtvHandles[backBafferIndex], crearColor, 0, nullptr);
			//指定した深度で画面全体をクリアする
			commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);



			commandList->RSSetViewports(1, &viewport);			//Viewportを設定
			commandList->RSSetScissorRects(1, &scissorRect);	//Scirssorを設定
			//RootSignatureを設定。PSOに設定しているけど別途設定が必要
			commandList->SetGraphicsRootSignature(rootSignature.Get());
			commandList->SetPipelineState(graphicsPipelineState.Get());	//PSOを設定
			//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばよい
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			object1->Draw(commandList, directionalLightResource, object3DTexture1->textureSrvHandleGPU());

			object2->Draw(commandList, directionalLightResource, object3DTexture2->textureSrvHandleGPU());

			sprite->Draw(commandList, spriteTexture->textureSrvHandleGPU());

			//実際のcommandListのImGuiの描画コマンドを詰む
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());


			//RenderTargetからPresentにする
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			//TransitionBarrierを張る
			commandList->ResourceBarrier(1, &barrier);

			//コマンドリストの内容を確定させる。すべてのコマンドを詰んでからCloseすること
			hr = commandList->Close();
			assert(SUCCEEDED(hr));

			//GPUにコマンドリストの実行を行わせる
			ID3D12CommandList* commandLists[] = {commandList.Get()};
			commandQueue->ExecuteCommandLists(1, commandLists);
			//GPUとOSに画面の交換を行うよう通知する
			swapChain->Present(1, 0);

			//Fenceの値を更新
			fenceValue++;
			//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
			commandQueue->Signal(fence.Get(), fenceValue);

			//Fenceの値が指定したSignal値に辿り着いているか確認する
			//GetCompletedValueの初期値はFence作成時に渡した初期値
			if (fence->GetCompletedValue() < fenceValue) {
				//指定したSignalに辿り着いていないので、辿り着くまで待つようにイベントを設定する
				fence->SetEventOnCompletion(fenceValue, fenceEvent);
				//イベントを待つ
				WaitForSingleObject(fenceEvent, INFINITE);
			}

			//次のフレーム用のコマンドリストを準備
			hr = commandAllocator->Reset();
			assert(SUCCEEDED(hr));
			hr = commandList->Reset(commandAllocator.Get(), nullptr);
			assert(SUCCEEDED(hr));
		}
	}		
	//ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CloseHandle(fenceEvent);
	CloseWindow(hwnd);

	CoUninitialize();

	return 0;
}
