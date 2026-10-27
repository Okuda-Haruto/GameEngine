#pragma once
#include "Factory/AbstractSceneFactory/AbstractSceneFactory.h"
#include "SceneFactory.h"
#include "Input/Input.h"

#include "SceneManager/SceneManager.h"
#include "ModelHolder/ModelHolder.h"
#include "AudioHolder/AudioHolder.h"



class GameManager {
private:
	std::shared_ptr<Input> input_;
	//シーンファクトリー
	std::unique_ptr<AbstractSceneFactory> sceneFactory_;
public:

	~GameManager();

	void Initialize();

	void Update();

	void Draw();;
};