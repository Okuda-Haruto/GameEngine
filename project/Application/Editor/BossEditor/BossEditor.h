#pragma once
#include <memory>
#include <string>

#include <Character/Boss/Boss.h>

class BossEditor {
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
	std::string modelPath_;
	//最大HP
	uint32_t maxHP_;
	//ボス行動パターン
	std::vector<std::unique_ptr<BossPattern>> patterns;


	//ボス
	std::unique_ptr<Boss> boss_;

	enum class EditorState {
		None,			//ボスを選択していない状態
		CreateNewFile,	//新しく作成するファイルの設定
		OpenFile,		//
		Edit,
	};

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