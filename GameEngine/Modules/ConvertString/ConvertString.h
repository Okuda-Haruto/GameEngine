#pragma once
#include<string>

//stringからwstringに変換
std::wstring ConvertString(const std::string& str);

//wstringからstringに変換
std::string ConvertString(const std::wstring& str);