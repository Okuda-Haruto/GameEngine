#pragma once
#include "Object/Object.h"

enum class ModelIndex {
	Skydome,
	Ground,
	Player,
};

//使用するモデルを保持
class ModelHolder {
private:
	struct FilePath {
		std::string directoryPath_;
		std::string fileName_;
	};

	std::vector<FilePath> filePathes_;

public:

	~ModelHolder();
	//初期化
	void Initialize();

	Model* GetModel(ModelIndex model);
};