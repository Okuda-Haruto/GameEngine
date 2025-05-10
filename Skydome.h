#pragma once
#include <GameEngine.h>

//天球
class Skydome {
private:
	//ワールド変換データ
	Object_3D_Data data_;

	Vector4 color_;
	//モデル
	Object_3D* object_ = nullptr;

	Texture* texture_ = nullptr;

	Light* light_ = nullptr;

public:
	//初期化
	void Initialize(Object_3D* object, Texture* texture);
	//更新
	void Update();
	//描画
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList,Camera camera);

	void Reset() { object_->Reset(); };
};