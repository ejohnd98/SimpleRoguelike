#pragma once
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <queue>
#include <unordered_map>
#include <array>

#include "Constants.h"


class ComponentArrayInterface //need an interface so that lists of differently typed ComponentArrays can work
{
public:
	virtual ~ComponentArrayInterface() = default; //default destructor if none provided
	virtual void EntityDestroyed(Entity entity) = 0;
};

template<typename T>
class ComponentArray : public ComponentArrayInterface {
public:
	void InsertData(Entity entity, T component) {
		assert(entityToIndexMap.find(entity) == entityToIndexMap.end()); //ensure entity isn't in array

		size_t newIndex = size;
		entityToIndexMap[entity] = newIndex;
		indexToEntityMap[newIndex] = entity;
		componentArray[newIndex] = component;
		size++;
	}

	void RemoveData(Entity entity) {
		assert(entityToIndexMap.find(entity) != entityToIndexMap.end()); //ensure entity is in array
	
		//fill new hole with last element in array
		size_t indexToRemove = entityToIndexMap[entity];
		size_t lastIndex = size - 1;
		componentArray[indexToRemove] = componentArray[lastIndex];

		//Update map to account for moving last element
		Entity lastEntity = indexToEntityMap[lastIndex];
		entityToIndexMap[lastEntity] = indexToRemove;
		indexToEntityMap[indexToRemove] = lastEntity;

		entityToIndexMap.erase(entity); //remove the entity
		indexToEntityMap.erase(lastIndex); //remove last index
		size--;
	}

	T& GetData(Entity entity) {
		assert(entityToIndexMap.find(entity) != entityToIndexMap.end()); //ensure entity is in array

		return componentArray[entityToIndexMap[entity]];
	}

	bool HasComponent(Entity entity) {
		return (entityToIndexMap.find(entity) != entityToIndexMap.end());
	}

	void EntityDestroyed(Entity entity) override {
		if (entityToIndexMap.find(entity) != entityToIndexMap.end()) {
			RemoveData(entity); //remove entity's component if it exists in array
		}
	}
private:
	//contains component data, packed
	std::array<T, MAX_ENTITIES> componentArray;

	//maps between entity ids and array indicies
	std::unordered_map<Entity, size_t> entityToIndexMap;
	std::unordered_map<size_t, Entity> indexToEntityMap;

	size_t size;
};