#pragma once
#include "Audio/Audio.h"

enum class AudioIndex {
	Battle_BGM,
	Title_BGM,
	Shot_SE,
	Reload_SE,
	Player_Damage_SE,
	Boss_Damage_SE,
	Dodge_SE,
	Fence_Collision_SE,
};

//使用する音源を保持
class AudioHolder {
private:

	AudioHolder() = default;
	~AudioHolder() = default;
	AudioHolder(AudioHolder&) = delete;
	AudioHolder& operator=(AudioHolder&) = delete;
	
	struct FilePath {
		std::string directoryPath_;
		bool isLoop_;
		float volume_;
	};

	std::vector<std::shared_ptr<Audio>> audios_;

public:

	//シングルトンインスタンスの取得
	static AudioHolder* GetInstance()
	{
		static AudioHolder instance;
		return &instance;
	}

	void Finalize();

	//初期化
	void Initialize();

	std::weak_ptr<Audio> GetAudio(AudioIndex audio);
};