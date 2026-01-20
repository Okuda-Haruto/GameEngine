#include "AudioHolder.h"
#include <AudioManager/AudioManager.h>

std::unique_ptr<AudioHolder> AudioHolder::instance;

AudioHolder* AudioHolder::GetInstance() {
	if (!instance) {
		instance = std::make_unique<AudioHolder>();
	}
	return instance.get();
}

void AudioHolder::Finalize() {
	audios_.clear();
	instance.reset();
}

void AudioHolder::Initialize() {
	std::vector<FilePath> filePaths_;
	FilePath filePath;

	//戦闘BGM
	filePath.directoryPath_ = "resources/Audio/BGM/Battle_BGM.mp3";
	filePath.isLoop_ = true;
	filePath.volume_ = 0.6f;
	filePaths_.push_back(filePath);
	//タイトルBGM
	filePath.directoryPath_ = "resources/Audio/BGM/Title_BGM.mp3";
	filePath.isLoop_ = true;
	filePath.volume_ = 0.6f;
	filePaths_.push_back(filePath);
	//射撃SE
	filePath.directoryPath_ = "resources/Audio/SE/Shot_SE.mp3";
	filePath.isLoop_ = false;
	filePath.volume_ = 0.5f;
	filePaths_.push_back(filePath);
	//リロードSE
	filePath.directoryPath_ = "resources/Audio/SE/Reload_SE.mp3";
	filePath.isLoop_ = false;
	filePath.volume_ = 0.6f;
	filePaths_.push_back(filePath);
	//プレイヤーダメ―ジSE
	filePath.directoryPath_ = "resources/Audio/SE/Player_Damage_SE.mp3";
	filePath.isLoop_ = false;
	filePath.volume_ = 0.4f;
	filePaths_.push_back(filePath);
	//ボスダメ―ジSE
	filePath.directoryPath_ = "resources/Audio/SE/Boss_Damage_SE.mp3";
	filePath.isLoop_ = false;
	filePath.volume_ = 0.6f;
	filePaths_.push_back(filePath);
	//回避SE
	filePath.directoryPath_ = "resources/Audio/SE/Dodge_SE.mp3";
	filePath.isLoop_ = false;
	filePath.volume_ = 0.6f;
	filePaths_.push_back(filePath);
	//柵衝突SE
	filePath.directoryPath_ = "resources/Audio/SE/Fence_Collision_SE.mp3";
	filePath.isLoop_ = false;
	filePath.volume_ = 0.6f;
	filePaths_.push_back(filePath);

	for (FilePath filePath : filePaths_) {
		AudioManager::GetInstance()->LoadAudio(filePath.directoryPath_, filePath.isLoop_);
		std::shared_ptr<Audio> audio = std::make_shared<Audio>();
		audio->Initialize(filePath.directoryPath_, filePath.isLoop_);
		audio->SetVolume(filePath.volume_);
		audios_.push_back(audio);
	}

}

std::weak_ptr<Audio> AudioHolder::GetAudio(AudioIndex audio) {
	return audios_[int32_t(audio)];
}