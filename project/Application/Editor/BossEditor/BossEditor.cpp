#include "BossEditor.h"
#include <fstream>
#include <Operation/Operation.h>
#include <ModelManager/ModelManager.h>
#include <ImGuiManager/ImGuiManager.h>

BossEditor::SetModelCommand::SetModelCommand(BossEditor* editor, std::string beforeDirectoryPath, std::string beforeFileName, std::string afterDirectoryPath, std::string afterFileName) {
	editor_ = editor;
	beforeDirectoryPath_ = beforeDirectoryPath;
	beforeFileName_ = beforeFileName;
	afterDirectoryPath_ = afterDirectoryPath;
	afterFileName_ = afterFileName;

	//DoとRedoで行うことは同じ
	RedoCommand();
}

void BossEditor::SetModelCommand::UndoCommand() {
	editor_->SetBossData(beforeDirectoryPath_, beforeFileName_);
}

void BossEditor::SetModelCommand::RedoCommand() {
	editor_->SetBossData(afterDirectoryPath_, afterFileName_);
}

void BossEditor::Initialize() {
	state_ = EditorState::None;


}

void BossEditor::Update() {
#ifdef USE_IMGUI
	for (auto& pattern : patterns_) {
		BossAction* action = pattern.second->GetAction();

		ImGui::Begin(pattern.first.c_str());
		std::vector<BaseStep*> steps = action->GetSteps();

		for (int i = 0; i < steps.size(); i++)
		{
			auto& step = steps[i];

			ImGui::Selectable(step->GetName().c_str());

			// ドラッグ開始
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("STEP_INDEX", &i, sizeof(i));
				ImGui::Text("%s", step->GetName().c_str());
				ImGui::EndDragDropSource();
			}

			// ドロップ先
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload =
					ImGui::AcceptDragDropPayload("STEP_INDEX"))
				{
					int from = *(int*)payload->Data;
					int to = i;

					auto step = std::move(steps[from]);
					steps.erase(steps.begin() + from);
					steps.insert(steps.begin() + to, std::move(step));
				}

				ImGui::EndDragDropTarget();
			}
		}

		ImGui::End();
	}
#endif
	stage_->Update();
}

void BossEditor::Draw() {

}

void BossEditor::ReadBossFile(std::string filePath) {

	filePath_ = filePath;

	//読み込むJsonファイル
	std::ifstream file(filePath_.c_str());
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
	//StageEditor優先
	stage_->Initialize({}, input_);
}

void BossEditor::WriteBossFile() {
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
	std::ofstream file(filePath_);
	file << bossJson.dump(4);
	file.close();
}

void BossEditor::SetBossData(std::string directoryPath, std::string modelname) {
	directoryPath_ = directoryPath;
	modelname_ = modelname;

	//ボスに入れる
	stage_->GetBoss()->SetModel(ModelManager::GetInstance()->GetModel(directoryPath_, modelname_));
}

void BossEditor::ImGuiFileTree_obj(std::string path, std::string name) {
#ifdef USE_IMGUI
	if (ImGui::TreeNode(name.c_str())) {
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
			{
				ImGuiFileTree_obj(path + "/" + entry.path().filename().string(), entry.path().filename().string());
			} else if (entry.is_regular_file()) {
				//objファイル
				if (entry.path().extension() == ".obj")
				{
					if (ImGui::Button(entry.path().filename().string().c_str())) {
						commands_.resize(commandIndex_);
						commands_.push_back(std::make_unique<SetModelCommand>(this, directoryPath_, modelname_, path, entry.path().filename().string()));	//パスとファイル名が必要なのでこうするしかない
						commandIndex_++;
					}
				}
			}
		}
		ImGui::TreePop();
	}
#endif
}
void BossEditor::ImGuiFileTree_json(std::string path, std::string name) {
#ifdef USE_IMGUI
	if (ImGui::TreeNode(name.c_str())) {
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
			{
				ImGuiFileTree_json(path + "/" + entry.path().filename().string(), entry.path().filename().string());
			} else if (entry.is_regular_file()) {
				//objファイル
				if (entry.path().extension() == ".json")
				{
					if (ImGui::Button(entry.path().filename().string().c_str())) {

						std::string jsonPath;
						jsonPath = path + "/" + entry.path().filename().string();

						strncpy_s(filePathText_, jsonPath.c_str(), sizeof(filePathText_) - 1);
						filePathText_[sizeof(filePathText_) - 1] = '\0';
					}
				}
			}
		}
		ImGui::TreePop();
	}
#endif
}

void BossEditor::ImGuiFolderTree(std::string path, std::string name) {
#ifdef USE_IMGUI
	if (ImGui::TreeNode(name.c_str())) {

		if (ImGui::IsItemClicked())
		{
			strncpy_s(filePathText_, path.c_str(), sizeof(filePathText_) - 1);
			filePathText_[sizeof(filePathText_) - 1] = '\0';
		}

		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
			{
				ImGuiFolderTree(
					path + "/" + entry.path().filename().string(),
					entry.path().filename().string());
			}
		}

		ImGui::TreePop();
	}
#endif
}