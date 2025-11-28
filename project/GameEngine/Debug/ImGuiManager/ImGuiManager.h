#pragma once
#ifdef USE_IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
#endif

#include <DirectXCommon/DirectXCommon.h>
#include <WindowsAPI/WindowsAPI.h>
#include <SRVManager/SRVManager.h>

class ImGuiManager {
private:
	DirectXCommon* dxCommon_ = nullptr;
	SRVManager* srvManager_ = nullptr;
	uint32_t descriptorindex_;

public:
	~ImGuiManager();

	//終了
	void Finalize();

	//初期化
	void Initialize([[maybe_unused]] DirectXCommon* dxCommon, [[maybe_unused]] WindowsAPI* winapp, [[maybe_unused]] SRVManager* srvManager);

	void Begin();
	void End();
	void Draw();
};