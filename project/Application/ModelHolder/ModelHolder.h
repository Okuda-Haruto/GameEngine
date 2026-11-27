#pragma once
#include "Object/Object.h"



enum class ModelIndex {
	Skydome,
	Ground,
	Fence,
	BackGround,
	Tumbleweed,
	Player,
	Boss,
	Bullet,
	Cylinder,
	Hat,
};

//使用するモデルを保持
class ModelHolder {
private:

	ModelHolder() = default;
	~ModelHolder() = default;
	ModelHolder(ModelHolder&) = delete;
	ModelHolder& operator=(ModelHolder&) = delete;

	struct FilePath {
		std::string directoryPath_;
		std::string fileName_;
	};

	std::vector<FilePath> filePaths_;

public:

	//シングルトンインスタンスの取得
	static ModelHolder* GetInstance()
	{
		static ModelHolder instance;
		return &instance;
	}

	void Finalize();

	//初期化
	void Initialize();

	const std::weak_ptr<Model> GetModel(ModelIndex model);
};