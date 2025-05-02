#pragma once
#include "ChunkHeader.h"
#include "xaudio2.h"

//FMTチャンク
struct FormatChunk {
	ChunkHeader chunk;	//"fmt "
	WAVEFORMATEX fmt;	//波形フォーマット
};