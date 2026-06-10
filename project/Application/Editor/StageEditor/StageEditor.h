#pragma once
#include <vector>
#include <memory>
#include <Object/Object.h>
#include <Collider/Colliders.h>
#include <Character/Boss/Boss.h>

//struct内std::unique_ptrだとコピー不可になってコマンドで扱えないのでstd::shared_ptr。よく考えたらStageEditor外に出さないかも

//エディター用オブジェクトデータ
struct EditorObject {
	//描画オブジェクト(nullptrなら表示しない)
	std::shared_ptr<Object> object;
	//Transform
	SRT transform;
	//接触判定
	std::shared_ptr<Colliders> colliders;
	//有効化されているか
	bool enableObject;
};

//エディター用ボスデータ
struct BossData {
	//ボス
	std::shared_ptr<Boss> boss;
	//出現位置
	Vector3 spownPosition;
};


//ステージオブジェクト配置
class StageEditor {
private:

#pragma region Command

	class BaseCommand {
	private:
		//エディター
		StageEditor* editor_;
	public:
		//コマンド実行
		virtual void DoCommand(StageEditor* editor) = 0;
		//コマンド再実行
		virtual void ReDoCommand() = 0;
		//コマンド取消
		virtual void UndoCommand() = 0;
	};

#pragma endregion

	//実行コマンド
	std::vector<BaseCommand> commands_;
	int32_t commandIndex_;

	//接地、壁判定を取るオブジェクト
	std::vector<EditorObject> groundObjects_;
	//ボス
	BossData bossData_;
	//プレイヤー出現位置
	Vector3 playerSpownPosition_;

	int32_t currentGroundIndex_;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// ステージファイルを読み込む
	/// </summary>
	/// <param name="filePath">ステージファイルへのパス</param>
	void ReadStageFile(std::string filePath);

	/// <summary>
	/// ステージファイルを書き出す
	/// </summary>
	/// <param name="filePath">ステージファイルへのパス</param>
	void WriteStageFile(std::string filePath);

	/// <summary>
	/// 接地オブジェクトの追加
	/// </summary>
	/// <param name="index">番号</param>
	void AddGroundObject(int32_t index, std::shared_ptr<Model> model, SRT transform);	//順番は常に一定

	/// <summary>
	/// 接地オブジェクトの削除
	/// </summary>
	/// <param name="index"></param>
	void DeleteGroundObject(int32_t index);

	/// <summary>
	/// 接地オブジェクトの位置変更
	/// </summary>
	/// <param name="index">番号</param>
	/// <param name="position">位置</param>
	void SetGroundObjectPosition(int32_t index, Vector3 position);

	/// <summary>
	/// 接地オブジェクトの向き変更
	/// </summary>
	/// <param name="index">番号</param>
	/// <param name="direction">正面の向き</param>
	void SetGroundObjectDirection(int32_t index, Vector3 direction);

	/// <summary>
	/// 接地オブジェクトの拡縮変更
	/// </summary>
	/// <param name="index">番号</param>
	/// <param name="scale">拡縮</param>
	void SetGroundObjectScale(int32_t index, Vector3 scale);

	/// <summary>
	/// ボスの変更
	/// </summary>
	/// <param name="name">ボスの名前</param>
	void ChangeBoss(std::string name);

	/// <summary>
	/// ボスの開始地点変更
	/// </summary>
	/// <param name="spownPosition">開始地点</param>
	void SetBossSpownPosition(Vector3 spownPosition);

	/// <summary>
	/// プレイヤーの開始地点変更
	/// </summary>
	/// <param name="spownPosition">開始地点</param>
	void SetPlayerSpownPosition(Vector3 spownPosition);
};