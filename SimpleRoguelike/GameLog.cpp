#include <string>
#include "GameLog.h"

void GameLog::AddLog(std::string str) {
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