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
	//Transform
	SRT transform;
};

//ボスに関するデータ
struct BossData {
	//ボスファイルのパス
	std::string filepath;
	//出現位置
	Vector3 spawnPosition;
};

//ステージのデータ
struct StageData {
	//ボスに関するデータ
	BossData bossData;
	//プレイヤー出現位置
	Vector3 playerSpawnPosition;
	//接触可能オブジェクト
	std::vector<ColliderObjectData> colliderObjects;

	std::string filePath_;
};

//ステージ管理
class StageManager {
private:
	static std::unique_ptr<StageManager> instance;

	//ステージデータ
	std::map<std::string, StageData> stageDatas_;
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
	/// ステージデータを得る
	/// </summary>
	/// <param name="stageName">ステージ名</param>
	StageData GetStageData(std::string stageName);

	/// <summary>
	/// ステージファイルを読み込む
	/// </summary>
	/// <param name="filePath">ステージファイルへのパス</param>
	void ReadStage(std::string filePath);
};