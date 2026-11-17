#pragma once
#include "GameCamera/GameCamera.h"
#include "Input/Input.h"

//個人設定
class Option {
private:
	//設定内容
	struct OptionState {
		//注目視点カメラオフセット
		Vector3 lockOnCameraBaseOffset;
		//注目視点肩越しずらし
		float lockOnCameraSideOffset;
		//カメラ切り替えを長押ししている間だけにする
		int isChangeCameraButtonHold;
		//メインとなるカメラ
		GameCamera::CameraType mainCameraType;
	};

	//適応されている設定
	OptionState state_;
	//変更中の設定
	OptionState changingState_;

	//カテゴリー
	enum class OptionCategory {
		Camera,
	};
	//選択中のカテゴリー
	OptionCategory selectedOptionCategory_;

	//設定項目
	enum class OptionItem {
		LockOnCameraBaseOffset,
		LockOnCameraSideOffset,
		IsChangeCameraButtonHold,
		MainCameraType,
	};
	//選択中の項目
	OptionItem selectedOptionItem_;

	std::weak_ptr<Input> input_;

public:
	//初期化
	void Initialize(std::weak_ptr<Input> input);
	//更新処理
	void Update();
	//描画処理
	void Draw();

	//書き込み
	void Save(std::string filePath);
	//読み込み
	void Load(std::string filePath);
	//デフォルト設定
	void Default();
	//カーソル移動
	void MoveCursor();
};