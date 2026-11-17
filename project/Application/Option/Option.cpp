#include "Option.h"
#include "JsonUtils.h"

void Option::Initialize(std::weak_ptr<Input> input) {
	input_ = input;

	selectedOptionCategory_ = OptionCategory::Camera;
	selectedOptionItem_ = OptionItem::LockOnCameraBaseOffset;
}

void Option::Update() {
	Pad pad = input_.lock()->GetPad();

	switch (selectedOptionCategory_)
	{
	case Option::OptionCategory::Camera:
		if (pad.Button[PAD_BUTTON_UP].trigger) {
			int itemIndex = int(selectedOptionItem_) + 1;
			if (itemIndex > int(OptionItem::MainCameraType)) {
				itemIndex = 0;
			}
			selectedOptionItem_ = OptionItem(itemIndex);
		}
		if (pad.Button[PAD_BUTTON_DOWN].trigger) {
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