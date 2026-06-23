#pragma once
#include <vector>
#include <memory>
#include <Object/Object.h>
#include <Collider/Colliders.h>
#include <Character/Boss/Boss.h>
#include <StageManager/Stage/Stage.h>
#include <GameManager/BaseScene/BaseScene.h>
#include <Math/Shape/Line.h>
#include <StageManager/StageManager.h>


//struct内std::unique_ptrだとコピー不可になってコマンドで扱えないのでstd::shared_ptr。よく考えたらStageEditor外に出さないかも

//エディター用オブジェクトデータ
struct EditorObject {
	//描画オブジェクト(nullptrなら表示しない)
	std::shared_ptr<Object> object;
	//ファイルパス
	std::string directoryPath;
	std::string filename;
	//Transform
	SRT transform;
	//接触判定
	std::shared_ptr<Colliders> colliders;
	//有効化されているか
	bool enableObject;
};

//エディター用ボスデータ
struct EditorBossData {
	//ボス
	std::shared_ptr<Boss> boss;
	//出現位置
	Vector3 spawnPosition;
	//ボスファイルへのパス
	std::string filePath;
};


//ステージオブジェクト配置
class StageEditor : public BaseScene {
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

	enum class EditorState {
		None,			//ステージを選択していない状態
		CreateNewFile,	//新しく作成するファイルの設定
		OpenFile,		//ステージファイルを開いた状態
		Edit,
	};

	EditorState state_;

	//実行コマンド
	std::vector<BaseCommand> commands_;
	int32_t commandIndex_;

	//接地、壁判定を取るオブジェクト
	std::vector<EditorObject> colliderObjects_;
	//ボス
	EditorBossData bossData_;
	//プレイヤー出現位置
	Vector3 playerSpownPosition_;

	//接地、壁判定を取るオブジェクトの最終番号
	int32_t currentColliderIndex_;

	std::unique_ptr<Stage> stage_;

	std::shared_ptr<Input> input_;
	std::shared_ptr<DebugCamera> debugCamera_;

	int32_t choiceObject_ = 0;

	StageData stageData_;
	std::string stageName_;
	
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(std::shared_ptr<Input> input);

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
	/// <param name="stageName">ステージファイルへのパス</param>
	void ReadStageFile(std::string stageName);

	/// <summary>
	/// ステージファイルを書き出す
	/// </summary>
	void WriteStageFile();

	/// <summary>
	/// 接地オブジェクトの追加
	/// </summary>
	/// <param name="index">番号</param>
	void AddColliderObject(int32_t index, std::string directoryPath, std::string filename, SRT transform);	//順番は常に一定

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

	void ImGuiFileTree(std::string path, std::string name);
};