#pragma once
#include <Factory/AbstractSceneFactory/AbstractSceneFactory.h>

using namespace std;

//シーンファクトリー
class SceneFactory : public AbstractSceneFactory {
public:
	
	/// <summary>
	/// シーン生成
	/// </summary>
	/// <param name="sceneName">シーン名</param>
	/// <returns>生成したシーン</returns>
	unique_ptr<BaseScene> CreateScene(const std::string& sceneName) override;
};