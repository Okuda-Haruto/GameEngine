#include "StageManager.h"
#include <fstream>
#include <JsonUtils.h>

std::unique_ptr<StageManager> StageManager::instance;

StageManager* StageManager::GetInstance() {
	if (!instance) {
		instance = std::make_unique<StageManager>();
	}
	return instance.get();
}

void StageManager::Finalize() {

	instance.reset();
}

StageData StageManager::ReadStage(std::string filePath) {
	StageData stageData{};

	//読み込むJsonファイル
	std::ifstream file(filePath.c_str());
	if (!file.is_open()) {
		//エラー処理
		return stageData;
	}

	nlohmann::json stagejson;
	file >> stagejson;
	file.close();

	//基本ステータス
	stageData.bossData.filepath = stagejson["boss"]["filePath"];
	stageData.bossData.startTransform = stagejson["boss"]["startTransform"].get<SRT>();
	stageData.playerStartTransform = stagejson["playerStartTransform"].get<SRT>();
	nlohmann::json& objectsJson = stagejson["colliderObject"];

	//読み込んだオブジェクト
	for (auto iterator = objectsJson.begin(); iterator != objectsJson.end(); ++iterator) {
		const std::string& name = iterator.key();

		ColliderObjectData data;
		data.directoryPath = objectsJson[name]["directoryPath"];
		data.filename = objectsJson[name]["filename"];
		data.startTransform = stagejson[name]["transform"].get<SRT>();
			
		stageData.colliderObjects.push_back(data);
	}

	return stageData;
}

void StageManager::WriteStage(std::string filePath, StageData stageData) {
	//書き出すJsonファイル
	nlohmann::json stageJson;

	//基本ステータス
	stageJson["boss"]["filePath"] = stageData.bossData.filepath;
	stageJson["boss"]["startTransform"] = stageData.bossData.startTransform;
	stageJson["playerStartTransform"] = stageData.playerStartTransform;

	int index = 0;
	//オブジェクト出力
	for (auto& object : stageData.colliderObjects)
	{
		stageJson["colliderObject"][std::to_string(index)]["directoryPath"] = object.directoryPath;
		stageJson["colliderObject"][std::to_string(index)]["filename"] = object.filename;
		stageJson["colliderObject"][std::to_string(index)]["startTransform"] = object.startTransform;

		index++;
	}

	//ファイル書き出し部分
	std::ofstream file(filePath);
	file << stageJson.dump(4);
	file.close();
}