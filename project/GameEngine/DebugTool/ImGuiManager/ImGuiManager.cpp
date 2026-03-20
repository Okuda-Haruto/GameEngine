#include "ImGuiManager.h"

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
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winapp->GetHwnd());
	ImGui_ImplDX12_Init(dxCommon_->GetDevice(), dxCommon_->SwapChainBufferCount(),
		dxCommon_->GetRTVFormat(),
		srvManager_->GetDescriptorHeap().Get(),
		srvManager_->GetCPUDescriptorHandle(descriptorindex_),
		srvManager_->GetGPUDescriptorHandle(descriptorindex_));

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("resources/DebugResources/x12y16pxMaruMonica .ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
#endif
}

void ImGuiManager::Begin() {
#ifdef USE_IMGUI
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
}

void ImGuiManager::End() {
#ifdef USE_IMGUI
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