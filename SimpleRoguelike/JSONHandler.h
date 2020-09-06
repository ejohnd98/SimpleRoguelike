#include <string>
#include "System.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#pragma once
class JSONHandler
{
public:
	static void ReadRoomJSONs(const std::string path);
	static RoomPrefab ReadRoomPrefab(rapidjson::Document& doc);
	static Position ValueToPosition(rapidjson::Value& val);
};