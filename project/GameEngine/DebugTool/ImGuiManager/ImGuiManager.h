#pragma once
#ifdef USE_IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
#include <ImGuizmo/ImGuizmo.h>
#endif

#include <DirectXCommon/DirectXCommon.h>
#include <WindowsAPI/WindowsAPI.h>
#include <SRVManager/SRVManager.h>
#include <Camera/Camera.h>
#include <SRT.h>
#include <Vector2.h>

class ImGuiManager {
public:
	struct GuizmoData {
		std::weak_ptr<Camera> camera;
		SRT* transform;
		bool isNew;
	};
private:
	DirectXCommon* dxCommon_ = nullptr;
	SRVManager* srvManager_ = nullptr;
	uint32_t descriptorindex_;

	// GPUハンドルを保持してコールバックで使用する
	D3D12_GPU_DESCRIPTOR_HANDLE gameTextureGpuHandle_{};

	// ImGui描画用のスクリーンPSO/ルート署名
	Microsoft::WRL::ComPtr<ID3D12PipelineState> screenPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> screenRootSignature_ = nullptr;

	// コールバック用グローバル参照
	static ImGuiManager* s_callbackInstance_;

	GuizmoData guizmoData_;
#ifdef USE_IMGUI
	ImGuizmo::OPERATION currentGizmoOperation_;
#endif
	Vector2 gameScreenSize_;
	Vector2 gameScreenPosition_;

public:
	~ImGuiManager();

	//初期化
	void Initialize([[maybe_unused]] DirectXCommon* dxCommon, [[maybe_unused]] WindowsAPI* winapp, [[maybe_unused]] SRVManager* srvManager);

	void Begin();
	void End();
	void Draw();
	// ImGui コールバック関数
	static void RenderGameTextureCallback(const ImDrawList* parent_list, const ImDrawCmd* cmd);
#ifdef USE_IMGUI
	void SetGuizmo(std::weak_ptr<Camera> camera, SRT* transform) { guizmoData_.isNew = true; guizmoData_.camera = camera; guizmoData_.transform = transform; }
	void SetGizmoOperation(ImGuizmo::OPERATION gizmoOperation) { currentGizmoOperation_ = gizmoOperation; }
	Vector2 GetGameScreenSize() { return gameScreenSize_; }
	Vector2 GetGameScreenPosition() { return gameScreenPosition_; }
#endif
};