#pragma once

#pragma comment(lib,"xaudio2.lib")
#include <xaudio2.h>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib,"Mf.lib")
#pragma comment(lib,"mfplat.lib")
#pragma comment(lib,"Mfreadwrite.lib")
#pragma comment(lib,"mfuuid.lib")

#include "SoundData.h"
#include "RiffHeader.h"
#include "FormatChunk.h"

#include <fstream>
#include <wrl.h>
#include <vector>
#include <list>

//音声
class Audio {
private:
	std::list<IXAudio2SourceVoice*> sourceVoiceList;

	//音量
	float volume_ = 1.0f;
	//音声データのループ再生するか
	bool isLoop_ = false;

	std::string path_;
public:
	~Audio();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="filename">.wavファイル名 (例:resources/Audio.wav)</param>
	/// <param name="isLoop">ループ再生するか</param>
	void Initialize(std::string path, float volume);

	/// <summary>
	/// 音量入力
	/// </summary>
	/// <param name="volume">音量 (0.0f ~ 1.0f)</param>
	void SetVolume(float volume);

	//音声データの再生
	void SoundPlayWave();
	//音声データの一時停止
	void SoundStopWave();
	//音声データの終了
	void SoundEndWave();
private:
	void IsPlaying();

	void DeleteSourceVoice(IXAudio2SourceVoice* sourceVoice);
};
