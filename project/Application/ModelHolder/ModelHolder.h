#pragma once
#include "Object/Object.h"

enum class ModelIndex {
	Skydome,
	Ground,
	Fence,
	Player,
	Boss,
	Bullet,
	Cylinder,
	Hat,
};

//使用するモデルを保持
class ModelHolder {
private:
	static ModelHolder* instance;

	ModelHolder() = default;
	~ModelHolder() = default;
	ModelHolder(ModelHolder&) = delete;
	ModelHolder& operator=(ModelHolder&) = delete;

	struct FilePath {
		std::string directoryPath_;
		std::string fileName_;
	};

	std::vector<FilePath> filePathes_;

public:
	static ModelHolder* GetInstance();

	void Finalize();

	//初期化
	void Initialize();

	Model* GetModel(ModelIndex model);
};