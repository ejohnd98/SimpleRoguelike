#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "DatabaseReader.h"
#include "Actor.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

Document DatabaseReader::ReadJSON(const std::string path) { //reads JSON at path into a rapidJSON document

	std::ifstream ifs(path);
	std::stringstream buffer;
	buffer << ifs.rdbuf();

	Document doc;
	doc.Parse(buffer.str().c_str());

	return doc;
}

Actor* DatabaseReader::DocumentToActor(Document& doc) { //placeholder json names
	Value& name = doc["name"];
	Value& sprite = doc["sprite"];

	return new Actor(name.GetString(), sprite.GetInt());
}