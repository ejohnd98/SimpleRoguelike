#pragma once
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <queue>
#include <array>
#include "ComponentArray.h"

#include "Constants.h"

class ComponentManager
{
public:
	template<typename T>
	void RegisterComponent() {
		const char* typeName = typeid(T).name();

		assert(componentTypes.find(typeName) == componentTypes.end());

		componentTypes.insert({ typeName, nextComponentType });

		componentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

		nextComponentType++;
	}

	template<typename T>
	ComponentType GetComponentType() {
		const char* typeName = typeid(T).name();

		assert(componentTypes.find(typeName) != componentTypes.end());

		return componentTypes[typeName];
	}

	template<typename T>
	void AddComponent(Entity entity, T component) { //add component to array for entity
		GetComponentArray<T>()->InsertData(entity, component);
	}

	template<typename T>
	void RemoveComponent(Entity entity) { //remove entity's component from array
		GetComponentArray<T>()->RemoveData(entity);
	}

	template<typename T>
	T& GetComponent(Entity entity) { //Get reference to entity's component
		return GetComponentArray<T>()->GetData(entity);
	}

	template<typename T>
	bool HasComponent(Entity entity) { //Get reference to entity's component
		return GetComponentArray<T>()->HasComponent(entity);
	}


	void EntityDestroyed(Entity entity) { //tells each component array of entity's destruction
		for (auto const& pair : componentArrays) {
			auto const& component = pair.second;

			component->EntityDestroyed(entity); //if entity has component, will remove it
		}
	}

private:
	//map from type string to component type
	std::unordered_map<const char*, ComponentType> componentTypes;

	//map from type string to component array pointer
	std::unordered_map<const char*, std::shared_ptr<ComponentArrayInterface>> componentArrays;

	ComponentType nextComponentType; //essentially an iterator for component type ids

	template<typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray() {
		const char* typeName = typeid(T).name();

		assert(componentTypes.find(typeName) != componentTypes.end()); //make sure component already registered

		return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]); //create shared pointer
	}
};

