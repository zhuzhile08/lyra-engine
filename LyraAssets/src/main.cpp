#define LYRA_LOG_FILE

#define VERSION "1.00"

#include "logger.h"
#include <filesystem>

void display_compress(std::string& answerBuffer);

void ask_for_path(std::string& answerBuffer);

int main() {
	Logger::init();

	std::string answerBuffer;

	Logger::log_info("Lyra command line asset compression tool [Version ", VERSION, "]");
	Logger::log_info("(c) 2022 Zhile Zhu", Logger::end_l());
	ask_for_path(answerBuffer);
}

void display_compress(std::string& answerBuffer) {
	

	ask_for_path(answerBuffer);
}

void ask_for_path(std::string& answerBuffer) {
	answerBuffer.clear();
	Logger::ask(answerBuffer, "Enter the path of a folder to compress: ");
	if (answerBuffer == "compress") display_compress(answerBuffer);
	else if (answerBuffer == "exit") exit(0);
	else {
		Logger::log_error("Invalid path or command! Please try again");
		ask_for_path(answerBuffer);
	}
}
