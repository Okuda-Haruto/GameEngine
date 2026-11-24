#include "Option.h"
#include "JsonUtils.h"

void Option::Initialize(std::weak_ptr<Input> input) {
	input_ = input;

	selectedOptionCategory_ = OptionCategory::Camera;
	selectedOptionItem_ = OptionItem::lockOnCameraBaseOffsetY;

	keyHoldTime_ = 0;
}

void Option::Update() {
	Pad pad = input_.lock()->GetPad();
	Keyboard keyboard = input_.lock()->GetKeyboard();

	if (pad.Button[PAD_BUTTON_UP].hold || pad.Button[PAD_BUTTON_DOWN].hold || pad.Button[PAD_BUTTON_RIGHT].hold || pad.Button[PAD_BUTTON_LEFT].hold ||
		keyboard.keys[DIK_UP].hold || keyboard.keys[DIK_DOWN].hold || keyboard.keys[DIK_RIGHT].hold || keyboard.keys[DIK_LEFT].hold ||
		keyboard.keys[DIK_W].hold || keyboard.keys[DIK_S].hold || keyboard.keys[DIK_D].hold || keyboard.keys[DIK_A].hold
		) {
		++keyHoldTime_;
	} else {
		keyHoldTime_ = 0;
	}

	//カテゴリー別でカーソル移動
	switch (selectedOptionCategory_)
	{
	case Option::OptionCategory::Camera:	//カメラカテゴリー
		if (pad.Button[PAD_BUTTON_UP].trigger || (pad.Button[PAD_BUTTON_UP].hold && keyHoldTime_ > 60) ||
			keyboard.keys[DIK_UP].trigger || (keyboard.keys[DIK_UP].hold && keyHoldTime_ > 60) || 
			keyboard.keys[DIK_W].trigger || (keyboard.keys[DIK_W].hold && keyHoldTime_ > 60)) {

			int itemIndex = int(selectedOptionItem_) + 1;
			if (itemIndex > int(OptionItem::MainCameraType)) {
				itemIndex = 0;
			}
			selectedOptionItem_ = OptionItem(itemIndex);
		}
		if (pad.Button[PAD_BUTTON_DOWN].trigger || (pad.Button[PAD_BUTTON_DOWN].hold && keyHoldTime_ > 60) ||
			keyboard.keys[DIK_DOWN].trigger || (keyboard.keys[DIK_DOWN].hold && keyHoldTime_ > 60) ||
			keyboard.keys[DIK_S].trigger || (keyboard.keys[DIK_S].hold && keyHoldTime_ > 60)) {

			int itemIndex = int(selectedOptionItem_) - 1;
			if (itemIndex < 0) {
				itemIndex = int(OptionItem::MainCameraType);
			}
			selectedOptionItem_ = OptionItem(itemIndex);
		}
		break;
	default:
		break;
	}

	//項目別値入力
	switch (selectedOptionItem_)
	{
	case Option::OptionItem::lockOnCameraBaseOffsetY:
		OptionItemSlider(changingState_.lockOnCameraBaseOffsetY, 0.1f, pad, keyboard);
		break;
	case Option::OptionItem::lockOnCameraBaseOffsetZ:
		OptionItemSlider(changingState_.lockOnCameraBaseOffsetZ, 0.1f, pad, keyboard);
		break;
	case Option::OptionItem::LockOnCameraSideOffset:
		OptionItemSlider(changingState_.lockOnCameraSideOffset, 0.1f, pad, keyboard);
		break;
	case Option::OptionItem::IsChangeCameraButtonHold:
		OptionItemButton(changingState_.isChangeCameraButtonHold, pad, keyboard);
		break;
	case Option::OptionItem::MainCameraType:
		CameraTypeSlider(changingState_.mainCameraType, pad, keyboard);
		break;
	default:
		break;
	}
}

void Option::Draw() {

}

void Option::Save(std::string filePath) {
	nlohmann::json optionJson;

	//ファイル書き出し部分
	std::ofstream file(filePath);
	file << optionJson.dump(4);
	file.close();
}

void Option::Load(std::string filePath) {
	//読み込むJsonファイル
	std::ifstream file(filePath.c_str());
	if (!file.is_open()) {
		Default();
		return;
	}

	nlohmann::json optionJson;
	file >> optionJson;
	file.close();
}

void Option::Default() {

}

void Option::MoveCursor() {
	switch (selectedOptionCategory_)
	{
	case Option::OptionCategory::Camera:
		break;
	default:
		break;
	}
}

template <typename T>
void Option::OptionItemSlider(T& optionItem, T value, Pad pad, Keyboard keyboard) {

	if (pad.Button[PAD_BUTTON_RIGHT].trigger || (pad.Button[PAD_BUTTON_RIGHT].hold && keyHoldTime_ > 60) ||
		keyboard.keys[DIK_RIGHT].trigger || (keyboard.keys[DIK_RIGHT].hold && keyHoldTime_ > 60) ||
		keyboard.keys[DIK_D].trigger || (keyboard.keys[DIK_D].hold && keyHoldTime_ > 60)) {

		optionItem += value;
	} else if (pad.Button[PAD_BUTTON_LEFT].trigger || (pad.Button[PAD_BUTTON_LEFT].hold && keyHoldTime_ > 60) ||
		keyboard.keys[DIK_LEFT].trigger || (keyboard.keys[DIK_LEFT].hold && keyHoldTime_ > 60) ||
		keyboard.keys[DIK_A].trigger || (keyboard.keys[DIK_A].hold && keyHoldTime_ > 60)) {

		optionItem += value;
	}
}

void Option::OptionItemButton(int& optionItem, Pad pad, Keyboard keyboard) {
	if (pad.Button[PAD_BUTTON_B].trigger ||
		keyboard.keys[DIK_X].trigger ||
		keyboard.keys[DIK_SPACE].trigger) {

		optionItem = 1 - optionItem;
	}
}

//カメラを選択する設定項目
void Option::CameraTypeSlider(GameCamera::CameraType optionItem, Pad pad, Keyboard keyboard) {

	if (pad.Button[PAD_BUTTON_RIGHT].trigger || (pad.Button[PAD_BUTTON_RIGHT].hold && keyHoldTime_ > 60) ||
		keyboard.keys[DIK_RIGHT].trigger || (keyboard.keys[DIK_RIGHT].hold && keyHoldTime_ > 60) ||
		keyboard.keys[DIK_D].trigger || (keyboard.keys[DIK_D].hold && keyHoldTime_ > 60)) {

		optionItem = GameCamera::CameraType(int(optionItem) + 1);

		if (int(optionItem) > GameCamera::CameraType::WideViewCamera) {
			optionItem = GameCamera::CameraType::LockOnCamera;
		}
	} else if (pad.Button[PAD_BUTTON_LEFT].trigger || (pad.Button[PAD_BUTTON_LEFT].hold && keyHoldTime_ > 60) ||
		keyboard.keys[DIK_LEFT].trigger || (keyboard.keys[DIK_LEFT].hold && keyHoldTime_ > 60) ||
		keyboard.keys[DIK_A].trigger || (keyboard.keys[DIK_A].hold && keyHoldTime_ > 60)) {

		optionItem = GameCamera::CameraType(int(optionItem) - 1);

		if (int(optionItem) < GameCamera::CameraType::LockOnCamera) {
			optionItem = GameCamera::CameraType::WideViewCamera;
		}
	}
}