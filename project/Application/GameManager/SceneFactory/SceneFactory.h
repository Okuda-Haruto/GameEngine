#pragma once
#include <Factory/AbstractSceneFactory/AbstractSceneFactory.h>

//シーンファクトリー
class SceneFactory : public AbstractSceneFactory {
public:
	
	/// <summary>
	/// シーン生成
	/// </summary>
	/// <param name="sceneName">シーン名</param>
	/// <returns>生成したシーン</returns>
	BaseScene* CreateScene(const std::string& sceneName) override;
};