#include "Stage.h"
#include "../StageManager.h"

void Stage::Initialize(std::string stageName, std::shared_ptr<Input> input) {
	input_ = input;

	//ステージ名からステージデータを得る
	StageManager::GetInstance()->GetStageData(stageName);
}

void Stage::Update() {

}

void Stage::Draw() {

}