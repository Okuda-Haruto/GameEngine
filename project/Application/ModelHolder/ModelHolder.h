#pragma once
#include "Object/Object.h"

using namespace std;

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

	static unique_ptr<ModelHolder> instance;

	struct FilePath {
		std::string directoryPath_;
		std::string fileName_;
	};

	std::vector<FilePath> filePathes_;

public:

	ModelHolder() = default;
	~ModelHolder() = default;
	ModelHolder(ModelHolder&) = delete;
	ModelHolder& operator=(ModelHolder&) = delete;

	static ModelHolder* GetInstance();

	void Finalize();

	//初期化
	void Initialize();

	shared_ptr<Model> GetModel(ModelIndex model);
};