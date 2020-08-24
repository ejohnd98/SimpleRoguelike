#pragma once
#include <queue>
#include <vector>
#include <array>

#include "System.h"

enum class LogType {
	ATTACK,
	ATTACK_AND_KILL
};

class LogSystem : public System
{
public:
	void AddLog(EventInfo eventInfo);
	void AddLog(TextLog log);
	void PushLogs();
	std::vector<TextLog> GetLogs(int count);

private:
	int currentLogPosition = 0;
	int currentLogCount = 0;
	std::array<TextLog, MAX_STORED_LOGS> currentLogs = {};
	std::queue<TextLog> pendingLogs = {};
};