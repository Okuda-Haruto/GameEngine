#include "ImGuiManager.h"
#include <TextureManager/TextureManager.h>

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
#endif
}

void ImGuiManager::Begin() {
#ifdef USE_IMGUI
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport();
#endif
}

void ImGuiManager::End() {
#ifdef USE_IMGUI

	//Gameウィンドウの描画
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::Begin("Game");

	ImVec2 avail = ImGui::GetContentRegionAvail();

	float aspect = 16.0f / 9.0f;

	float width = avail.x;
	float height = width / aspect;

	if (height > avail.y)
	{
		height = avail.y;
		width = height * aspect;
	}

	auto gpuHandle =
		srvManager_->GetGPUDescriptorHandle(TextureManager::GetInstance()->GetSrvIndex("ImGui"));

	ImTextureRef tex_ref(
		(ImTextureID)gpuHandle.ptr
	);

	ImGui::Image(
		tex_ref,
		ImVec2(width, height)
	);

	ImGui::End();

	ImGui::PopStyleVar(2);

	//ImGuiの内部コマンドを生成する
	ImGui::Render();
#endif
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