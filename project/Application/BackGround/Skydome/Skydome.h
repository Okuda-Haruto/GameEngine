#pragma once
#include <memory>
#include <PrimitiveManager/PrimitiveBox.h>
#include <Camera/Camera.h>
#include <ModelHolder/ModelHolder.h>

class Skydome
{
private:
	//天球モデル
	std::unique_ptr<PrimitiveBox> skybox_;

public:
	//初期化
	void Initialize(std::shared_ptr<Camera> camera);
	//描画
	void Draw();
};
