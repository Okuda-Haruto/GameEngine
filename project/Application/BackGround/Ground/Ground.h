#pragma once
#include <memory>
#include <Resource/Object_3D.h>

class Ground
{
private:
	//天球モデル
	std::unique_ptr<Object_3D> model_;
	Object_Multi_Data data_;

public:
	//初期化
	void Initialize();
	//描画
	void Draw();

	void SetCamera(Camera* camera) { model_->SetCamera(camera); }
};
