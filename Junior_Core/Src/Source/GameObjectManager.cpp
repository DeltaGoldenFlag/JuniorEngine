/*
* Author: David Wong
* Email: david.wongcascante@digipen.edu
* File name: GameObjectManager.cpp
* Description: Takes care to initialize, update, and clean up all of the objects
* Created: 11 Dec 2018
* Last Modified: 10 Sep 2019
*/

// Includes
#include "GameObjectManager.h"			// Game Object Manager
#include "GameObject.h"					// Game Object
#include "Time.h"						// Time


Junior::GameObjectManager::GameObjectManager()
	: GameSystem("GameObjectManager"), fixedDtTarget_(1.0/60.0), fixedDtAccumulator_(0.0f), gameObjects_(), archetypes_(), destroyedObjects_()
{
}

bool Junior::GameObjectManager::Load()
{
	return true;
}

bool Junior::GameObjectManager::Initialize()
{
	return true;
}

void Junior::GameObjectManager::Render()
{

}

void Junior::GameObjectManager::Update(double dt)
{
	for (GameObject* gameObject : gameObjects_)
	{
		// First check if the object is destroyed
		if (gameObject->IsDestroyed())
		{
			// If the object is destroyed then put it inside the destroyed tab and not update it
			destroyedObjects_.push_back(gameObject);
			continue;
		}
		gameObject->Update(Time::GetInstance().GetDeltaTime());
	}

	// Perform a fixed update for every
	fixedDtAccumulator_ += Time::GetInstance().GetDeltaTime();
	while (fixedDtAccumulator_ >= fixedDtTarget_)
	{
		fixedDtAccumulator_ -= fixedDtTarget_;
		for (GameObject* gameObject : gameObjects_)
		{
			gameObject->FixedUpdate(fixedDtTarget_);
		}
	}

	// Check if there are any objects that are destroyed
	if (destroyedObjects_.size() > 0)
	{
		// Then destroy the objects and remove them from the original list
		for (GameObject* object : destroyedObjects_)
		{
			for (unsigned i = 0; i < gameObjects_.size(); ++i)
			{
				if (object == gameObjects_[i])
				{
					gameObjects_.erase(gameObjects_.begin()+i);
				}
			}

			// Clean up the destroyed object
			object->Shutdown();
			object->Unload();
			delete object;
		}
		destroyedObjects_.clear();
	}
}

void Junior::GameObjectManager::Shutdown()
{
	for (GameObject* gameObject : gameObjects_)
	{
		gameObject->Shutdown();
	}

	for (GameObject* archetype : archetypes_)
	{
		archetype->Shutdown();
	}
}

void Junior::GameObjectManager::Unload()
{
	for (GameObject* gameObject : gameObjects_)
	{
		gameObject->Unload();
		delete gameObject;
	}

	for (GameObject* archetype : archetypes_)
	{
		archetype->Unload();
		delete archetype;
	}

	gameObjects_.clear();
	archetypes_.clear();
}

void Junior::GameObjectManager::AddObject(GameObject* const object)
{
	object->Initialize();
	gameObjects_.push_back(object);
}

void Junior::GameObjectManager::AddArchetype(GameObject* const archetype)
{
	archetype->Initialize();
	archetypes_.push_back(archetype);
}

Junior::GameObject* Junior::GameObjectManager::FindByName(const std::string& name) const
{
	for (auto begin = gameObjects_.cbegin(); begin != gameObjects_.cend(); ++begin)
	{
		if ((*begin)->GetName() == name)
		{
			return *begin;
		}
	}

	return nullptr;
}

Junior::GameObject* Junior::GameObjectManager::CreateFromArchetype(const std::string& name) const
{
	for (auto begin = archetypes_.cbegin(); begin != archetypes_.cend(); ++begin)
	{
		if ((*begin)->GetName() == name)
		{
			return new GameObject(**begin);
		}
	}

	return nullptr;
}

	
const std::vector<Junior::GameObject*>& Junior::GameObjectManager::GetAllObjects() const
{
	return gameObjects_;
}

Junior::GameObjectManager& Junior::GameObjectManager::GetInstance()
{
	static GameObjectManager singleton_;
	return singleton_;
}