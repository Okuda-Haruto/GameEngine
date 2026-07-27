#include "ImGuiManager.h"
#include <TextureManager/TextureManager.h>
#include "Initialvalue.h"
#include <d3d12.h>

// static メンバの定義
ImGuiManager* ImGuiManager::s_callbackInstance_ = nullptr;

ImGuiManager::~ImGuiManager() {
#ifdef USE_IMGUI
	//ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif
}

void ImGuiManager::Initialize([[maybe_unused]] DirectXCommon* dxCommon, [[maybe_unused]] WindowsAPI* winapp, [[maybe_unused]] SRVManager* srvManager) {
#ifdef USE_IMGUI
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	// コールバックインスタンスを登録
	s_callbackInstance_ = this;

	descriptorindex_ = srvManager_->Allocate();

	//ImGui初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.Fonts->AddFontFromFileTTF("resources/DebugResources/x12y16pxMaruMonica .ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winapp->GetHwnd());

	ImGui_ImplDX12_InitInfo init_info = {};
	init_info.Device = dxCommon_->GetDevice();
	init_info.CommandQueue = dxCommon_->GetCommandQueue();
	init_info.NumFramesInFlight = dxCommon_->SwapChainBufferCount();
	init_info.RTVFormat = dxCommon_->GetRTVFormat();
	init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;

	// フォントテクスチャ用SRV
	init_info.SrvDescriptorHeap = srvManager_->GetDescriptorHeap().Get();
	init_info.SrvDescriptorAllocFn =
		[](ImGui_ImplDX12_InitInfo* info,
			D3D12_CPU_DESCRIPTOR_HANDLE* cpuHandle,
			D3D12_GPU_DESCRIPTOR_HANDLE* gpuHandle)
		{
			auto* srvManager =
				static_cast<SRVManager*>(info->UserData);

			uint32_t index = srvManager->Allocate();

			*cpuHandle = srvManager->GetCPUDescriptorHandle(index);
			*gpuHandle = srvManager->GetGPUDescriptorHandle(index);
		};
	init_info.SrvDescriptorFreeFn =
		[](ImGui_ImplDX12_InitInfo* info,
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
		{
			// 必要なら解放処理
		};
	init_info.UserData = srvManager_;

	ImGuiDockNodeFlags dockspace_flags =
		ImGuiDockNodeFlags_PassthruCentralNode;

	ImGui_ImplDX12_Init(&init_info);

	// スクリーン用PSOを自前で作成しておく（ImGuiが使うデフォルトPSOと差し替え）
	{
		// シェーダーをコンパイル
		auto copyVS = dxCommon_->CompileShader(L"./resources/Shader/CopyImage.VS.hlsl", L"vs_6_0");
		auto copyPS = dxCommon_->CompileShader(L"./resources/Shader/CopyImage.PS.hlsl", L"ps_6_0");
		assert(copyVS != nullptr && copyPS != nullptr);

		// ルート署名はDirectXCommonのスクリーン用を再利用
		screenRootSignature_ = dxCommon_->Screen_RootSignatureInitialvalue();
		// PSOを生成
		screenPipelineState_ = Screen_PipelineStateInitialvalue(dxCommon_->GetDevice(), screenRootSignature_, copyVS.Get(), copyPS.Get());
		assert(screenPipelineState_ != nullptr);
	}

	currentGizmoOperation_ = ImGuizmo::TRANSLATE;
#endif
}

void ImGuiManager::Begin() {
#ifdef USE_IMGUI
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	ImGui::DockSpaceOverViewport();
#endif
}

void ImGuiManager::End() {
#ifdef USE_IMGUI

	//Gameウィンドウの描画
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::Begin("Game");

	ImGuizmo::SetDrawlist();

	ImVec2 avail = ImGui::GetContentRegionAvail();

	float aspect = 16.0f / 9.0f;

	gameScreenSize_.x = avail.x;
	gameScreenSize_.y = gameScreenSize_.x / aspect;

	if (gameScreenSize_.x > avail.y)
	{
		gameScreenSize_.y = avail.y;
		gameScreenSize_.x = gameScreenSize_.y * aspect;
	}

	auto gpuHandle =
		srvManager_->GetGPUDescriptorHandle(TextureManager::GetInstance()->GetSrvIndex("ImGui"));

	// GPUハンドルを保存してコールバックで使う
	gameTextureGpuHandle_ = gpuHandle;
	
	//guizmo用pos
	ImVec2 imagePos = ImGui::GetCursorScreenPos();
	gameScreenPosition_ = { imagePos.x,imagePos.y };

	// ImGui 側の標準描画は使わず、コールバックで自前のPSOを使って描画する。
	// レイアウト用にダミーを配置して、描画コマンドを挿入する
	ImGui::Dummy(ImVec2(gameScreenSize_.x, gameScreenSize_.y));
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	// コールバックデータとして this を渡す
	draw_list->AddCallback(ImGuiManager::RenderGameTextureCallback, this);

	//上記のゲーム画面から範囲を取得
	ImGuizmo::SetRect(
		imagePos.x,
		imagePos.y,
		gameScreenSize_.x,
		gameScreenSize_.y
	);

	if (guizmoData_.isNew) {
		Matrix4x4 matrix{};

		ImGuizmo::RecomposeMatrixFromComponents(
			&guizmoData_.transform->translate.x,
			&guizmoData_.transform->rotate.x,
			&guizmoData_.transform->scale.x,
			&matrix.m[0][0]
		);

		ImGuizmo::Manipulate(
			(float*)&guizmoData_.camera.lock()->GetViewMatrix().m[0][0],
			(float*)&guizmoData_.camera.lock()->GetProjectionMatrix().m[0][0],
			currentGizmoOperation_,
			ImGuizmo::LOCAL,
			(float*)&matrix.m[0][0]
		);

		ImGuizmo::DecomposeMatrixToComponents(
			&matrix.m[0][0],
			&guizmoData_.transform->translate.x,
			&guizmoData_.transform->rotate.x,
			&guizmoData_.transform->scale.x
		);

		guizmoData_.isNew = false;
	}

	ImGui::End();

	ImGui::PopStyleVar(2);

	//ImGuiの内部コマンドを生成する
	ImGui::Render();
#endif
}

// ImGui の描画コールバック。ImDrawCmd::UserCallbackData に ImGuiManager* が入る
void ImGuiManager::RenderGameTextureCallback(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
	// UserCallbackData に渡した this を取り出す
	ImGuiManager* self = static_cast<ImGuiManager*>(cmd->UserCallbackData);
	if (!self) return;

	ID3D12GraphicsCommandList* commandList = self->dxCommon_->GetCommandList();
	// デスクリプタヒープをセット
	ID3D12DescriptorHeap* ppHeaps[] = { self->srvManager_->GetDescriptorHeap().Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// ルート署名とPSOを設定
	commandList->SetGraphicsRootSignature(self->screenRootSignature_.Get());
	commandList->SetPipelineState(self->screenPipelineState_.Get());

	// SRVをルートにバインド (ルートパラメータ1がDescriptorTable)
	commandList->SetGraphicsRootDescriptorTable(1, self->gameTextureGpuHandle_);

	// トポロジと描画
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ビューポートとシザーを描画領域に設定
	D3D12_VIEWPORT vp{};
	vp.TopLeftX = self->gameScreenPosition_.x;
	vp.TopLeftY = self->gameScreenPosition_.y;
	vp.Width = self->gameScreenSize_.x;
	vp.Height = self->gameScreenSize_.y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	commandList->RSSetViewports(1, &vp);

	D3D12_RECT rect{};
	rect.left = static_cast<LONG>(self->gameScreenPosition_.x);
	rect.top = static_cast<LONG>(self->gameScreenPosition_.y);
	rect.right = rect.left + static_cast<LONG>(self->gameScreenSize_.x);
	rect.bottom = rect.top + static_cast<LONG>(self->gameScreenSize_.y);
	commandList->RSSetScissorRects(1, &rect);

	// フルスクリーン三角形で描画
	commandList->DrawInstanced(3, 1, 0, 0);
}

void ImGuiManager::Draw() {
#ifdef USE_IMGUI
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	ID3D12DescriptorHeap* ppHeaps[] = { srvManager_->GetDescriptorHeap().Get() };

	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//実際のcommandListのImGuiの描画コマンドを詰む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

#endif
}