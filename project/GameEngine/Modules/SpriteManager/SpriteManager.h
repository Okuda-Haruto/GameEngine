#pragma once
#include "DirectXCommon/DirectXCommon.h"

using namespace std;

class SpriteManager {
private:
	static unique_ptr<SpriteManager> instance;

	DirectXCommon* dxCommon_ = nullptr;
public:

	SpriteManager() = default;
	~SpriteManager() = default;
	SpriteManager(SpriteManager&) = delete;
	SpriteManager& operator=(SpriteManager&) = delete;

	//シングルトンインスタンスの取得
	static SpriteManager* GetInstance();

	//終了
	void Finalize();

	//初期化
	void Initialize(DirectXCommon* dxCommon);

	DirectXCommon* GetDirectXCommon() const { return dxCommon_; }
};