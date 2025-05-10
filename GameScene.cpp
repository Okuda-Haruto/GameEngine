#include "GameScene.h"
#include "Matrix4x4_operation.h"

GameScene::GameScene(GameEngine* gameEngine) {
	gameEngine_ = gameEngine;
}

GameScene::~GameScene() {
	delete debugCamera_;
	delete Object_Player_;
	delete Object_Block_;
	delete Object_Skydome_;
	delete Texture_Player_;
	delete Texture_Block_;
	delete Texture_Skydome_;
	delete player_;
	delete mapChipField_;
	for (std::vector<Object_3D_Data*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (Object_3D_Data* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

void GameScene::Initialize() {
	//3Dモデルの生成
	Object_Player_ = new Object_3D;
	gameEngine_->LoadObject(Object_Player_,"resources/player","player.obj");
	Object_Block_ = new Object_3D;
	gameEngine_->LoadObject(Object_Block_, "resources/box", "box.obj");
	Object_Skydome_ = new Object_3D;
	gameEngine_->LoadObject(Object_Skydome_, "resources/skydome", "skydome.obj");

	//ファイル名を指定してテクスチャを読み込む
	Texture_Player_ = new Texture;
	gameEngine_->LoadTexture(Texture_Player_, (Object_Player_->ModelData()).material.textureFilePath);
	Texture_Block_ = new Texture;
	gameEngine_->LoadTexture(Texture_Block_, (Object_Block_->ModelData()).material.textureFilePath);
	Texture_Skydome_ = new Texture;
	gameEngine_->LoadTexture(Texture_Skydome_, (Object_Skydome_->ModelData()).material.textureFilePath);

	//カメラの初期化
	camera_ = {
		MakeIdentity4x4(),
		MakeIdentity4x4()
	};
	camera_ = gameEngine_->UpdateCamera(cameraTransform.rotate, cameraTransform.translate);
	// デバッグカメラの生成
	debugCamera_ = new DebugCamera();
	debugCamera_->Initialize();

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	//光源
	light_ = new Light();
	gameEngine_->LoadLight(light_);

	Vector4 lightColor = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 lightDirection = { 0.0f,-1.0f,0.0f };
	float lightIntensity = 1.0f;
	light_->SetColor(lightColor);
	light_->SetDirection(lightDirection);
	light_->SetIntensity(lightIntensity);

	//自キャラの生成
	player_ = new Player();

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, mapChipField_->GetNumBlockVirtical() - 2);
	//自キャラの初期化
	player_->Initialize(Object_Player_, Texture_Player_,light_, playerPosition);
	//天球の生成
	skydome_ = new Skydome();
	//天球の初期化
	skydome_->Initialize(Object_Skydome_, Texture_Skydome_);

	//表示ブロックの生成
	GenerateBlocks();
}

void GameScene::Update() {

	keyBord_ = gameEngine_->GetKeybord();
	mouse_ = gameEngine_->GetMouse();

#ifdef _DEBUG
	if (keyBord_.hit[DIK_SPACE]) {
		isDebugCameraActive_ = true;
	}
#endif
	//カメラの処理
	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		debugCamera_->Update(mouse_);
		camera_ = debugCamera_->GetTransform();
	} else {
		camera_ = gameEngine_->UpdateCamera(cameraTransform.rotate, cameraTransform.translate);
	}
	//自キャラの更新
	player_->Update(keyBord_);
	// 天球の更新
	skydome_->Update();
}

void GameScene::Draw(GameEngine* gameEngine) {

	gameEngine->PreDraw();

	gameEngine_ = gameEngine;

	//自キャラの描画
	player_->Draw(gameEngine_->GetCommandList(),camera_);
	//天球の描画
	skydome_->Draw(gameEngine_->GetCommandList(),camera_);

	
	Object_Block_->SetLight(light_);
	Object_Block_->SetTexture(Texture_Block_);

	//ブロックの描画
	for (std::vector<Object_3D_Data*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (Object_3D_Data* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			worldTransformBlock->camera = camera_;
			Object_Block_->Draw(gameEngine_->GetCommandList(), *(worldTransformBlock));
		}
	}

	gameEngine->PostDraw();

	Object_Block_->Reset();
	player_->Reset();
	skydome_->Reset();

}

void GameScene::GenerateBlocks() {
	// 要素数
	const uint32_t kNumBlockVirtical = mapChipField_->GetNumBlockVirtical();
	const uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	worldTransformBlocks_.resize(kNumBlockVirtical);
	// 列数の設定
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		//1列の要素数を設定
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// 要素数を変更する
	worldTransformBlocks_.resize(kNumBlockHorizontal);
	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				Object_3D_Data* worldTransform = new Object_3D_Data();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->transform.scale = { 1.0f,1.0f,1.0f };
				worldTransformBlocks_[i][j]->transform.translate = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}