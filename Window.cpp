#include"Window.h"
#include <wrl.h>

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/// ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

// ウィンドウクラス
WNDCLASS WindowClass() {
	//ウィンドウクラス
	WNDCLASS wc{};
	//ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	//ウィンドウクラス名
	wc.lpszClassName = L"WindowClass";
	//インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	//カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//ウィンドウクラスを追加する
	RegisterClass(&wc);

	return wc;
}

// ウィンドウサイズ
RECT WindowSize(const int32_t ClientWidth,const int32_t ClientHeight){
	//ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0,ClientWidth,ClientHeight };

	//クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	return wrc;
}
// ウィンドウの生成
HWND WindowInitialvalue(const wchar_t* WindowName, const int32_t ClientWidth, const int32_t ClientHeight, WNDCLASS wc) {

	//ウィンドウクラスの生成
	wc = WindowClass();

	//クライアント領域の指定
	RECT wrc = WindowSize(ClientWidth, ClientHeight);

	//ウィンドウの生成
	HWND hwnd = CreateWindow(
		wc.lpszClassName,
		WindowName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	//ウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);

	return hwnd;
}