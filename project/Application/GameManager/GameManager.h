#pragma once
#include <Factory/AbstractSceneFactory/AbstractSceneFactory.h>
#include <SceneFactory.h>
#include <Input/Input.h>

#include <SceneManager/SceneManager.h>
#include "ModelHolder/ModelHolder.h"
#include "AudioHolder/AudioHolder.h"

using namespace std;

class GameManager {
private:
	shared_ptr<Input> input_;
	//シーンファクトリー
	unique_ptr<AbstractSceneFactory> sceneFactory_;
public:

	~GameManager();

	void Initialize();

	void Update();

	void Draw();;
};