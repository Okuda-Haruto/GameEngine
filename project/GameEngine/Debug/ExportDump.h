#pragma once
#include <strsafe.h>
#include <Windows.h>

#pragma comment(lib,"Dbghelp.lib")
#include <DbgHelp.h>
#include <wrl.h>

//クラッシュ時、Dumpファイルを出力する(CrashHandler)
LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception);