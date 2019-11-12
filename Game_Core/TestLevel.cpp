/*
* Author: David Wong
* Email: david.wongcascante@digipen.edu
* File name: TestLevel.cpp
* Description: Test level for object management
* Created: 20 Dec 2018
* Last Modified: 2 Oct 2019
*/

// Includes
#include "TestLevel.h"

#ifdef _DEBUG
#include "MemoryLeakGuard.h"		// Memory Leak Guard
#endif

#include <EventManager.h>			// Event Manager
#include <GameObjectManager.h>		// Game Object Manager
#include <GameObjectFactory.h>		// Game Object Factory
#include <Space.h>					// Space
#include <GameObject.h>				// Game Object
#include <Time.h>					// Time
#include <Input.h>					// Input
#include <Physics.h>				// Physics
#include <Transform.h>				// Transform
#include <Graphics.h>				// Graphics
#include <RenderJob.h>				// RenderJob
#include <Sprite.h>					// Sprite
#include <Animator.h>				// Animator
#include <Camera.h>					// Camera

#include <Debug.h>		// Debug

// Testing event calling
// Params:
//	object: The TestLevel instance
//	event: The event data for the resize
void WindowResizeCallback(void* object, const Junior::Event* event);
// Reset the level when pressing the F5 key
// Params:
//	object: A TestLevel instance
//	event: The event data for the key input
void ResetTestLevel(void* object, const Junior::Event* event);

JuniorGame::TestLevel::TestLevel()
	: Level("TestLevel"), cog_(nullptr), cog2_(nullptr), transform_(nullptr), transform2_(nullptr), timer_(0.0), deletedObject2_(false),
	  cameraTransform_(nullptr), camera_(nullptr), playerPhysics_(nullptr), restart_(false)
{
}

bool JuniorGame::TestLevel::Load()
{
	return true;
}

bool JuniorGame::TestLevel::Initialize()
{
	Junior::GameObjectFactory::GetInstance().FillLevel("TestLevel", this);

	camera_ = owner_->GetObjectManager()->FindByName("Camera");
	cog_ = owner_->GetObjectManager()->FindByName("Player");
	cog2_ = owner_->GetObjectManager()->FindByName("Second_New_Object");

	cog_->AddChild(owner_->GetObjectManager()->FindByName("Particle_Effect"));

	Junior::Camera* cameraComponent = camera_->GetComponent<Junior::Camera>();
	Junior::Graphics::GetInstance().mainCamera_ = cameraComponent;

	transform_ = cog_->GetComponent<Junior::Transform>();
	transform2_ = cog2_->GetComponent<Junior::Transform>();
	cameraTransform_ = camera_->GetComponent<Junior::Transform>();

	playerPhysics_ = cog_->GetComponent<Junior::Physics>();

	Junior::EventManager& manager = Junior::EventManager::GetInstance();
	manager.Subscribe(Junior::WindowResizeEvent::WindowResizeEventName, this, WindowResizeCallback);
	manager.Subscribe(Junior::KeyEvent::KeyEventName, this, ResetTestLevel);

	return true;
}

void JuniorGame::TestLevel::Update(double dt)
{
	transform2_->SetLocalTranslation(
		Junior::Vec3(300.0f * cosf(static_cast<float>(Junior::Time::GetInstance().GetTimeRan())), -300.0f,
					sinf(static_cast<float>(Junior::Time::GetInstance().GetTimeRan())))
	);

	// Slowly move the camera toward the player
	Junior::Vec3 toPlayer = transform_->GetLocalTranslation() - cameraTransform_->GetLocalTranslation();
	cameraTransform_->SetLocalTranslation(cameraTransform_->GetLocalTranslation() + toPlayer * 0.01f);

	if (restart_)
	{
		owner_->RestartLevel();
	}
}

void JuniorGame::TestLevel::Shutdown()
{
	Junior::EventManager& manager = Junior::EventManager::GetInstance();
	manager.Unsubscribe(Junior::WindowResizeEvent::WindowResizeEventName, this, WindowResizeCallback);
	manager.Unsubscribe(Junior::KeyEvent::KeyEventName, this, ResetTestLevel);
}

void JuniorGame::TestLevel::Unload()
{
}

// Testing the event system
// Params:
//	object: The object hooked to this callback (should be an instance of TestLevel)
//	event: The event that fired this callback
void WindowResizeCallback(void* object, const Junior::Event* event)
{
	const Junior::WindowResizeEvent* windowEvent = reinterpret_cast<const Junior::WindowResizeEvent*>(event);
	Junior::Debug::GetInstance().Print("New Window Dimensions: [");
	Junior::Debug::GetInstance().Print(windowEvent->newWidth_);
	Junior::Debug::GetInstance().Print(", ");
	Junior::Debug::GetInstance().Print(windowEvent->newHeight_);
	Junior::Debug::GetInstance().PrintLn("]");
}
// Reset the level when pressing the F5 key
// Params:
//	object: A TestLevel instance
//	event: The event data for the key input
void ResetTestLevel(void* object, const Junior::Event* event)
{
	const Junior::KeyEvent* keyEvent = reinterpret_cast<const Junior::KeyEvent*>(event);
	if (keyEvent->action_ == GLFW_PRESS && keyEvent->key_ == GLFW_KEY_F5)
	{
		JuniorGame::TestLevel* testLevel = reinterpret_cast<JuniorGame::TestLevel*>(object);
		if (testLevel)
		{
			testLevel->GetOwner()->RestartLevel();
		}
	}
}