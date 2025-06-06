#pragma once
#include <GameEngine.h>

class SampleScene
{
private:
	// 3Dモデル
	Object_3D* object_ = nullptr;
	// 3Dモデルデータ
	Object_3D_Data objectData_;
	// テクスチャ
	Texture* texture_ = nullptr;

	// カメラ
	SRT cameraTransform_{};
	Camera* camera_;

	//デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	//デバッグカメラを使用するか
	bool isUseDebugCamera_ = false;

	//光源
	DirectionalLight directionalLight;
	Light* light_;

	//ゲームエンジン
	GameEngine* gameEngine_;

	//インプット
	Keybord keyBord_;
	Mouse mouse_;

public:
	//デストラクタ
	~SampleScene();
	//初期化
	void Initialize(GameEngine* gameEngine);
	//更新
	void Update();
	//描画
	void Draw();
};

