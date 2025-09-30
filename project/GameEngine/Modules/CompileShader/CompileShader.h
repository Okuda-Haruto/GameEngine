#pragma once

#include <Windows.h>

#pragma comment(lib,"dxcompiler.lib")
#include <dxcapi.h>
#include <string>
#include <sstream>

IDxcBlob* CompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfile
	const wchar_t* profile,
	//初期化で生成したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler,
	//ログファイル
	std::ofstream& logStream);