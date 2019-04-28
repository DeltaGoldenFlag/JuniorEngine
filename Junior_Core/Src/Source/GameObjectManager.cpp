/*
* Author: David Wong
* Email: david.wongcascante@digipen.edu
* File name: GameObjectManager.cpp
* Description: Takes care to initialize, update, and clean up all of the objects
* Created: 11 Dec 2018
* Last Modified: 27 Apr 2019
*/

// Includes
#include "GameObjectManager.h"			// Game Object Manager
#include "GameObject.h"					// Game Object
#include "MemoryManager.h"				// Memory Manager
#include "Time.h"						// Time

namespace Junior
{
	GameObjectManager::GameObjectManager()
		: gameObjects_(), destroyedObjects_()
	{
	}

	void GameObjectManager::AddObject(GameObject* const object)
	{
		object->Initialize();
		gameObjects_.push_back(object);
	}

	GameObject* GameObjectManager::FindByName(const std::string& name) const
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

	void GameObjectManager::Update(MemoryManager* manager)
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
				object->Unload();
				delete object;
			}
			destroyedObjects_.clear();
		}
	}

	void GameObjectManager::CleanUp(MemoryManager* manager)
	{
		for (GameObject* gameObject : gameObjects_)
		{
			gameObject->Unload();
			delete gameObject;
		}
	}
	
	const std::vector<GameObject*>& Junior::GameObjectManager::GetAllObjects() const
	{
		return gameObjects_;
	}

	GameObjectManager& Junior::GameObjectManager::GetInstance()
	{
		// Private Static Variables //
		static GameObjectManager singleton_;
		return singleton_;
	}
}