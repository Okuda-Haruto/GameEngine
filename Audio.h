#pragma once

#pragma comment(lib,"xaudio2.lib")
#include <xaudio2.h>

#include "SoundData.h"
#include "RiffHeader.h"
#include "FormatChunk.h"

#include <fstream>
#include <wrl.h>

//音声
class Audio {
private:
	//XAudio2インスタンス
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	//使用する音声データ
	SoundData soundData_;
public:
	~Audio();
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="filename">.wavファイル名 (例:resources/Audio.wav)</param>
	/// <param name="xAudio2">XAudio2インスタンス</param>
	void Initialize(const char* filename, Microsoft::WRL::ComPtr<IXAudio2> xAudio2);

	/// <summary>
	/// .wavファイルの読み込み
	/// </summary>
	/// <param name="filename">.wavファイル名 (例:resources/Audio.wav)</param>
	void SoundLoadWave(const char* filename);

	// 音声データの解放
	void SoundUnload();

	//音声データの再生
	void SoundPlayWave();
};
