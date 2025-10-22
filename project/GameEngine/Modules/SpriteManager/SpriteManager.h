#pragma once
#include "DirectXCommon/DirectXCommon.h"

class SpriteManager {
private:
	DirectXCommon* dxCommon_ = nullptr;
public:
	//初期化
	void Initialize(DirectXCommon* dxCommon);

	DirectXCommon* GetDirectXCommon() const { return dxCommon_; }
};