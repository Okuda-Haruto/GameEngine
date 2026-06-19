#pragma once
#include <memory>
#include <string>

#include <StageManager/Stage/Stage.h>

class BossEditor : public Stage {
private:

#pragma region Command

	class BaseCommand {
	private:
		//エディター
		BossEditor* editor_;
	public:
		//コマンド実行
		virtual void DoCommand(BossEditor* editor) = 0;
		//コマンド再実行
		virtual void ReDoCommand() = 0;
		//コマンド取消
		virtual void UndoCommand() = 0;
	};

#pragma endregion

	//ボス名
	std::string bossName_;
	//描画するモデルファイルへのパス
	std::string directoryPath_;
	std::string modelname_;
	//最大HP
	uint32_t maxHP_;
	//ボス行動パターン
	std::unordered_map<std::string, std::unique_ptr<BossPattern>> patterns_;



	enum class EditorState {
		None,			//ボスを選択していない状態
		CreateNewFile,	//新しく作成するファイルの設定
		OpenFile,		//ボスファイルを開いた状態
		Edit,
	};

	EditorState state_;

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
	/// ボスファイルを読み込む
	/// </summary>
	/// <param name="filePath">ボスファイルファイルへのパス</param>
	void ReadBossFile(std::string filePath);

	/// <summary>
	/// ボスファイルファイルを書き出す
	/// </summary>
	/// <param name="filePath">ボスファイルファイルへのパス</param>
	void WriteBossFile(std::string filePath);
};