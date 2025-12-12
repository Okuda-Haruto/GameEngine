#include "SpriteManager.h"

SpriteManager* SpriteManager::instance = nullptr;

SpriteManager* SpriteManager::GetInstance() {
	if (instance == nullptr) {
		instance = new SpriteManager;
	}
	return instance;
}

void SpriteManager::Finalize() {
	delete instance;
	instance = nullptr;
}

//初期化
void SpriteManager::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
}