#pragma once
#include <BaseScene/BaseScene.h>
#include <Factory/AbstractSceneFactory/AbstractSceneFactory.h>

using namespace std;

class SceneManager {
private:
	static unique_ptr<SceneManager> instance;

	//今のシーン
	unique_ptr<BaseScene> scene_ = nullptr;
	//次のシーン
	unique_ptr<BaseScene> nextScene_ = nullptr;

	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;

	shared_ptr<Input> input_;
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
	void ChangeScene(string sceneName);

	//シーンファクトリーのセッター
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }
	void SetInput(shared_ptr<Input> input) { input_ = input; }
};