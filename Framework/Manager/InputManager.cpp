#include "InputManager.hpp"
#include "GraphicsManager.hpp"

namespace My {
	int InputManager::Initialize()
	{
		return 0;
	}

	void InputManager::Finalize()
	{

	}

	void InputManager::Tick()
	{

	}

	void InputManager::UpArrowKeyDown()
	{
		g_pGraphicsManager->UpArrowKeyDown();
	}

	void InputManager::UpArrowKeyUp()
	{

	}

	void InputManager::DownArrowKeyDown()
	{
		g_pGraphicsManager->DownArrowKeyDown();
	}

	void InputManager::DownArrowKeyUp()
	{

	}

	void InputManager::LeftArrowKeyDown()
	{
		g_pGraphicsManager->LeftArrowKeyDown();
	}

	void InputManager::LeftArrowKeyUp()
	{

	}

	void InputManager::RightArrowKeyDown()
	{
		g_pGraphicsManager->RightArrowKeyDown();
	}

	void InputManager::RightArrowKeyUp()
	{

	}

	void InputManager::NumPadKeyDown(long long key)
	{
		g_pGraphicsManager->NumPadKeyDown(key);
	}

	void InputManager::NumPadKeyUp(long long key)
	{
		g_pGraphicsManager->NumPadKeyUp(key);
	}

	void InputManager::FunctionKeyDown(long long key)
	{
		g_pGraphicsManager->FunctionKeyDown(key);
	}

	void InputManager::FunctionKeyUp(long long key)
	{
		g_pGraphicsManager->FunctionKeyUp(key);
	}
}