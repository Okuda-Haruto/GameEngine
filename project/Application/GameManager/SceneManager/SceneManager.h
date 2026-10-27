#pragma once
#include "BaseScene/BaseScene.h"
#include "Factory/AbstractSceneFactory/AbstractSceneFactory.h"



class SceneManager {
private:
	static std::unique_ptr<SceneManager> instance;

	//今のシーン
	std::unique_ptr<BaseScene> scene_ = nullptr;
	//次のシーン
	std::unique_ptr<BaseScene> nextScene_ = nullptr;

	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;

	std::shared_ptr<Input> input_;
public:

	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(SceneManager&) = delete;
	SceneManager& operator=(SceneManager&) = delete;

	//シングルトンインスタンスの取得
	static SceneManager* GetInstance();
	
	//解放処理
	void Finalize();
	//更新処理
	void Update();
	//描画処理
	void Draw();

	//次のシーンの指定
	void ChangeScene(std::string sceneName);

	//シーンファクトリーのセッター
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }
	void SetInput(std::shared_ptr<Input> input) { input_ = input; }
};