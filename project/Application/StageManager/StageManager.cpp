#include "StageManager.h"

std::unique_ptr<StageManager> StageManager::instance;

StageManager* StageManager::GetInstance() {
	if (!instance) {
		instance = std::make_unique<StageManager>();
	}
	return instance.get();
}

void StageManager::Finalize() {
	//データの解放
	stageDatas_.clear();

	instance.reset();
}

void StageManager::ReadStage(std::string filePath) {

}