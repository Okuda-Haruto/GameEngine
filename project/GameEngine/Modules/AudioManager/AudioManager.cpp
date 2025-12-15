#include "AudioManager.h"
#include "ConvertString.h"
#include <GameEngine.h>

#include <cassert>

unique_ptr<AudioManager> AudioManager::instance;

AudioManager* AudioManager::GetInstance() {
	if (!instance) {
		instance = make_unique<AudioManager>();
	}
	return instance.get();
}

void AudioManager::Finalize() {
	//音声データの解放

	for (auto& data : audioData_) {
		CoTaskMemFree(data.second.waveFormat);
		data.second.pMFMediaType_->Release();
		data.second.pMFSourceReader_->Release();
	}
	MFShutdown();

	CoUninitialize();

	instance.reset();
}

void AudioManager::Initialize(Microsoft::WRL::ComPtr<IXAudio2> xAudio2) {

	xAudio2_ = xAudio2;

}

void AudioManager::LoadAudio(std::string path, bool isLoop = false) {

	std::wstring path_ = ConvertString(path);
	
	if (audioData_.contains(path_)) {
		return;
	}

	MFStartup(MF_VERSION);

	//ソースリーダーを作成
	MFCreateAttributes(&audioData_[path_].pAttr_, 1);
	audioData_[path_].pAttr_->SetUINT32(MF_LOW_LATENCY, TRUE);
	HRESULT hr = MFCreateSourceReaderFromURL(path_.c_str(), audioData_[path_].pAttr_.Get(), &audioData_[path_].pMFSourceReader_);
	assert(SUCCEEDED(hr));

	//メディアタイプを作成
	MFCreateMediaType(&audioData_[path_].pMFMediaType_);
	audioData_[path_].pMFMediaType_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	audioData_[path_].pMFMediaType_->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	//メディアタイプを指定
	audioData_[path_].pMFSourceReader_->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, audioData_[path_].pMFMediaType_);
	audioData_[path_].pMFMediaType_->Release();
	audioData_[path_].pMFMediaType_ = nullptr;

	//指定したメディアタイプを取得
	audioData_[path_].pMFSourceReader_->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &audioData_[path_].pMFMediaType_);

	//メディアタイプからWAVEFORMATMATEXを生成
	MFCreateWaveFormatExFromMFMediaType(audioData_[path_].pMFMediaType_, &audioData_[path_].waveFormat, nullptr);

	//データの読み込み
	while (true) {
		IMFSample* pMFSample = nullptr;
		DWORD dwStreamFlags = 0;
		audioData_[path_].pMFSourceReader_->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

		//ストリームが終わったらループを抜ける
		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
			break;
		}

		IMFMediaBuffer* pMFMediaBuffer = nullptr;
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		BYTE* pBuffer = nullptr;
		DWORD cbCurrentLength = 0;
		pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

		audioData_[path_].mediaData.resize(audioData_[path_].mediaData.size() + cbCurrentLength);
		memcpy(audioData_[path_].mediaData.data() + audioData_[path_].mediaData.size() - cbCurrentLength,
			pBuffer,
			cbCurrentLength);

		pMFMediaBuffer->Unlock();

		pMFMediaBuffer->Release();
		pMFSample->Release();
	}

	XAUDIO2_BUFFER* buf = &audioData_[path_].buffer_;

	//再生する波形データの設定
	buf->pAudioData = audioData_[path_].mediaData.data();
	buf->AudioBytes = sizeof(BYTE) * UINT32(audioData_[path_].mediaData.size());
	buf->Flags = XAUDIO2_END_OF_STREAM;
	if (isLoop) {
		buf->LoopCount = XAUDIO2_LOOP_INFINITE;	//無限ループ
	} else {
		buf->LoopCount = 0;						//ループしない
	}

	MFShutdown();
}

IXAudio2SourceVoice* AudioManager::CreateSourceVoice(std::string path) {

	std::wstring path_ = ConvertString(path);

	if (!(audioData_.contains(path_))) {
		//バッファがないならもう一度(ループするか判断できないのでループはしない)
		LoadAudio(path, false);
	}

	IXAudio2SourceVoice* sourceVoice;

	//波形フォーマットを元にSourceVioceの生成
	HRESULT hr = xAudio2_->CreateSourceVoice(&sourceVoice, audioData_[path_].waveFormat);
	assert(SUCCEEDED(hr));

	//設定入力
	hr = sourceVoice->SubmitSourceBuffer(&audioData_[path_].buffer_);
	assert(SUCCEEDED(hr));

	return sourceVoice;
}