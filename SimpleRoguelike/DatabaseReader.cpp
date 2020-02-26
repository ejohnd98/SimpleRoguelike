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

void DatabaseReader::ReadActorDatabase(Document& doc) { //reads in actors from the provided doc. Currently does nothing with the data
	Value& actors = doc["actors"];
	for (SizeType i = 0; i < doc["actors"].Size(); i++) {
		Actor* testActor = ValueToActor(doc["actors"][i]);
		std::cout << "Reading in: " << testActor->GetName() << "\n"; //log the actors being read in
		delete testActor;
	}
}

Actor* DatabaseReader::ValueToActor(Value& val) { //deserialize given value into Actor class
	std::string name = val["name"].GetString();
	int sprite = val["sprite"].GetInt();
	int health = val["health"].GetInt();
	int strength = val["strength"].GetInt();
	int faction = val["faction"].GetInt();
	int sight = val["sight"].GetInt();

	Actor* actor = new Actor(name, sprite, health, strength, faction, sight);
	return actor;
}