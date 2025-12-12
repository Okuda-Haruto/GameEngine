#pragma once
#include <BaseScene/BaseScene.h>
#include <Factory/AbstractSceneFactory/AbstractSceneFactory.h>

using namespace std;

class SceneManager {
private:
	static SceneManager* instance;

	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(SceneManager&) = delete;
	SceneManager& operator=(SceneManager&) = delete;

	//今のシーン
	BaseScene* scene_ = nullptr;
	//次のシーン
	BaseScene* nextScene_ = nullptr;

	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
public:
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
};