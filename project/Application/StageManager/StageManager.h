#pragma once
#include <memory>
#include <map>
#include <string>
#include <Vector3.h>

//ボスに関するデータ
struct BossData {
	std::string filepath;
	Vector3 spownPosition;
};

//ステージのデータ
struct StageData {
	//ボスに関するデータ
	BossData bossData;
	//プレイヤー出現位置
	Vector3 playerSpownPosition;
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
	StageData GetStageData(std::string stageName) { return stageDatas_[stageName]; }

	/// <summary>
	/// ステージファイルを読み込む
	/// </summary>
	/// <param name="filePath">ステージファイルへのパス</param>
	void ReadStage(std::string filePath);
};