#pragma once
#include <Windows.h>
#include <cstdint>

#include <d3d12.h>

//WindowsAPI
class WindowsAPI {
public:
	//ウィンドウの幅
	static int32_t kClientWidth_;
	//ウィンドウの高さ
	static int32_t kClientHeight_;
private:
	//ウィンドウクラス
	WNDCLASS w_;
	//ウィンドウ
	HWND hwnd_ = nullptr;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	// ウィンドウクラス
	WNDCLASS WindowClass();
	// ウィンドウサイズ
	RECT WindowSize(const int32_t ClientWidth, const int32_t ClientHeight);
	// ウィンドウの生成
	HWND WindowInitialvalue(const wchar_t* WindowName, const int32_t ClientWidth, const int32_t ClientHeight, WNDCLASS wc);
public:

	//初期化
	void Initialize(const wchar_t* WindowName, int32_t kWindowWidth, int32_t kWindowHeight);
	//更新
	void Update();
	//終了
	void Finalize();

	bool ProcessMessage();

	HWND GetHwnd() const { return hwnd_; }
	HINSTANCE GetHInstance() const { return w_.hInstance; }
};