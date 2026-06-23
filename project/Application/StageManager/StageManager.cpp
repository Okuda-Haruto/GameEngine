#include "StageManager.h"
#include <fstream>
#include <nlohmann/json.hpp>

std::unique_ptr<StageManager> StageManager::instance;

StageManager* StageManager::GetInstance() {
	if (!instance) {
		instance = std::make_unique<StageManager>();
	}
	return instance.get();
}

void StageManager::Finalize() {
	//データの解放
	stageDatas_.clear();

	instance.reset();
}

StageData StageManager::GetStageData(std::string stageName) {
	//読み込み済みステージを検索
	if (stageDatas_.contains(stageName)) {
		return stageDatas_[stageName];
	}

	//ないなら空白のステージを返す
	return {};
}

void StageManager::ReadStage(std::string filePath) {
	//読み込むJsonファイル
	std::ifstream file(filePath.c_str());
	if (!file.is_open()) {
		//エラー処理
		return;
	}

	nlohmann::json stagejson;
	file >> stagejson;
	file.close();

	//基本ステータス
	std::string stageName = stagejson["name"];
	stageDatas_[stageName].filePath = filePath;
	stageDatas_[stageName].bossData.filepath = stagejson["boss"]["filePath"];
	stageDatas_[stageName].bossData.spawnPosition = {
		stagejson["boss"]["spawnPosition"]["x"],
		stagejson["boss"]["spawnPosition"]["y"],
		stagejson["boss"]["spawnPosition"]["z"],
	};
	stageDatas_[stageName].playerSpawnPosition = {
		stagejson["playerSpawnPosition"]["x"],
		stagejson["playerSpawnPosition"]["y"],
		stagejson["playerSpawnPosition"]["z"],
	};
	nlohmann::json& objectsJson = stagejson["colliderObject"];

	//読み込んだパターン
	for (auto iterator = objectsJson.begin(); iterator != objectsJson.end(); ++iterator) {
		const std::string& name = iterator.key();

		ColliderObjectData data;
		data.directoryPath = objectsJson[name]["directoryPath"];
		data.filename = objectsJson[name]["filename"];
		data.transform.scale = {
			objectsJson[name]["transform"]["scale"]["x"],
			objectsJson[name]["transform"]["scale"]["y"],
			objectsJson[name]["transform"]["scale"]["z"],
		};
		data.transform.rotate = {
			objectsJson[name]["transform"]["rotate"]["x"],
			objectsJson[name]["transform"]["rotate"]["y"],
			objectsJson[name]["transform"]["rotate"]["z"],
		};
		data.transform.translate = {
			 objectsJson[name]["transform"]["translate"]["x"],
			 objectsJson[name]["transform"]["translate"]["y"],
			 objectsJson[name]["transform"]["translate"]["z"],
		};

		stageDatas_[stageName].colliderObjects.push_back(data);
	}
}

void StageManager::WriteStage(std::string stageName, StageData stageData) {
	//書き出すJsonファイル
	nlohmann::json stageJson;

	//基本ステータス
	stageJson["name"] = stageName;
	stageJson["boss"]["filePath"] = stageData.bossData.filepath;
	stageJson["boss"]["spawnPosition"]["x"] = stageData.bossData.spawnPosition.x;
	stageJson["boss"]["spawnPosition"]["y"] = stageData.bossData.spawnPosition.y;
	stageJson["boss"]["spawnPosition"]["z"] = stageData.bossData.spawnPosition.z;
	stageJson["playerSpawnPosition"]["x"] = stageData.playerSpawnPosition.x;
	stageJson["playerSpawnPosition"]["y"] = stageData.playerSpawnPosition.y;
	stageJson["playerSpawnPosition"]["z"] = stageData.playerSpawnPosition.z;

	int index = 0;
	//パターン出力
	for (auto& object : stageData.colliderObjects)
	{
		stageJson["colliderObject"][std::to_string(index)]["directoryPath"] = object.directoryPath;
		stageJson["colliderObject"][std::to_string(index)]["filename"] = object.filename;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["scale"]["x"] = object.transform.scale.x;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["scale"]["y"] = object.transform.scale.y;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["scale"]["z"] = object.transform.scale.z;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["rotate"]["x"] = object.transform.rotate.x;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["rotate"]["y"] = object.transform.rotate.y;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["rotate"]["z"] = object.transform.rotate.z;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["translate"]["x"] = object.transform.translate.x;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["translate"]["y"] = object.transform.translate.y;
		stageJson["colliderObject"][std::to_string(index)]["transform"]["translate"]["z"] = object.transform.translate.z;

		index++;
	}

	//ファイル書き出し部分
	std::ofstream file(stageData.filePath);
	file << stageJson.dump(4);
	file.close();
}