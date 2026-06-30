#include "BackGround.h"
#include "GameEngine.h"
#include <numbers>

void BackGround::Initialize(std::string filePath, std::shared_ptr<GameCamera> gameCamera, std::shared_ptr<DirectionalLight> directionalLight, std::shared_ptr<PointLight> pointLight) {
	directionalLight_ = directionalLight;
	pointLight_ = pointLight;
	gameCamera_ = gameCamera;

	LoadBackGroundObject(filePath);

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(gameCamera_->GetCamera());
	ground_ = std::make_unique<Ground>();
	ground_->Initialize();
	ground_->SetCamera(gameCamera_->GetCamera());
	ground_->SetDirectionalLight(directionalLight_);
	ground_->SetPointLight(pointLight_);
	tumbleweed_ = std::make_unique<Tumbleweed>();
	tumbleweed_->Initialize(gameCamera_->GetCamera(), directionalLight_, pointLight_);

	fence_ = std::make_unique<Fence>();
	fence_->Initialize(gameCamera_->GetCamera(), directionalLight_, pointLight_);
}

void BackGround::Update() {

	tumbleweed_->Update();
#ifdef USE_IMGUI
	ImGui::Begin("背景オブジェクト");
	static int cursor = 0;
	ImGui::SliderInt("オブジェクト番号", &cursor, 0, int(objects_.size() - 1));
	SRT transform = { {1,1,1},{0,0,0},{0,0,0} };
	if (ImGui::Button("追加")) {

		std::unique_ptr<Object> newObject = std::make_unique<Object>();
		newObject->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::BackGround));
		newObject->SetShininess(20);
		newObject->SetTransform(transform);
		newObject->SetDirectionalLight(directionalLight_);
		newObject->SetPointLight(pointLight_);
		objects_.push_back(move(newObject));

		cursor = int(objects_.size() - 1);
	}
	transform = objects_[cursor]->GetTransform();
	transform.rotate = transform.rotate / std::numbers::pi_v<float> *180;

	ImGui::SliderInt("使用するモデル番号", &indexes_[cursor], 0, 7);
	ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &transform.rotate.x, 1.0f);
	ImGui::DragFloat3("Translate", &transform.translate.x, 0.1f);

	transform.rotate = transform.rotate * std::numbers::pi_v<float> / 180;
	objects_[cursor]->SetTransform(transform);

	ImGui::End();
#endif
}

void BackGround::Draw() {
	skydome_->Draw();
	ground_->Draw();
	fence_->Draw();
	tumbleweed_->Draw();
	for (int i = 0; i < objects_.size();i++) {
		objects_[i]->Draw3D(indexes_[i]);
	}
}

void BackGround::LoadBackGroundObject(std::string filePath) {
	// ファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	std::stringstream LoadEnemyCommands_;
	LoadEnemyCommands_ << file.rdbuf();

	// ファイルを閉じる
	file.close();

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(LoadEnemyCommands_, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(line_stream, word, ':');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// 無視する
			continue;
		}

		// OBJECTコマンド
		if (word.find("OBJECT") == 0) {

			std::unique_ptr<Object> newObject = std::make_unique<Object>();
			SRT transform;

			//Index
			getline(line_stream, word, ',');
			int index = (uint32_t)std::atoi(word.c_str());

			while (line_stream && (line_stream.peek() == ',' || line_stream.peek() == '{')) { //','と'{'をスキップ
				line_stream.ignore();
			}

			//Scale
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());
			getline(line_stream, word, '}');
			float z = (float)std::atof(word.c_str());

			transform.scale = { x, y, z };

			while (line_stream && line_stream.peek() == '}') { //'}'をスキップ
				line_stream.ignore();
			}

			while (line_stream && (line_stream.peek() == ',' || line_stream.peek() == '{')) { //','と'{'をスキップ
				line_stream.ignore();
			}

			//Rotate
			getline(line_stream, word, ',');
			x = (float)std::atof(word.c_str());
			getline(line_stream, word, ',');
			y = (float)std::atof(word.c_str());
			getline(line_stream, word, '}');
			z = (float)std::atof(word.c_str());

			//わかりにくいので度数法で書くこと
			transform.rotate = { x * std::numbers::pi_v<float> / 180, y * std::numbers::pi_v<float> / 180, z * std::numbers::pi_v<float> / 180 };

			while (line_stream && line_stream.peek() == '}') { //'}'をスキップ
				line_stream.ignore();
			}

			while (line_stream && (line_stream.peek() == ',' || line_stream.peek() == '{')) { //','と'{'をスキップ
				line_stream.ignore();
			}

			//Translate
			getline(line_stream, word, ',');
			x = (float)std::atof(word.c_str());
			getline(line_stream, word, ',');
			y = (float)std::atof(word.c_str());
			getline(line_stream, word, '}');
			z = (float)std::atof(word.c_str());

			while (line_stream && line_stream.peek() == '}') { //'}'をスキップ
				line_stream.ignore();
			}

			transform.translate = { x, y, z };

			newObject->Initialize(ModelHolder::GetInstance()->GetModel(ModelIndex::BackGround));
			newObject->SetShininess(20);
			newObject->SetTransform(transform);
			newObject->SetDirectionalLight(directionalLight_);
			newObject->SetPointLight(pointLight_);

			objects_.push_back(move(newObject));
			indexes_.push_back(index);
		}
	}
}

void BackGround::SaveBackGround(std::string filePath) {
	// ファイルを開く
	std::ofstream file;
	file.open(filePath);	//上書きモード
	assert(file.is_open());

	file << "//コマンド:パーツ番号,{Scale},{Rotate},{Translate}\n\n";

	for (int i = 0; i < objects_.size(); i++) {

		SRT Transform = objects_[i]->GetTransform();

		file << "OBJECT:" << indexes_[i] <<
			",{" << Transform.scale.x << "," << Transform.scale.y << "," << Transform.scale.z <<
			"},{" << Transform.rotate.x / std::numbers::pi_v<float> *180 << "," << Transform.rotate.y / std::numbers::pi_v<float> *180 << "," << Transform.rotate.z / std::numbers::pi_v<float> *180 <<
			"},{" << Transform.translate.x << "," << Transform.translate.y << "," << Transform.translate.z << "},\n";

	}

	// ファイルを閉じる
	file.close();
}