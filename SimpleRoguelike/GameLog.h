#pragma once
#include <string>
#include <vector>
class GameLog
{
	std::vector<std::string> logVector;
	std::string value;
	static GameLog* instancePtr;
	GameLog(std::string v = "")
	{
		logVector.push_back(v);
	}
public:
	static GameLog* instance()
	{
		if (!instancePtr)
			instancePtr = new GameLog;
		return instancePtr;
	}

	std::string get_value();
	void set_value(std::string v);
	void AddLog(std::string v);
	std::string GetLogs(int num);
};