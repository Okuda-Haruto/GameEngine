#include "BossEditor.h"
#include <fstream>
#include <Operation/Operation.h>

void BossEditor::Initialize() {
	state_ = EditorState::None;


}

void BossEditor::Update() {
	
	Stage::Update();
}

void BossEditor::Draw() {

}

void BossEditor::ReadBossFile(std::string filePath) {

	//読み込むJsonファイル
	std::ifstream file(filePath.c_str());
	nlohmann::json bossJson;
	file >> bossJson;
	file.close();

	//基本ステータス
	bossName_ = bossJson["name"];
	directoryPath_ = bossJson["name"]["directoryPath"];
	modelname_ = bossJson["name"]["modelname"];
	maxHP_ = bossJson["name"]["maxHP"];



	nlohmann::json& patternJson = bossJson["name"]["pattern"];

	//読み込んだパターン
	for (auto iterator = patternJson.begin(); iterator != patternJson.end(); ++iterator) {
		const std::string& name = iterator.key();
		const auto& stepArray = iterator.value();

		auto pattern = std::make_unique<BossPattern>();
		auto action = std::make_unique<BossAction>();

		//Actionを複数にするときに変える

		//ステップ読み込み
		std::vector<std::unique_ptr<BaseStep>> steps;
		for (const auto& stepJson : stepArray)
		{
			steps.push_back(ReadStepJson(stepJson["name"]["pattern"][name]));
		}
		action->SetSteps(move(steps));

		pattern->SetAction(std::move(action));

		patterns_.emplace(name, std::move(pattern));
	}

	Stage::Initialize("BossEditor", input_);
}

void BossEditor::WriteBossFile(std::string filePath) {
	//書き出すJsonファイル
	nlohmann::json bossJson;

	//基本ステータス
	bossJson["name"] = bossName_.c_str();
	bossJson["name"]["directoryPath"] = directoryPath_.c_str();
	bossJson["name"]["modelname"] = modelname_.c_str();
	bossJson["name"]["maxHP"] = int(maxHP_);

	//パターン出力
	for (auto& pattern : patterns_)
	{
		BossAction* action = pattern.second->GetAction();

		//Actionを複数にするときに変える

		for (auto& step : action->GetSteps())
		{
			bossJson["name"]["pattern"][pattern.first].push_back(step->WriteStep());
		}
	}

	//ファイル書き出し部分
	std::ofstream file(filePath);
	file << bossJson.dump(4);
	file.close();
}