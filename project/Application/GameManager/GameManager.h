#pragma once
#include <Factory/AbstractSceneFactory/AbstractSceneFactory.h>
#include <SceneFactory.h>

#include <SceneManager/SceneManager.h>
#include "ModelHolder/ModelHolder.h"

using namespace std;

class GameManager {
private:
	//シーンファクトリー
	unique_ptr<AbstractSceneFactory> sceneFactory_;
public:

	~GameManager();

	void Initialize();

	void Update();

	void Draw();;
};