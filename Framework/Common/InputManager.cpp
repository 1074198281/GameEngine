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
		g_pGraphicsManager->MoveCameraYPositive();
	}

	void InputManager::UpArrowKeyUp()
	{

	}

	void InputManager::DownArrowKeyDown()
	{
		g_pGraphicsManager->MoveCameraYNegative();
	}

	void InputManager::DownArrowKeyUp()
	{

	}

	void InputManager::LeftArrowKeyDown()
	{
		g_pGraphicsManager->MoveCameraXNegative();
	}

	void InputManager::LeftArrowKeyUp()
	{

	}

	void InputManager::RightArrowKeyDown()
	{
		g_pGraphicsManager->MoveCameraXPositive();
	}

	void InputManager::RightArrowKeyUp()
	{

	}
}