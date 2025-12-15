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
#include <unordered_map>

using namespace std;

//音声
class AudioManager {
private:
	static unique_ptr<AudioManager> instance;

	//XAudio2インスタンス
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;

	struct AudioData {
		//再生する波形データの設定
		XAUDIO2_BUFFER buffer_;

		//メディアファンデーション	ソースリーダー
		IMFSourceReader* pMFSourceReader_ = nullptr;
		//メディアタイプ
		IMFMediaType* pMFMediaType_ = nullptr;
		//オーディオデータ形式
		WAVEFORMATEX* waveFormat = nullptr;
		//オーディオデータ
		std::vector<BYTE> mediaData;

		Microsoft::WRL::ComPtr<IMFAttributes> pAttr_ = nullptr;
	};

	std::unordered_map<std::wstring, AudioData> audioData_;

public:

	AudioManager() = default;
	~AudioManager() = default;
	AudioManager(AudioManager&) = delete;
	AudioManager& operator=(AudioManager&) = delete;

	//シングルトンインスタンスの取得
	static AudioManager* GetInstance();

	//終了
	void Finalize();

	void Initialize(Microsoft::WRL::ComPtr<IXAudio2> xAudio2);

	void LoadAudio(std::string path, bool isLoop);

	IXAudio2SourceVoice* CreateSourceVoice(std::string path);
};