#pragma once
#include <vector>
#include <memory>
#include <Object/Object.h>
#include <Collider/Colliders.h>
#include <Entity/Boss/Boss.h>
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
	std::shared_ptr<SRT> startTransform;
	//接触判定
	std::shared_ptr<Colliders> colliders;
	//有効化されているか
	bool enableObject;
};

//エディター用ボスデータ
struct EditorBossData {
	//出現位置
	std::shared_ptr<SRT> startTransform;
	//ボスファイルへのパス
	std::string filePath;
};


//ステージオブジェクト配置
class StageEditor : public BaseScene {
private:

#pragma region Command

	class BaseCommand {
	protected:
		//エディター
		StageEditor* editor_;
	public:
		//コマンド再実行
		virtual void RedoCommand() = 0;
		//コマンド取消
		virtual void UndoCommand() = 0;
	};

	//オブジェクトの座標移動
	class MoveCommand : public BaseCommand {
	private:
		std::weak_ptr<SRT> targetTransform_;
		SRT beforeTransform_;
		SRT afterTransform_;
	public:
		//コマンド実行
		MoveCommand(StageEditor* editor, std::weak_ptr<SRT> targetTransform, SRT beforeTransform, SRT afterTransform);
		//コマンド再実行
		void RedoCommand() override;
		//コマンド取消
		void UndoCommand() override;
	};

	//接触可能オブジェクトの追加
	class AddColliderObjectCommand : public BaseCommand {
	private:
		int32_t index_;
	public:
		//コマンド実行
		AddColliderObjectCommand(StageEditor* editor, int32_t index, SRT transform, std::string directoryPath, std::string filename);
		//コマンド再実行
		void RedoCommand() override;
		//コマンド取消
		void UndoCommand() override;
	};

	//接触可能オブジェクトの削除
	class DeleteColliderObjectCommand : public BaseCommand {
	private:
		int32_t index_;
	public:
		//コマンド実行
		DeleteColliderObjectCommand(StageEditor* editor, int32_t index);
		//コマンド再実行
		void RedoCommand() override;
		//コマンド取消
		void UndoCommand() override;
	};

	//破壊可能オブジェクトの追加
	class AddBreakObjectCommand : public BaseCommand {
	private:
		int32_t index_;
	public:
		//コマンド実行
		AddBreakObjectCommand(StageEditor* editor, int32_t index, SRT transform, std::string directoryPath, std::string filename);
		//コマンド再実行
		void RedoCommand() override;
		//コマンド取消
		void UndoCommand() override;
	};

	//破壊可能オブジェクトの削除
	class DeleteBreakObjectCommand : public BaseCommand {
	private:
		int32_t index_;
	public:
		//コマンド実行
		DeleteBreakObjectCommand(StageEditor* editor, int32_t index);
		//コマンド再実行
		void RedoCommand() override;
		//コマンド取消
		void UndoCommand() override;
	};

#pragma endregion

	//実行コマンド
	std::vector<std::unique_ptr<BaseCommand>> commands_;
	int32_t commandIndex_;

	//エディター状態
	enum class EditorState {
		None,	//ステージを選択していない状態
		Edit,	//ステージファイルを開いた状態
	};
	EditorState state_;

	//ステージファイル読み込み設定
	enum class OpenFile {
		None,			
		CreateNewFile,	//ステージファイルを新規作成
		ReadFile,		//既存のファイルを読み込む,
		ReadLastOpenFile//最後に読み込んだファイルを開く
	};
	OpenFile openFile_;

	//接地、壁判定を取るオブジェクト
	std::vector<EditorObject> colliderObjects_;
	//破壊可能オブジェクト
	std::vector<EditorObject> breakObjects_;
	//ボス
	EditorBossData bossData_;
	//プレイヤー出現位置
	std::shared_ptr<SRT> playerStartTransform_;

	//接地、壁判定を取るオブジェクトの最終番号
	int32_t currentColliderIndex_;
	//破壊可能オブジェクトの最終番号
	int32_t currentBreakIndex_;

	//選択中オブジェクトの種類
	enum class IndexType {
		None,
		Player,
		Boss,
		ColliderObject,
		BreakObject
	};
	IndexType indexType_;

	//選択しているオブジェクト番号
	int32_t cursorObjectIndex_;

	//コピー対象
	IndexType copyIndexType_;
	//選択しているオブジェクト番号
	int32_t copyObjectIndex_;
	//コピー座標
	SRT copyTransform_;

	//移動前の座標
	SRT beforeTransform_;
	SRT beforeTransformInput_;
	//ギズモ用Transform
	std::weak_ptr<SRT> nextTransform_;
	//選択中の接触可能オブジェクト
	int32_t choiceObject_ = 0;
	//transform編集中か
	bool isEditingTransform_;

	//出力用データ
	StageData stageData_;
	std::string filePath_;
	//ImGui用
	char filePathText_[256] = {};
	
	std::unique_ptr<Stage> stage_;

	std::shared_ptr<Input> input_;
	std::shared_ptr<DebugCamera> debugCamera_;

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
	/// <param name="filePath">ステージファイルへのパス</param>
	void ReadStageFile(std::string filePath);

	/// <summary>
	/// ステージファイルを書き出す
	/// </summary>
	void WriteStageFile();

	/// <summary>
	/// 接地オブジェクトの追加
	/// </summary>
	/// <param name="index">番号</param>
	void AddColliderObject(int32_t index, SRT transform, std::string directoryPath, std::string filename);

	void SetEnableColliderObject(int32_t index, bool enableObject) { colliderObjects_[index].enableObject = enableObject; }

	void AddBreakObject(int32_t index, SRT transform, std::string directoryPath, std::string filename);

	void SetEnableBreakObject(int32_t index, bool enableObject) { breakObjects_[index].enableObject = enableObject; }

	/// <summary>
	/// ボスの変更
	/// </summary>
	/// <param name="name">ボスの名前</param>
	void ChangeBoss(std::string name);

private:

	//マウスカーソル地点への半直線
	Ray GetCursorRay(std::shared_ptr<GameCamera> gameCamera, std::shared_ptr<Input> input);

	//半直線からオブジェト座標を得る
	std::shared_ptr<SRT> GetObjectTransformFromRay(Ray ray);

	//ファイルを開く
	void OpenFileWindow();

	//前回開いたファイルを保存
	void SaveLastOpenFilePath(std::string lastOpenFilePath);
	std::string LoadLastOpenFilePath();

	/// <summary>
	/// ファイル参照ツリー
	/// </summary>
	/// <param name="path">探索するフォルダパス</param>
	/// <param name="name">ノードにつける名称</param>
	void ImGuiFileTree_ColliderObject(std::string path = "resources", std::string name = "resources");
	void ImGuiFileTree_BreakObject(std::string path = "resources", std::string name = "resources");
	void ImGuiFileTree_json(std::string path = "resources", std::string name = "resources");

	void ImGuiFolderTree(std::string path = "resources", std::string name = "resources");

};