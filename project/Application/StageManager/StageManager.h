#pragma once
#include <memory>
#include <map>
#include <string>
#include <Vector3.h>
#include <Object/Object.h>
#include <Collider/Colliders.h>

//エディター用オブジェクトデータ
struct ColliderObjectData {
	//モデルパス
	std::string directoryPath;
	std::string filename;
	//出現座標
	SRT startTransform;
};

//破壊可能オブジェクトデータ
struct BreakObjectData {
	//モデルパス
	std::string directoryPath;
	std::string filename;
	//出現座標
	SRT startTransform;
};

//ボスに関するデータ
struct BossData {
	//ボスファイルのパス
	std::string filepath;
	//出現座標
	SRT startTransform;
};

//ステージのデータ
struct StageData {
	//ボスに関するデータ
	BossData bossData;
	//プレイヤー出現座標
	SRT playerStartTransform;
	//接触可能オブジェクト
	std::vector<ColliderObjectData> colliderObjects;
	//破壊可能オブジェクト
	std::vector<BreakObjectData> breakObjects;
};

//ステージ管理
class StageManager {
private:
	static std::unique_ptr<StageManager> instance;
public:

	StageManager() = default;
	~StageManager() = default;
	StageManager(StageManager&) = delete;
	StageManager& operator=(StageManager&) = delete;

	//シングルトンインスタンスの取得
	static StageManager* GetInstance();

	//解放処理
	void Finalize();

	/// <summary>
	/// ステージファイルを読み込む
	/// </summary>
	/// <param name="filePath">ステージファイルへのパス</param>
	StageData ReadStage(std::string filePath);

	/// <summary>
	/// ステージデータの保存
	/// </summary>
	void WriteStage(std::string filePath, StageData stageData);
};