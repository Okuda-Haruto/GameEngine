#include "WindowsAPI.h"
#include <wrl.h>

#include "imgui/imgui.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int32_t WindowsAPI::kClientWidth_ = 1280;
int32_t WindowsAPI::kClientHeight_ = 720;

void WindowsAPI::Initialize(const wchar_t* WindowName, int32_t kWindowWidth = 1280, int32_t kWindowHeight = 720) {

	//COMの初期化
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);

	//ウィンドウクラスの生成
	w_ = WindowClass();

	//ウィンドウの生成
	hwnd_ = WindowInitialvalue(WindowName, kWindowWidth, kWindowHeight, w_);
}

void WindowsAPI::Update() {

}

void WindowsAPI::Finalize() {
	CloseWindow(hwnd_);
	CoUninitialize();
}



/// ウィンドウプロシージャ
LRESULT CALLBACK WindowsAPI::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

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
WNDCLASS WindowsAPI::WindowClass() {
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
RECT WindowsAPI::WindowSize(const int32_t ClientWidth, const int32_t ClientHeight) {

	kClientWidth_ = ClientWidth;
	kClientHeight_ = ClientHeight;

	//ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0,kClientWidth_,kClientHeight_ };

	//クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	return wrc;
}

// ウィンドウの生成
HWND WindowsAPI::WindowInitialvalue(const wchar_t* WindowName, const int32_t ClientWidth, const int32_t ClientHeight, WNDCLASS wc) {

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



bool WindowsAPI::ProcessMessage() {
	MSG msg{};
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	if (msg.message == WM_QUIT) {
		return true;
	}
	return false;
}