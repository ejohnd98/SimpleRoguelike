#pragma once
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <unordered_map>
#include "System.h"


class SystemManager
{
public:
	template<typename T>
	std::shared_ptr<T> RegisterSystem() {
		const char* typeName = typeid(T).name();

		assert(systems.find(typeName) == systems.end());
	
		auto system = std::make_shared<T>();
		systems.insert({ typeName, system });
		return system;
	}

	template<typename T>
	void SetSignature(Signature signature) {
		const char* typeName = typeid(T).name();

		assert(systems.find(typeName) != systems.end());

		signatures.insert({ typeName, signature });
	}

	void EntityDestroyed(Entity entity) { //remove from any systems the entity is in
		for (auto const& pair : systems) {
			auto const& system = pair.second;
			system->entities.erase(entity); //no check needed when erasing from sets
		}
	}

	void EntitySignatureChanged(Entity entity, Signature entitySignature) {
		for (auto const& pair : systems) {

			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemSignature = signatures[type];

			if ((entitySignature & systemSignature) == systemSignature) {
				system->entities.insert(entity); //entity has component, so insert into system
			} else {
				system->entities.erase(entity); //otherwise erase entity from system
			}
		}
	}

private:
	//map from system type to system signature
	std::unordered_map<const char*, Signature> signatures{};

	// map from system type to system
	std::unordered_map<const char*, std::shared_ptr<System>> systems{};
};

