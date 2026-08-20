#include "BossEditor.h"
#include <fstream>
#include <Operation/Operation.h>
#include <ModelManager/ModelManager.h>
#include <ImGuiManager/ImGuiManager.h>

#include <format>
#include <Windows.h>

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

void BossEditor::Initialize(std::shared_ptr<Input> input) {
	input_ = input;

	debugCamera_ = std::make_shared<DebugCamera>();
	debugCamera_->Initialize(input_);

	stepNameList_ = GetStepNameList();

	state_ = EditorState::None;
	openFile_ = OpenFile::None;
	addPattern_ = AddPattern::None;
}

void BossEditor::Update() {

	if (stage_) {
		stage_->Update();
	}

#ifdef USE_IMGUI
	if (state_ == EditorState::None) {
		ImGui::Begin("ファイル選択");
		if (ImGui::Button("新規作成")) {
			openFile_ = OpenFile::CreateNewFile;
		}
		if (ImGui::Button("ファイルを読み込む")) {
			openFile_ = OpenFile::ReadFile;
		}
		if (ImGui::Button("前回開いたファイルを開く")) {
			openFile_ = OpenFile::ReadLastOpenFile;
		}
		ImGui::End();

		OpenFileWindow();
	} else if (state_ == EditorState::Edit) {
		ImGui::Begin("ボス基本ステータス");
		ImGui::Text("Model");
		ImGui::InputText("ディレクトリパス", directoryPathText_, sizeof(directoryPathText_));
		ImGui::InputText("ファイル名", modelnameText_, sizeof(modelnameText_));

		ImGui::Text("パターン");
		for (const auto& [str, pattern] : patterns_) {

			//開閉どちらでもドロップできるように
			bool originNode = ImGui::TreeNode(str.c_str());

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload =
					ImGui::AcceptDragDropPayload("PATTERN"))
				{
					const char* name = static_cast<const char*>(payload->Data);

					pattern->GetAction()->PushBackStep(GetStep(name));
				}

				ImGui::EndDragDropTarget();
			}

			if (originNode) {

				for (auto& step : pattern->GetAction()->GetSteps()) {
					if (ImGui::TreeNode(step->GetName().c_str())) {

						step->EditorItem();

						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}
		}
		if (ImGui::Button("パターン追加")) {
			addPattern_ = AddPattern::AddPattern;
		}

		ImGui::End();


		ImGui::Begin("パターン");


		for (auto stepName : stepNameList_) {
			ImGui::Button(stepName.c_str());

			if (ImGui::BeginDragDropSource())
			{
				std::string str = stepName;

				ImGui::SetDragDropPayload("PATTERN", str.c_str(), str.size() + 1);

				ImGui::Text(str.c_str());

				ImGui::EndDragDropSource();
			}
		}

		ImGui::End();
	}

	if (addPattern_ == AddPattern::AddPattern) {
		ImGui::Begin("パターン追加");
		ImGui::InputText("パターン名", patternNameText_, sizeof(patternNameText_));
		if (ImGui::Button("追加")) {
			std::string str = patternNameText_;
			patterns_[str] = std::make_unique<BossPattern>();
			patterns_[str]->SetAction(std::make_unique<BossAction>());
			addPattern_ = AddPattern::None;
		}
		ImGui::End();
	}
#endif


#ifdef USE_IMGUI
		ImGui::Begin("test");

		if (ImGui::Button("Apple"))
		{
			// 普通のボタン
		}

		if (ImGui::BeginDragDropSource())
		{
			int id = 123;

			ImGui::SetDragDropPayload("ITEM", &id, sizeof(id));

			ImGui::Text("Apple");

			ImGui::EndDragDropSource();
		}

		ImGui::Button("Box");

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload =
				ImGui::AcceptDragDropPayload("ITEM"))
			{
				int id = *(int*)payload->Data;

				std::wstring msg = std::format(L"ID = {}\n", id);
				OutputDebugStringW(msg.c_str());
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::End();
#endif
}

void BossEditor::Draw() {
	if (stage_) {
		stage_->Draw();
	}
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
	directoryPath_ = bossJson["directoryPath"];
	modelname_ = bossJson["modelname"];
	maxHP_ = bossJson["maxHP"];



	nlohmann::json& patternJson = bossJson["pattern"];

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
			steps.push_back(ReadStepJson(stepJson["pattern"][name]));
		}
		action->SetSteps(move(steps));

		pattern->SetAction(std::move(action));

		patterns_.emplace(name, std::move(pattern));
	}
	StageData stageData{};
	stageData.bossData.startTransform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	stageData.playerStartTransform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	stageData.bossData.filepath = filePath_;

	stage_ = std::make_unique<Stage>();
	stage_->Initialize(false, stageData, input_);
	stage_->SetDebugCamera(debugCamera_);
	state_ = EditorState::Edit;
}

void BossEditor::WriteBossFile() {
	//書き出すJsonファイル
	nlohmann::json bossJson;

	//基本ステータス
	bossJson["name"] = bossName_.c_str();
	bossJson["directoryPath"] = directoryPath_.c_str();
	bossJson["modelname"] = modelname_.c_str();
	bossJson["maxHP"] = int(maxHP_);

	//パターン出力
	for (auto& pattern : patterns_)
	{
		BossAction* action = pattern.second->GetAction();

		//Actionを複数にするときに変える

		for (auto& step : action->GetSteps())
		{
			bossJson["pattern"][pattern.first].push_back(step->WriteStep());
		}
	}

	std::filesystem::path path(filePath_);

	// 親フォルダを作成（既に存在していてもOK）
	std::filesystem::create_directories(path.parent_path());


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

void BossEditor::OpenFileWindow() {
#ifdef USE_IMGUI
	std::string path;

	switch (openFile_)
	{
	case BossEditor::OpenFile::CreateNewFile:
		ImGui::Begin("ファイルを新規作成");

		ImGui::InputText("ファイルパス", filePathText_, sizeof(filePathText_));
		ImGui::Text("フォルダ位置参照");
		if (ImGuiFolderTree(path)) {
			strncpy_s(filePathText_, path.c_str(), sizeof(filePathText_) - 1);
			filePathText_[sizeof(filePathText_) - 1] = '\0';
		}

		if (ImGui::Button("作成")) {
			stageData_ = {};
			//ImGuiに直接stringを入れられないのでコピー
			filePath_ = filePathText_;

			WriteBossFile();

			ReadBossFile(filePath_);
			SaveLastOpenFilePath(filePath_);
		}

		ImGui::SameLine();
		if (ImGui::Button("戻る")) {
			openFile_ = OpenFile::None;
		}
		ImGui::End();
		break;
	case BossEditor::OpenFile::ReadFile:
		ImGui::Begin("既存のファイルを読み込む");

		ImGui::InputText("ファイルパス", filePathText_, sizeof(filePathText_));
		ImGui::Text("jsonファイル参照");

		if (ImGuiFileTree_json(path)) {
			strncpy_s(filePathText_, path.c_str(), sizeof(filePathText_) - 1);
			filePathText_[sizeof(filePathText_) - 1] = '\0';
		}

		if (ImGui::Button("読み込み")) {
			stageData_ = {};
			//ImGuiに直接stringを入れられないのでコピー
			filePath_ = filePathText_;

			ReadBossFile(filePath_);
			SaveLastOpenFilePath(filePath_);
		}

		ImGui::SameLine();
		if (ImGui::Button("戻る")) {
			openFile_ = OpenFile::None;
		}
		ImGui::End();
		break;
	case BossEditor::OpenFile::ReadLastOpenFile:

		stageData_ = {};
		//ImGuiに直接stringを入れられないのでコピー
		filePath_ = LoadLastOpenFilePath();

		ReadBossFile(filePath_);

		break;
	default:
		break;
	}
#endif
}

void BossEditor::SaveLastOpenFilePath(std::string lastOpenFilePath) {
	//書き出すJsonファイル
	nlohmann::json filePathJson;

	filePathJson["LastOpenFilePath"] = lastOpenFilePath;

	//ファイル書き出し部分
	std::ofstream file("resources/Data/Boss/LastOpenFile.json");
	file << filePathJson.dump(4);
	file.close();
}

std::string BossEditor::LoadLastOpenFilePath() {
	//読み込むJsonファイル
	std::ifstream file("resources/Data/Boss/LastOpenFile.json");
	if (!file.is_open()) {
		//エラー処理
		return {};
	}

	nlohmann::json filePathJson;
	file >> filePathJson;
	file.close();

	//基本ステータス
	std::string lastOpenFilePath = filePathJson["LastOpenFilePath"];

	return lastOpenFilePath;
}

bool BossEditor::ImGuiFileTree_obj(std::string& path, std::string name) {
#ifdef USE_IMGUI
	bool isClicked = false;

	if (ImGui::TreeNode(name.c_str())) {
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
			{
				path = path + +"/" + entry.path().filename().string();
				isClicked = ImGuiFileTree_obj(path, entry.path().filename().string());
			} else if (entry.is_regular_file()) {
				//objファイル
				if (entry.path().extension() == ".obj")
				{
					if (ImGui::Button(entry.path().filename().string().c_str())) {
						commands_.resize(commandIndex_);
						commands_.push_back(std::make_unique<SetModelCommand>(this, directoryPath_, modelname_, path, entry.path().filename().string()));	//パスとファイル名が必要なのでこうするしかない
						commandIndex_++;

						return true;
					}
				}
			}
		}
		ImGui::TreePop();
	}

	return isClicked;
#endif
}
bool BossEditor::ImGuiFileTree_json(std::string& filePath, std::string path, std::string name) {
#ifdef USE_IMGUI
	bool isClicked = false;

	if (ImGui::TreeNode(name.c_str())) {
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
			{
				isClicked = ImGuiFileTree_json(filePath, path + "/" + entry.path().filename().string(), entry.path().filename().string());
			} else if (entry.is_regular_file()) {
				//objファイル
				if (entry.path().extension() == ".json")
				{
					if (ImGui::Button(entry.path().filename().string().c_str())) {

						filePath = path + "/" + entry.path().filename().string();
						return true;
					}
				}
			}
		}
		ImGui::TreePop();
	}

	return isClicked;
#endif
}

bool BossEditor::ImGuiFolderTree(std::string& filePath, std::string path, std::string name) {
#ifdef USE_IMGUI
	bool isClicked = false;
	if (ImGui::TreeNode(name.c_str())) {

		if (ImGui::IsItemClicked())
		{
			filePath = path;
			return true;
		}

		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
			{
				isClicked = ImGuiFolderTree(
					filePath,
					path + "/" + entry.path().filename().string(),
					entry.path().filename().string());
			}
		}

		ImGui::TreePop();
	}

	return isClicked;
#endif
}