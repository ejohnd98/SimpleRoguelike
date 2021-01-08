#include <stdio.h>
#include <iostream>
#include <SDL.h>
#include <algorithm>

#include "LogSystem.h"
#include "ECS.h"

extern std::shared_ptr <ECS> ecs;

void LogSystem::AddLog(EventInfo eventInfo) {
	switch (eventInfo.type) {
		case EventType::ATTACK: {
			std::string log = {
				"<purple>"
				+ ecs->GetComponent<Info>(eventInfo.source).name
				+ "<> attacked <orange>"
				+ ecs->GetComponent<Info>(eventInfo.target).name
				+ "<> for <red>"
				+ std::to_string(eventInfo.damageDealt) + "<> damage" + (eventInfo.killed ? ", <red>killing it" : "")};
			AddLog({ log });
			break;
		}
		case EventType::MOVE_FLOORS: {
			std::string log = {
				(eventInfo.descended ? "<red>You descended to floor " : "<red>You ascended to floor ") + std::to_string(eventInfo.newFloor)
			};
			AddLog({ log });
			break;
		}
		case EventType::PICK_UP_ITEM: {
			std::string log = {
				"You picked up <blue>" + eventInfo.item.name };
			AddLog({ log });
			break;
		}
		default:
			break;
	}
}

void LogSystem::AddLog(TextLog log) {
	pendingLogs.push(log);
}

void LogSystem::PushLogs() {
	if (!pendingLogs.empty()) {
		currentLogPosition = (currentLogPosition + 1) % MAX_STORED_LOGS;
		currentLogs[currentLogPosition] = TextLog{"-------------------------------------------------------------------------"};
		currentLogCount = std::min(MAX_STORED_LOGS, currentLogCount + 1);
	}
	while (!pendingLogs.empty()) {
		currentLogPosition = (currentLogPosition + 1) % MAX_STORED_LOGS;
		currentLogs[currentLogPosition] = pendingLogs.front();
		pendingLogs.pop();
		currentLogCount = std::min(MAX_STORED_LOGS, currentLogCount + 1);
	}
}

std::vector<TextLog> LogSystem::GetLogs(int count) {
	int returnCount = std::min(count, currentLogCount);

	std::vector<TextLog> returnLogs;
	returnLogs.reserve(returnCount);

	for (int i = 0; i < returnCount; i++) {
		returnLogs.push_back(currentLogs[std::abs((currentLogPosition - i) % MAX_STORED_LOGS)]);
	}
	return returnLogs;
}