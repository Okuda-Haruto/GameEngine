#include "Audio.h"
#include "ConvertString.h"
#include <GameEngine.h>
#include <AudioManager/AudioManager.h>

#include <cassert>

Audio::~Audio() {
	SoundEndWave();
}

void Audio::Initialize(std::string path, float volume) {
	path_ = path;
	volume_ = volume;
}

void Audio::SetVolume(float volume) {

	volume_ = volume;
	for (IXAudio2SourceVoice* sourceVoice : sourceVoiceList) {
		sourceVoice->SetVolume(volume_, XAUDIO2_COMMIT_NOW);
	}

}

void Audio::SoundPlayWave() {

	HRESULT hr;

	//終了しているか判定
	IsPlaying();

	IXAudio2SourceVoice* newSourceVoice;

	newSourceVoice = AudioManager::GetInstance()->CreateSourceVoice(path_);

	newSourceVoice->SetVolume(volume_, XAUDIO2_COMMIT_NOW);

	//波形データの再生
	hr = newSourceVoice->Start();
	assert(SUCCEEDED(hr));

	sourceVoiceList.push_back(newSourceVoice);

}

void Audio::SoundStopWave() {

	HRESULT hr;

	//終了しているか判定
	IsPlaying();

	for (IXAudio2SourceVoice* sourceVoice : sourceVoiceList) {
		hr = sourceVoice->Stop();
		assert(SUCCEEDED(hr));
	}

}

void Audio::SoundEndWave() {

	for (IXAudio2SourceVoice* sourceVoice : sourceVoiceList) {
		DeleteSourceVoice(sourceVoice);
	}
	sourceVoiceList.clear();

}

bool Audio::IsSoundPlayingWave() {
	//終了しているか判定
	IsPlaying();

	return !sourceVoiceList.empty();
}

void Audio::IsPlaying() {

	//再生中じゃない音源は消す
	for (std::list<IXAudio2SourceVoice*>::iterator sourceVoiceIterator = sourceVoiceList.begin();
		sourceVoiceIterator != sourceVoiceList.end();) {

		XAUDIO2_VOICE_STATE state;
		(*sourceVoiceIterator)->GetState(&state);

		if (state.BuffersQueued == 0) {
			DeleteSourceVoice((*sourceVoiceIterator));
			sourceVoiceIterator = sourceVoiceList.erase(sourceVoiceIterator);
			continue;
		}

		sourceVoiceIterator++;
	}
}

void Audio::DeleteSourceVoice(IXAudio2SourceVoice* sourceVoice) {

	sourceVoice->DestroyVoice();

	sourceVoice = nullptr;
}