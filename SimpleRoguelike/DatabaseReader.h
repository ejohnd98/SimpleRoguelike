#include <string>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#pragma once
class DatabaseReader
{
public:	
	static rapidjson::Document ReadJSON(const std::string path);

	static class Actor* DocumentToActor(rapidjson::Document& doc);
};

