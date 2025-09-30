#include "Log.h"

//ログの書き出し
void Log(std::ostream& os, const std::string& message) {
	os << message << std::endl;
	OutputDebugStringA(message.c_str());
}

//ログファイルの生成
std::ofstream CreateLogFile() {
	//ログのディレクトリを用意
	std::filesystem::create_directory("logs");

	//現在時刻を取得(UTC時間)
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	//ログファイルの名前にコンマ何秒はいらないので、削って秒にする
	std::chrono::time_point < std::chrono::system_clock, std::chrono::seconds>
		nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
	//日本時間(PCの設定時間)に変換
	std::chrono::zoned_time localTime{ std::chrono::current_zone(),nowSeconds };
	//formatを使って年月日_時分秒の文字列に変換
	std::string dataString = std::format("{:%Y%m%d_%H%M%S}", localTime);
	//時刻を使ってファイル名を変更
	std::string logFilePath = std::string("logs/") + dataString + ".log";
	//ファイルを作って書き込み準備
	std::ofstream logStream(logFilePath);
	return logStream;
}