#pragma once
#include <BaseScene/BaseScene.h>
#include <string>

//概念シーンファクトリー
class AbstractSceneFactory {
public:
	//仮想デストラクタ
	virtual ~AbstractSceneFactory() = default;
	virtual unique_ptr<BaseScene> CreateScene(const std::string& sceneName) = 0;
};