#pragma once
#include <GameEngine.h>
#include <Line.h>

class SampleScene
{
private:
	// 3Dモデル
	Object_3D* object_ = nullptr;
	// 3Dモデルデータ
	Object_Multi_Data objectData_;

	Grid* grid_ = nullptr;

	bool isDrawXY_ = false;
	bool isDrawXZ_ = true;
	bool isDrawYZ_ = false;

	Audio* audio_ = nullptr;

	// カメラ
	SRT cameraTransform_{};
	Camera* camera_;

	AxisIndicator* axis_ = nullptr;

	//デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	//デバッグカメラを使用するか
	bool isUseDebugCamera_ = true;

	//光源
	DirectionalLight directionalLight;
	Light* light_;

	//インプット
	Keybord keyBord_;
	Mouse mouse_;

public:
	//デストラクタ
	~SampleScene();
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();
};

