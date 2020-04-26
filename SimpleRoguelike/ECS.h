#pragma once
#include <stdio.h>
#include <iostream>

#include "Constants.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "SystemManager.h"

#include "Components.h"
#include "RendererSystem.h"
#include "TurnSystem.h"
#include "MapSystem.h"
#include "PlayerSystem.h"

//Heavily based on https://austinmorlan.com/posts/entity_component_system/

class ECS
{
public:
	void Init() {
		componentManager = std::make_unique<ComponentManager>();
		entityManager = std::make_unique<EntityManager>();
		systemManager = std::make_unique<SystemManager>();
	}

	//Entity functions
	Entity CreateEntity() {
		Entity e = entityManager->CreateEntity();
		return e;
	}

	void DestroyEntity(Entity entity) {
		entityManager->DestroyEntity(entity);
		componentManager->EntityDestroyed(entity);
		systemManager->EntityDestroyed(entity);
	}

	//Component functions
	template<typename T>
	void RegisterComponent() {
		componentManager->RegisterComponent<T>();
	}

	template<typename T>
	void AddComponent(Entity entity, T component) {
		componentManager->AddComponent<T>(entity, component);

		Signature signature = entityManager->GetSignature(entity);
		signature.set(componentManager->GetComponentType<T>(), true);
		entityManager->SetSignature(entity, signature);

		systemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	void RemoveComponent(Entity entity) {
		componentManager->RemoveComponent<T>(entity);

		Signature signature = entityManager->GetSignature(entity);
		signature.set(componentManager->GetComponentType<T>(), false);
		entityManager->SetSignature(entity, signature);

		systemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	T& GetComponent(Entity entity) {
		return componentManager->GetComponent<T>(entity);
	}

	template<typename T>
	bool HasComponent(Entity entity) {
		return componentManager->HasComponent<T>(entity);
	}

	template<typename T>
	ComponentType GetComponentType() {
		return componentManager->GetComponentType<T>();
	}

	//System functions
	template<typename T>
	std::shared_ptr<T> RegisterSystem() {
		return systemManager->RegisterSystem<T>();
	}

	template<typename T>
	void SetSystemSignature(Signature signature) {
		systemManager->SetSignature<T>(signature);
	}

private:
	std::unique_ptr<ComponentManager> componentManager;
	std::unique_ptr<EntityManager> entityManager;
	std::unique_ptr<SystemManager> systemManager;
};

