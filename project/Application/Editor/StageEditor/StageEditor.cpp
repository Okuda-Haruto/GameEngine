#include "StageEditor.h"
#include <ModelManager/ModelManager.h>
#include <fstream>
#include <cassert>
#include <GameEngine.h>
#include <Operation/Operation.h>
#include <Collision.h>
#include <PrimitiveManager/PrimitiveManager.h>
#include <ImGuiManager/ImGuiManager.h>


StageEditor::MoveCommand::MoveCommand(StageEditor* editor, std::weak_ptr<SRT> targetTransform, SRT beforeTransform, SRT afterTransform) {
	editor_ = editor;
	targetTransform_ = targetTransform;
	beforeTransform_ = beforeTransform;
	afterTransform_ = afterTransform;
	
	//DoとRedoで行うことは同じ
	RedoCommand();
}

void StageEditor::MoveCommand::UndoCommand() {
	*targetTransform_.lock() = beforeTransform_;
}

void StageEditor::MoveCommand::RedoCommand() {
	*targetTransform_.lock() = afterTransform_;
}

StageEditor::AddColliderObjectCommand::AddColliderObjectCommand(StageEditor* editor, int32_t index, std::string directoryPath, std::string filename) {
	editor_ = editor;
	index_ = index;

	editor_->AddColliderObject(index, directoryPath, filename);

	//DoとRedoで行うことは同じ
	RedoCommand();
}

void StageEditor::AddColliderObjectCommand::UndoCommand() {
	editor_->SetEnableObject(index_, false);
}

void StageEditor::AddColliderObjectCommand::RedoCommand() {
	editor_->SetEnableObject(index_, true);
}

StageEditor::DeleteColliderObjectCommand::DeleteColliderObjectCommand(StageEditor* editor, int32_t index) {
	editor_ = editor;
	index_ = index;

	//DoとRedoで行うことは同じ
	RedoCommand();
}

void StageEditor::DeleteColliderObjectCommand::UndoCommand() {
	editor_->SetEnableObject(index_, true);
}

void StageEditor::DeleteColliderObjectCommand::RedoCommand() {
	editor_->SetEnableObject(index_, false);
}

void StageEditor::Initialize(std::shared_ptr<Input> input) {
	input_ = input;

	commandIndex_ = 0;

	bossData_.startTransform = std::make_shared<SRT>();
	*bossData_.startTransform = {
		{ 1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,0.0f },
	};

	playerStartTransform_ = std::make_shared<SRT>();
	*playerStartTransform_ = {
		{ 1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,0.0f,-20.0f },
	};

	debugCamera_ = std::make_shared<DebugCamera>();
	debugCamera_->Initialize(input_);

	state_ = EditorState::None;
	openFile_ = OpenFile::None;

	isEditingTransform_ = false;
}

void StageEditor::Update() {
	Mouse mouse = input_->GetMouse();
	Keyboard keyboard = input_->GetKeyboard();
#ifdef USE_IMGUI
	if (stage_) {
		stage_->Update();
		stage_->GetBoss()->SetTransfrom(*bossData_.startTransform);
		stage_->GetPlayer()->SetTransfrom(*playerStartTransform_);
		for (auto& colliderObject : colliderObjects_) {
			colliderObject.object->SetTransform(*colliderObject.startTransform);
		}
	}
	if (state_ == EditorState::None) {
		ImGui::Begin("ステージ選択");
		if (ImGui::Button("新規作成")) {
			openFile_ = OpenFile::CreateNewFile;
		}
		if (ImGui::Button("ステージファイルを読み込む")) {
			openFile_ = OpenFile::ReadFile;
		}
		if (ImGui::Button("前回開いたファイルを開く")) {
			openFile_ = OpenFile::ReadLastOpenFile;
		}
		ImGui::End();
	
		OpenFileWindow();
	} else if (state_ == EditorState::Edit) {
		ImGui::Begin("オブジェクト配置");
		//フォルダ名取得
		ImGuiFileTree_obj();
		ImGui::End();

		ImGui::Begin("ファイル操作");
		if (ImGui::Button("保存")) {
			WriteStageFile();
		}
		ImGui::End();

		//Undo,Redoコマンド
		if ((keyboard.keys[DIK_LCONTROL].hold || keyboard.keys[DIK_RCONTROL].hold) &&
			(keyboard.keys[DIK_LSHIFT].hold || keyboard.keys[DIK_RSHIFT].hold) &&
			keyboard.keys[DIK_Z].trigger) {
			
			if (commandIndex_ < commands_.size()) {
				commands_[commandIndex_]->RedoCommand();
				++commandIndex_;
			}

		}else if ((keyboard.keys[DIK_LCONTROL].hold || keyboard.keys[DIK_RCONTROL].hold) &&
			keyboard.keys[DIK_Z].trigger) {

			if (commandIndex_ > 0) {
				--commandIndex_;
				commands_[commandIndex_]->UndoCommand();
			}

		}



		//オブジェクト取得
		if (mouse.click[MOUSE_BOTTON_LEFT].trigger) {
			//マウスカーソルの半直線を取得
			Ray cursorRay = GetCursorRay(stage_->GetGameCamera(), input_);
			std::shared_ptr<SRT> nearTransform = GetObjectTransformFromRay(cursorRay);
			if (nearTransform) {
				nextTransform_ = nearTransform;
			}
			if (nextTransform_.lock()) {
				beforeTransform_ = *nextTransform_.lock();
			}
		} else if(mouse.click[MOUSE_BOTTON_LEFT].release) {
			//変化がないならスキップ
			if (nextTransform_.lock()) {
				if (std::abs(Length(beforeTransform_.scale - nextTransform_.lock()->scale)) > 1e-6f ||
					std::abs(Length(beforeTransform_.rotate - nextTransform_.lock()->rotate)) > 1e-6f ||
					std::abs(Length(beforeTransform_.translate - nextTransform_.lock()->translate)) > 1e-6f) {

					commands_.resize(commandIndex_);
					commands_.push_back(std::make_unique<MoveCommand>(this, nextTransform_, beforeTransform_, *nextTransform_.lock()));
					commandIndex_++;
				}
			}
		}

		if (nextTransform_.lock()) {

			//ImGuiの項目では書き換え後しか得られないため
			SRT before = *nextTransform_.lock();

			ImGui::Begin("transform");
			ImGui::InputFloat3("scale", &nextTransform_.lock()->scale.x);
			ImGui::InputFloat3("rotate", &nextTransform_.lock()->rotate.x);
			ImGui::InputFloat3("translate", &nextTransform_.lock()->translate.x);

			if (ImGui::IsItemActivated()) {
				beforeTransformInput_ = before;
			}

			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				//変化がないならスキップ
				if (std::abs(Length(beforeTransformInput_.scale - nextTransform_.lock()->scale)) > 1e-6f ||
					std::abs(Length(beforeTransformInput_.rotate - nextTransform_.lock()->rotate)) > 1e-6f ||
					std::abs(Length(beforeTransformInput_.translate - nextTransform_.lock()->translate)) > 1e-6f) {

					commands_.resize(commandIndex_);
					commands_.push_back(std::make_unique<MoveCommand>(this, nextTransform_, beforeTransformInput_, *nextTransform_.lock()));
					commandIndex_++;
					isEditingTransform_ = false;
				}
			}

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 16.0f);
			ImGui::Text("マニピュレーター変更");
			if (ImGui::Button("拡縮")) {
				GameEngine::GetImGuiManager()->SetGizmoOperation(ImGuizmo::SCALE);
			}
			if (ImGui::Button("回転")) {
				GameEngine::GetImGuiManager()->SetGizmoOperation(ImGuizmo::ROTATE);
			}
			if (ImGui::Button("位置")) {
				GameEngine::GetImGuiManager()->SetGizmoOperation(ImGuizmo::TRANSLATE);
			}
			ImGui::End();
		}

		AABB collisionAABB;
		//プレイヤー初期位置
		collisionAABB.min = { playerStartTransform_->translate - playerStartTransform_->scale };
		collisionAABB.max = { playerStartTransform_->translate + playerStartTransform_->scale };

		PrimitiveManager::GetInstance()->AddAABB(collisionAABB);

		//ボス初期位置
		collisionAABB.min = { bossData_.startTransform->translate - bossData_.startTransform->scale };
		collisionAABB.max = { bossData_.startTransform->translate + bossData_.startTransform->scale };

		PrimitiveManager::GetInstance()->AddAABB(collisionAABB);

		//オブジェクト初期位置
		for (auto& object : colliderObjects_) {
			if (object.enableObject) {
				collisionAABB.min = { object.startTransform->translate - object.startTransform->scale };
				collisionAABB.max = { object.startTransform->translate + object.startTransform->scale };

				PrimitiveManager::GetInstance()->AddAABB(collisionAABB);
			}
		}

		//選択している場合
		if (nextTransform_.lock()) {
			GameEngine::GetImGuiManager()->SetGuizmo(stage_->GetGameCamera()->GetCamera(), nextTransform_.lock().get());
		}
	}
#endif
}

void StageEditor::Draw() {
	if (stage_) {
		stage_->Draw();
	}
	for (auto& object : colliderObjects_) {
		if (object.enableObject) {
			object.object->Draw3D();
		}
	}
}

void StageEditor::ReadStageFile(std::string filePath) {
	filePath_ = filePath;
	stageData_ = StageManager::GetInstance()->ReadStage(filePath_);
	*bossData_.startTransform = stageData_.bossData.startTransform;
	*playerStartTransform_ = stageData_.playerStartTransform;

	stage_ = std::make_unique<Stage>();
	stage_->Initialize(stageData_, input_);
	stage_->SetDebugCamera(debugCamera_);
	stage_->ClearColliderObjects();

	for (auto& data : stageData_.colliderObjects) {
		EditorObject editorObject;
		editorObject.object = std::make_shared<Object>();
		editorObject.object->Initialize(ModelManager::GetInstance()->GetModel(data.directoryPath, data.filename));
		editorObject.object->SetDirectionalLight(stage_->GetDirectionalLight());
		editorObject.object->SetPointLight(stage_->GetPointLight());
		editorObject.startTransform = std::make_shared<SRT>();
		*editorObject.startTransform = data.startTransform;

		editorObject.directoryPath = data.directoryPath;
		editorObject.filename = data.filename;

		colliderObjects_.push_back(editorObject);

		currentColliderIndex_++;
	}

	state_ = EditorState::Edit;
}

void StageEditor::WriteStageFile() {
	//データ入力
	stageData_.bossData.filepath = bossData_.filePath;
	stageData_.bossData.startTransform = *bossData_.startTransform;

	stageData_.playerStartTransform = *playerStartTransform_;

	stageData_.colliderObjects.clear();
	for (auto& object : colliderObjects_) {
		//無効化されているオブジェクトは使用しない
		if (!object.enableObject)continue;

		//一旦衝突判定無し
		ColliderObjectData data;
		data.directoryPath = object.directoryPath;
		data.filename = object.filename;
		data.startTransform = *object.startTransform;
		stageData_.colliderObjects.push_back(data);
	}

	StageManager::GetInstance()->WriteStage(filePath_, stageData_);
}

void StageEditor::AddColliderObject(int32_t index, std::string directoryPath, std::string filename) {
	//新しく作った場合のみobjectを追加
	if (colliderObjects_.size() <= index) {
		EditorObject editorObject;
		editorObject.object = std::make_shared<Object>();
		editorObject.object->Initialize(ModelManager::GetInstance()->GetModel(directoryPath, filename));
		editorObject.object->SetDirectionalLight(stage_->GetDirectionalLight());
		editorObject.object->SetPointLight(stage_->GetPointLight());
		editorObject.startTransform = std::make_shared<SRT>();
		*editorObject.startTransform = {
			{1.0f,1.0f,1.0f},
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f,0.0f}
		};

		editorObject.directoryPath = directoryPath;
		editorObject.filename = filename;

		colliderObjects_.push_back(std::move(editorObject));
	}

}

void StageEditor::ChangeBoss(std::string name) {}

Ray StageEditor::GetCursorRay(std::shared_ptr<GameCamera> gameCamera, std::shared_ptr<Input> input) {
	Mouse mouse = input->GetMouse();
	Ray ray;

#ifdef USE_IMGUI
	//ドッキングウィンドウ用のスクリーンの座標に合わせる
	Matrix4x4 matViewport = MakeViewportMatrix(
		GameEngine::GetImGuiManager()->GetGameScreenPosition().x,
		GameEngine::GetImGuiManager()->GetGameScreenPosition().y,
		GameEngine::GetImGuiManager()->GetGameScreenSize().x,
		GameEngine::GetImGuiManager()->GetGameScreenSize().y,
		0,
		1
	);

	Matrix4x4 matViewProjectionViewport =
		gameCamera->GetCamera()->GetViewMatrix() *
		gameCamera->GetCamera()->GetProjectionMatrix() *
		matViewport;

	Matrix4x4 matInverseVPV = Inverse(matViewProjectionViewport);

	Vector3 posNear = Vector3(mouse.Position.x, mouse.Position.y, 0.0f);
	Vector3 posFar = Vector3(mouse.Position.x, mouse.Position.y, 1.0f);

	posNear = Transform(posNear, matInverseVPV);
	posFar = Transform(posFar, matInverseVPV);

	Vector3 mouseDirection = posFar - posNear;
	mouseDirection = Normalize(mouseDirection);

	Matrix4x4 inverseViewMatrix = Inverse(gameCamera->GetCamera()->GetViewMatrix());
	Vector3 translate = Vector3{ inverseViewMatrix.m[3][0], inverseViewMatrix.m[3][1], inverseViewMatrix.m[3][2] };

	Vector3 rayDir = posNear + mouseDirection - translate;

	ray = {
		translate,
		rayDir
	};
#endif

	return ray;
}

std::shared_ptr<SRT> StageEditor::GetObjectTransformFromRay(Ray ray) {
	std::shared_ptr<SRT> result = nullptr;

	//Transformだけではどれを選択してるかわからないのでindexを得る
	cursorObjectIndex_ = -1;

	AABB collisionAABB;
	//プレイヤー初期位置
	collisionAABB.min = { playerStartTransform_->translate - playerStartTransform_->scale };
	collisionAABB.max = { playerStartTransform_->translate + playerStartTransform_->scale };

	if (IsCollision(collisionAABB, ray)) {
		result = playerStartTransform_;
	}
	//ボス初期位置
	collisionAABB.min = { bossData_.startTransform->translate - bossData_.startTransform->scale };
	collisionAABB.max = { bossData_.startTransform->translate + bossData_.startTransform->scale };

	if (IsCollision(collisionAABB, ray)) {
		if (!result) {
			result = bossData_.startTransform;
		} else if (Length(result->translate - ray.origin) <=
			Length(bossData_.startTransform->translate - ray.origin)) {

			result = bossData_.startTransform;
		}
	}
	//オブジェクト初期位置
	for (int32_t index = 0; index < colliderObjects_.size();index++) {
		collisionAABB.min = { colliderObjects_[index].startTransform->translate - colliderObjects_[index].startTransform->scale};
		collisionAABB.max = { colliderObjects_[index].startTransform->translate + colliderObjects_[index].startTransform->scale };

		if (IsCollision(collisionAABB, ray)) {
			if (!result) {
				result = colliderObjects_[index].startTransform;
				cursorObjectIndex_ = index;
			} else if (Length(result->translate - ray.origin) <=
				Length(colliderObjects_[index].startTransform->translate - ray.origin)) {

				result = colliderObjects_[index].startTransform;
				cursorObjectIndex_ = index;
			}
		}
	}

	return result;
}

void StageEditor::OpenFileWindow() {
#ifdef USE_IMGUI
	switch (openFile_)
	{
	case StageEditor::OpenFile::CreateNewFile:
		ImGui::Begin("ステージファイルを新規作成");

		ImGui::InputText("ファイルパス", filePathText_, sizeof(filePathText_));
		ImGui::Text("フォルダ位置参照");
		ImGuiFolderTree();

		if (ImGui::Button("作成")) {
			stageData_ = {};
			//ImGuiに直接stringを入れられないのでコピー
			filePath_ = filePathText_;

			StageManager::GetInstance()->WriteStage(filePath_, stageData_);

			ReadStageFile(filePath_);
			SaveLastOpenFilePath(filePath_);
		}

		ImGui::SameLine();
		if (ImGui::Button("戻る")) {
			openFile_ = OpenFile::None;
		}
		ImGui::End();
		break;
	case StageEditor::OpenFile::ReadFile:
		ImGui::Begin("既存のファイルを読み込む");

		ImGui::InputText("ファイルパス", filePathText_, sizeof(filePathText_));
		ImGui::Text("jsonファイル参照");
		ImGuiFileTree_json();

		if (ImGui::Button("読み込み")) {
			stageData_ = {};
			//ImGuiに直接stringを入れられないのでコピー
			filePath_ = filePathText_;

			ReadStageFile(filePath_);
			SaveLastOpenFilePath(filePath_);
		}

		ImGui::SameLine();
		if (ImGui::Button("戻る")) {
			openFile_ = OpenFile::None;
		}
		ImGui::End();
		break;
	case StageEditor::OpenFile::ReadLastOpenFile:

		stageData_ = {};
		//ImGuiに直接stringを入れられないのでコピー
		filePath_ = LoadLastOpenFilePath();

		ReadStageFile(filePath_);

		break;
	default:
		break;
	}
#endif
}

void StageEditor::SaveLastOpenFilePath(std::string lastOpenFilePath) {
	//書き出すJsonファイル
	nlohmann::json filePathJson;

	filePathJson["LastOpenFilePath"] = lastOpenFilePath;

	//ファイル書き出し部分
	std::ofstream file("resources/Data/Stage/LastOpenFile.json");
	file << filePathJson.dump(4);
	file.close();
}

std::string StageEditor::LoadLastOpenFilePath() {
	//読み込むJsonファイル
	std::ifstream file("resources/Data/Stage/LastOpenFile.json");
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

void StageEditor::ImGuiFileTree_obj(std::string path, std::string name) {
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
						commands_.push_back(std::make_unique<AddColliderObjectCommand>(this, currentColliderIndex_, path, entry.path().filename().string()));	//パスとファイル名が必要なのでこうするしかない
						commandIndex_++;
						currentColliderIndex_++;
					}
				}
			}
		}
		ImGui::TreePop();
	}
#endif
}
void StageEditor::ImGuiFileTree_json(std::string path, std::string name) {
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

void StageEditor::ImGuiFolderTree(std::string path, std::string name) {
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