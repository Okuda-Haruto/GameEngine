#pragma once
#include <cstdint>

#pragma comment(lib,"d3d12.lib")
#include <d3d12.h>

/// ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
// ウィンドウクラス
WNDCLASS WindowClass();
// ウィンドウサイズ
RECT WindowSize(const int32_t ClientWidth, const int32_t ClientHeight);
// ウィンドウの生成
HWND WindowInitialvalue(const wchar_t* WindowName, const int32_t ClientWidth, const int32_t ClientHeight, WNDCLASS wc);