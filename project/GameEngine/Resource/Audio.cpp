#include "Audio.h"
#include "ConvertString.h"
#include <GameEngine.h>

#include <cassert>

Audio::~Audio() {
	//音声データの解放
	SoundUnload();

	CoTaskMemFree(waveFormat);
	pMFMediaType_->Release();
	pMFSourceReader_->Release();
	MFShutdown();

	CoUninitialize();

}

void Audio::Initialize(std::string path, bool isLoop) {

	MFStartup(MF_VERSION);

	HRESULT hr;

	xAudio2_ = GameEngine::GetXAudio2();

	Volume_ = 1.0f;
	isLoop_ = isLoop;

	path_ = ConvertString(path);

	//ソースリーダーを作成
	MFCreateAttributes(&pAttr_, 1);
	pAttr_->SetUINT32(MF_LOW_LATENCY, TRUE);
	hr = MFCreateSourceReaderFromURL(path_.c_str(), pAttr_.Get(), &pMFSourceReader_);
	assert(SUCCEEDED(hr));

	//メディアタイプを作成
	MFCreateMediaType(&pMFMediaType_);
	pMFMediaType_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pMFMediaType_->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	//メディアタイプを指定
	pMFSourceReader_->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMFMediaType_);
	pMFMediaType_->Release();
	pMFMediaType_ = nullptr;

	//指定したメディアタイプを取得
	pMFSourceReader_->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMFMediaType_);

	//メディアタイプからWAVEFORMATMATEXを生成
	MFCreateWaveFormatExFromMFMediaType(pMFMediaType_, &waveFormat, nullptr);

	//データの読み込み
	while (true) {
		IMFSample* pMFSample = nullptr;
		DWORD dwStreamFlags = 0;
		pMFSourceReader_->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

		//ストリームが終わったらループを抜ける
		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
			break;
		}

		IMFMediaBuffer* pMFMediaBuffer = nullptr;
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		BYTE* pBuffer = nullptr;
		DWORD cbCurrentLength = 0;
		pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

		mediaData.resize(mediaData.size() + cbCurrentLength);
		memcpy(mediaData.data() + mediaData.size() - cbCurrentLength,
			pBuffer,
			cbCurrentLength);

		pMFMediaBuffer->Unlock();

		pMFMediaBuffer->Release();
		pMFSample->Release();
	}

	//波形フォーマットを元にSourceVioceの生成
	hr = xAudio2_->CreateSourceVoice(&pSourceVoice_, waveFormat);
	assert(SUCCEEDED(hr));

	//再生する波形データの設定
	buf_.pAudioData = mediaData.data();
	buf_.AudioBytes = sizeof(BYTE) * UINT32(mediaData.size());
	buf_.Flags = XAUDIO2_END_OF_STREAM;
	if (isLoop_) {
		buf_.LoopCount = XAUDIO2_LOOP_INFINITE;	//無限ループ
	} else {
		buf_.LoopCount = 0;						//ループしない
	}
	//設定入力
	hr = pSourceVoice_->SubmitSourceBuffer(&buf_);
	assert(SUCCEEDED(hr));

	pSourceVoice_->GetVolume(&Volume_);

	MFShutdown();

}

void Audio::SoundLoadWave(bool isLoop) {

	isLoop_ = isLoop;

	//ソースリーダーを作成
	MFCreateSourceReaderFromURL(path_.c_str(), NULL, &pMFSourceReader_);
	//メディアタイプを作成
	MFCreateMediaType(&pMFMediaType_);
	pMFMediaType_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pMFMediaType_->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	//メディアタイプを指定
	pMFSourceReader_->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMFMediaType_);
	pMFMediaType_->Release();
	pMFMediaType_ = nullptr;

	//指定したメディアタイプを取得
	pMFSourceReader_->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMFMediaType_);

	//メディアタイプからWAVEFORMATMATEXを生成
	MFCreateWaveFormatExFromMFMediaType(pMFMediaType_, &waveFormat, nullptr);

	//データの読み込み
	while (true) {
		IMFSample* pMFSample = nullptr;
		DWORD dwStreamFlags = 0;
		pMFSourceReader_->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

		//ストリームが終わったらループを抜ける
		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
			break;
		}

		IMFMediaBuffer* pMFMediaBuffer = nullptr;
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		BYTE* pBuffer = nullptr;
		DWORD cbCurrentLength = 0;
		pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

		mediaData.resize(mediaData.size() + cbCurrentLength);
		memcpy(mediaData.data() + mediaData.size() - cbCurrentLength,
			pBuffer,
			cbCurrentLength);

		pMFMediaBuffer->Unlock();

		pMFMediaBuffer->Release();
		pMFSample->Release();
	}

	//波形フォーマットを元にSourceVioceの生成
	HRESULT hr = xAudio2_->CreateSourceVoice(&pSourceVoice_, waveFormat);
	assert(SUCCEEDED(hr));

	//再生する波形データの設定
	buf_.pAudioData = mediaData.data();
	buf_.AudioBytes = sizeof(BYTE) * UINT32(mediaData.size());
	buf_.Flags = XAUDIO2_END_OF_STREAM;
	if (isLoop_) {
		buf_.LoopCount = XAUDIO2_LOOP_INFINITE;	//無限ループ
	} else {
		buf_.LoopCount = 0;						//ループしない
	}
	//設定入力
	hr = pSourceVoice_->SubmitSourceBuffer(&buf_);
	assert(SUCCEEDED(hr));

	pSourceVoice_->GetVolume(&Volume_);
}

void Audio::SoundUnload() {
	//バッファのメモリを解放
	delete[] soundData_.pBuffer;

	soundData_.pBuffer = 0;
	soundData_.bufferSize = 0;
	soundData_.wfex = {};
	
	pSourceVoice_->DestroyVoice();
}

void Audio::SetVolume(float volume) {

	Volume_ = volume;
	pSourceVoice_->SetVolume(Volume_);

}

void Audio::SoundPlayWave() {

	HRESULT hr;

	//終了しているか判定
	XAUDIO2_VOICE_STATE state;
	pSourceVoice_->GetState(&state);

	if (state.BuffersQueued == 0) {
		SoundEndWave();
	}

	//波形データの再生
	hr = pSourceVoice_->Start();
	assert(SUCCEEDED(hr));

}

void Audio::SoundStopWave() {

	HRESULT hr;

	hr = pSourceVoice_->Stop();
	assert(SUCCEEDED(hr));

}

void Audio::SoundEndWave() {

	HRESULT hr;

	hr = pSourceVoice_->Stop();
	assert(SUCCEEDED(hr));

	//バッファを削除し、再生位置を元に戻す
	pSourceVoice_->FlushSourceBuffers();

	//設定入力
	hr = pSourceVoice_->SubmitSourceBuffer(&buf_);
	assert(SUCCEEDED(hr));

	pSourceVoice_->GetVolume(&Volume_);

}