#pragma once

#include <filesystem>
#include <fstream>
#include <chrono>
#pragma comment(lib,"dxguid.lib")
#include <dxgidebug.h>

//ログの書き出し
void Log(std::ostream& os, const std::string& message);

//ログファイルの生成
std::ofstream CreateLogFile();