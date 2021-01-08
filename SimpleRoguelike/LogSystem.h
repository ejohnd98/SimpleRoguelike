#pragma once
#include <queue>
#include <vector>
#include <array>

#include "System.h"

class LogSystem : public System
{
public:
	void AddLog(EventInfo eventInfo);
	void AddLog(TextLog log);
	void PushLogs(bool advanceTurn = false);
	std::vector<TextLog> GetLogs(int count);

private:
	int currentLogPosition = 0;
	int currentLogCount = 0;
	std::array<TextLog, MAX_STORED_LOGS> currentLogs = {};
	std::queue<TextLog> pendingLogs = {};
};