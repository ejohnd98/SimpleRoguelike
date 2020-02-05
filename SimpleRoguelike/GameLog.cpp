#include <string>
#include "GameLog.h"

bool clearOnNextLog = true;

void GameLog::ClearLog() {
	logVector.clear();
	clearOnNextLog = false;
}
void GameLog::AdvanceTurn() {
	if (clearOnNextLog) {
		ClearLog();
	}
	clearOnNextLog = true;
}
void GameLog::AddLog(std::string str) {
	if (clearOnNextLog) {
		ClearLog();
	}
	logVector.insert(logVector.begin(),str);
}
std::string GameLog::GetLogs(int num) {
	std::string concatLogs = "";
	if (num > logVector.size() ) {
		num = logVector.size();
	}
	for (int i = 0; i < num; i++) {
		concatLogs += logVector.at(i);
		concatLogs += "\n";
	}
	return concatLogs;
}