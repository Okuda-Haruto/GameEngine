#pragma once
#include <memory>
#include <string>

#include <Character/Boss/Boss.h>

class BaseAction;
//1つ単位の行動
class BaseStep {
private:
	BaseAction* action_;
public:
	virtual void Activate(BaseAction* action) = 0;
};

//ボスのアクション
class BaseAction {
private:
	Boss* boss_;
	//ボス行動ステップ
	std::vector<std::unique_ptr<BaseStep>> steps;
public:
	virtual void Initialize(Boss* boss) = 0;
	virtual void Update() = 0;
};

//ボスの行動パターン
class BasePattern {
private:
	//ボス行動
	std::vector<std::unique_ptr<BaseAction>> actions;
public:
	virtual void Initialize(Boss* boss) = 0;
	virtual void Update() = 0;
};

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
	//描画するモデル名
	std::string modelName_;
	//ボス行動パターン
	std::vector<std::unique_ptr<BasePattern>> patterns;


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