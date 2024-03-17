#include "InputManager.hpp"
#include "GraphicsManager.hpp"
#include "BaseApplication.hpp"

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
		IGraphicsManager* pGraphicsManager = reinterpret_cast<BaseApplication*>(m_pApp)->GetGraphicsManager();
		pGraphicsManager->UpArrowKeyDown();
	}

	void InputManager::UpArrowKeyUp()
	{

	}

	void InputManager::DownArrowKeyDown()
	{
		IGraphicsManager* pGraphicsManager = reinterpret_cast<BaseApplication*>(m_pApp)->GetGraphicsManager();
		pGraphicsManager->DownArrowKeyDown();
	}

	void InputManager::DownArrowKeyUp()
	{

	}

	void InputManager::LeftArrowKeyDown()
	{
		IGraphicsManager* pGraphicsManager = reinterpret_cast<BaseApplication*>(m_pApp)->GetGraphicsManager();
		pGraphicsManager->LeftArrowKeyDown();
	}

	void InputManager::LeftArrowKeyUp()
	{

	}

	void InputManager::RightArrowKeyDown()
	{
		IGraphicsManager* pGraphicsManager = reinterpret_cast<BaseApplication*>(m_pApp)->GetGraphicsManager();
		pGraphicsManager->RightArrowKeyDown();
	}

	void InputManager::RightArrowKeyUp()
	{

	}

	void InputManager::NumPadKeyDown(long long key)
	{
		IGraphicsManager* pGraphicsManager = reinterpret_cast<BaseApplication*>(m_pApp)->GetGraphicsManager();
		pGraphicsManager->NumPadKeyDown(key);
	}

	void InputManager::NumPadKeyUp(long long key)
	{
		IGraphicsManager* pGraphicsManager = reinterpret_cast<BaseApplication*>(m_pApp)->GetGraphicsManager();
		pGraphicsManager->NumPadKeyUp(key);
	}

	void InputManager::FunctionKeyDown(long long key)
	{
		IGraphicsManager* pGraphicsManager = reinterpret_cast<BaseApplication*>(m_pApp)->GetGraphicsManager();
		pGraphicsManager->FunctionKeyDown(key);
	}

	void InputManager::FunctionKeyUp(long long key)
	{
		IGraphicsManager* pGraphicsManager = reinterpret_cast<BaseApplication*>(m_pApp)->GetGraphicsManager();
		pGraphicsManager->FunctionKeyUp(key);
	}
}