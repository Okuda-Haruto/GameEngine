#pragma once
#include <GameEngine.h>
#include <Line.h>
#include <array>

class SampleScene
{
private:
	// 3Dモデル
	std::array<bool, 7>isObjectDraw_{ TRUE ,TRUE ,TRUE ,TRUE ,TRUE ,TRUE ,TRUE };
	std::array<Object_3D*, 7> object_{ nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr };
	// 3Dモデルデータ
	std::array<Object_Multi_Data, 7> objectData_;

	//2Dモデル
	bool isSpriteDraw_ = TRUE;
	Sprite_2D* sprite_ = nullptr;
	// 2Dモデルデータ
	Object_Single_Data spriteData_;

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
	UINT isLighting_ = 1;

	//インプット
	Keybord keyBord_;
	Mouse mouse_;
	Pad pad_;

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

