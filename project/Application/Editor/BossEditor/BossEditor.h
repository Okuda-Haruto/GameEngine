#pragma once
#include <memory>
#include <string>

#include <StageManager/Stage/Stage.h>
#include <GameManager/BaseScene/BaseScene.h>

class BossEditor : public BaseScene {
private:

#pragma region Command

	class BaseCommand {
	protected:
		//エディター
		BossEditor* editor_;
	public:
		//コマンド再実行
		virtual void RedoCommand() = 0;
		//コマンド取消
		virtual void UndoCommand() = 0;
	};

	class SetModelCommand : public BaseCommand {
	private:
		std::string beforeDirectoryPath_;
		std::string beforeFileName_;
		std::string afterDirectoryPath_;
		std::string afterFileName_;
	public:
		//コマンド実行
		SetModelCommand(BossEditor* editor, std::string beforeDirectoryPath, std::string beforeFileName, std::string afterDirectoryPath, std::string afterFileName);
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

	//ボス名
	std::string bossName_;
	//描画するモデルファイルへのパス
	std::string directoryPath_;
	std::string modelname_;
	//最大HP
	uint32_t maxHP_;
	//ボス行動パターン
	std::unordered_map<std::string, std::unique_ptr<BossPattern>> patterns_;

	//出力用データ
	StageData stageData_;
	std::string filePath_;
	//ImGui用
	char filePathText_[256] = {};
	char directoryPathText_[256]{};
	char modelnameText_[256]{};

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
	/// ボスファイルを読み込む
	/// </summary>
	/// <param name="filePath">ボスファイルファイルへのパス</param>
	void ReadBossFile(std::string filePath);

	/// <summary>
	/// ボスファイルファイルを書き出す
	/// </summary>
	/// <param name="filePath">ボスファイルファイルへのパス</param>
	void WriteBossFile();

private:

	void SetBossData(std::string directoryPath = "resources", std::string modelname = "resources");


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
	bool ImGuiFileTree_obj(std::string& path, std::string name);
	bool ImGuiFileTree_json(std::string& filePath, std::string path = "resources", std::string name = "resources");

	bool ImGuiFolderTree(std::string& folderPath, std::string path = "resources", std::string name = "resources");
};