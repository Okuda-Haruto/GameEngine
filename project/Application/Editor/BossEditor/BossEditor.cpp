#include "BossEditor.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <Operation/Operation.h>

void BossEditor::Initialize() {

}

void BossEditor::Update() {

}

void BossEditor::Draw() {

}

void BossEditor::ReadBossFile(std::string filePath) {

}

void BossEditor::WriteBossFile(std::string filePath) {
	//書き出すJsonファイル
	nlohmann::json bossJson;

	//基本ステータス
	bossJson["name"] = bossName_.c_str();
	bossJson["name"]["model"] = modelPath_.c_str();
	bossJson["name"]["maxHP"] = int(maxHP_);



	//ファイル書き出し部分
	std::ofstream file(filePath);
	file << bossJson.dump(4);
	file.close();
}