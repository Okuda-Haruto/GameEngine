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
	//波形フォーマットを元にSourceVioceの生成
	IXAudio2SourceVoice* pSourceVoice_ = nullptr;
	//再生する波形データの設定
	XAUDIO2_BUFFER buf_{};

	//音量
	float Volume_ = 1.0f;
	//音声データのループ再生するか
	bool isLoop_ = false;
public:
	~Audio();
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="filename">.wavファイル名 (例:resources/Audio.wav)</param>
	/// <param name="xAudio2">XAudio2インスタンス</param>
	/// <param name="isLoop">ループ再生するか</param>
	void Initialize(const char* filename, Microsoft::WRL::ComPtr<IXAudio2> xAudio2,bool isLoop);

	/// <summary>
	/// .wavファイルの読み込み
	/// </summary>
	/// <param name="filename">.wavファイル名 (例:resources/Audio.wav)</param>
	void SoundLoadWave(const char* filename);

	// 音声データの解放
	void SoundUnload();

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
};
