#pragma once
#include <format>

//チャンクヘッダ
struct ChunkHeader {
	char id[4];	//チャンク毎のID
	int32_t size;	//チャンクサイズ
};