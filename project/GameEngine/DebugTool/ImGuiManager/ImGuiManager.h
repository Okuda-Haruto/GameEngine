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

	GuizmoData guizmoData_;
#ifdef USE_IMGUI
	ImGuizmo::OPERATION currentGizmoOperation_;
#endif
	Vector2 gameScreenSize_;
	Vector2 gameScreenPosition_;

	static bool isGameHovered_;
public:
	~ImGuiManager();

	//初期化
	void Initialize([[maybe_unused]] DirectXCommon* dxCommon, [[maybe_unused]] WindowsAPI* winapp, [[maybe_unused]] SRVManager* srvManager);

	void Begin();
	void End();
	void Draw();
#ifdef USE_IMGUI
	void SetGuizmo(std::weak_ptr<Camera> camera, SRT* transform) { guizmoData_.isNew = true; guizmoData_.camera = camera; guizmoData_.transform = transform; }
	void SetGizmoOperation(ImGuizmo::OPERATION gizmoOperation) { currentGizmoOperation_ = gizmoOperation; }
	Vector2 GetGameScreenSize() { return gameScreenSize_; }
	Vector2 GetGameScreenPosition() { return gameScreenPosition_; }

	static bool GetIsGameHovered() { return isGameHovered_; }
#endif
};