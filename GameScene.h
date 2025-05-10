#pragma once
#include <GameEngine.h>
#include "Player.h"
#include "Skydome.h"
#include "MapChipField.h"
#include <vector>

//ゲームシーン
class GameScene {
private:
	// 3Dモデルデータ
	Object_3D* Object_Player_ = nullptr;
	Object_3D* Object_Block_ = nullptr;
	Object_3D* Object_Skydome_ = nullptr;
	// テクスチャデータ
	Texture* Texture_Player_ = nullptr;
	Texture* Texture_Block_ = nullptr;
	Texture* Texture_Skydome_ = nullptr;

	// カメラ
	SRT cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-50.0f} };
	Camera camera_;
	//デバッグカメラ有効
	bool isDebugCameraActive_ = false;
	//デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	//光源
	Light* light_;
	//自キャラ
	Player* player_ = nullptr;
	//天球
	Skydome* skydome_ = nullptr;
	//マップチップフィールド
	MapChipField* mapChipField_;
	//ブロックの可変個配列
	std::vector <std::vector<Object_3D_Data*>> worldTransformBlocks_;

	GameEngine* gameEngine_;

	Keybord keyBord_;
	Mouse mouse_;

public:
	//コンストラクタ
	GameScene(GameEngine* gameEngine);
	//デストラクタ
	~GameScene();
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw(GameEngine* gameEngine);
	//表示ブロック生成
	void GenerateBlocks();
};