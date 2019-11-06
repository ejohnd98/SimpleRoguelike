#pragma once
#include <string>
class GameLog
{
	std::string value;
	static GameLog* instancePtr;
	GameLog(std::string v = "")
	{
		value = v;
	}
public:
	std::string get_value()
	{
		return value;
	}
	void set_value(std::string v)
	{
		value = v;
	}
	static GameLog* instance()
	{
		if (!instancePtr)
			instancePtr = new GameLog;
		return instancePtr;
	}
};