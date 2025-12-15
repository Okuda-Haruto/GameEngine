#include "SpriteManager.h"

unique_ptr<SpriteManager> SpriteManager::instance;

SpriteManager* SpriteManager::GetInstance() {
	if (!instance) {
		instance = make_unique<SpriteManager>();
	}
	return instance.get();
}

void SpriteManager::Finalize() {
	instance.reset();
}

//初期化
void SpriteManager::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;
}