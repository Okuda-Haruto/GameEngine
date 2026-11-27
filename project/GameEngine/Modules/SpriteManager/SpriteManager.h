#pragma once
#include "DirectXCommon/DirectXCommon.h"



class SpriteManager {
private:
	SpriteManager() = default;
	~SpriteManager() = default;
	SpriteManager(SpriteManager&) = delete;
	SpriteManager& operator=(SpriteManager&) = delete;

	DirectXCommon* dxCommon_ = nullptr;
public:
	//シングルトンインスタンスの取得
	static SpriteManager* GetInstance()
	{
		static SpriteManager instance;
		return &instance;
	}

	//終了
	void Finalize();

	//初期化
	void Initialize(DirectXCommon* dxCommon);
};