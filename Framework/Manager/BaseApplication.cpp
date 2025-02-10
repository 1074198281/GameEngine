#include "PhysicsManager.hpp"
#include "InputManager.hpp"
#include "BaseApplication.hpp"

#include <iostream>

using namespace My;

bool My::BaseApplication::m_bQuit = false;

My::BaseApplication::BaseApplication(GfxConfiguration& cfg)
    :m_Config(cfg)
{
}

// initialize all sub modules
int My::BaseApplication::Initialize()
{
    int ret = 0;

    std::cout << m_Config;

	for (auto& mgr : m_RuntimeModule) {
		int ret = mgr->Initialize();
		if (ret > 0) {
			printf("[RuntimeModuleInit] Initialize RuntimeModule Failed!");
			break;
		}
	}

    return ret;
}

// Finalize all sub modules and clean up all runtime temporary files.
void My::BaseApplication::Finalize()
{
	for (auto& mgr : m_RuntimeModule) {
		mgr->Finalize();
	}
}

// One cycle of the main loop
void My::BaseApplication::Tick()
{
	for (auto& mgr : m_RuntimeModule) {
		mgr->Tick();
	}
}

bool My::BaseApplication::IsQuit()
{
    return m_bQuit;
}

// Parse command line, read configuration, 
void My::BaseApplication::SetCommandLineParameters(int argc, char** argv)
{
	m_nArgC = argc;
	m_ppArgV = argv;
}

int My::BaseApplication::GetCommandLineParametersCount()
{
	return m_nArgC;
}

const char* My::BaseApplication::GetCommandLineParameters(int index)
{
	return m_ppArgV[index];
}

void My::BaseApplication::RegisterManagerModule(IGraphicsManager* mgr)
{
	m_pGraphicsManager = mgr;
	mgr->SetAppPtr(this);
	m_RuntimeModule.push_back(mgr);
}

void My::BaseApplication::RegisterManagerModule(IMemoryManager* mgr)
{
	m_pMemoryManager = mgr;
	mgr->SetAppPtr(this);
	m_RuntimeModule.push_back(mgr);
}

void My::BaseApplication::RegisterManagerModule(IAssetLoader* mgr)
{
	m_pAssetLoader = mgr;
	mgr->SetAppPtr(this);
	m_RuntimeModule.push_back(mgr);
}

void My::BaseApplication::RegisterManagerModule(IInputManager* mgr)
{
	m_pInputManager = mgr;
	mgr->SetAppPtr(this);
	m_RuntimeModule.push_back(mgr);
}

void My::BaseApplication::RegisterManagerModule(ISceneManager* mgr)
{
	m_pSceneManager = mgr;
	mgr->SetAppPtr(this);
	m_RuntimeModule.push_back(mgr);
}

void My::BaseApplication::RegisterManagerModule(IPhysicsManager* mgr)
{
	m_pPhysicsManager = mgr;
	mgr->SetAppPtr(this);
	m_RuntimeModule.push_back(mgr);
}

void My::BaseApplication::RegisterManagerModule(IGameLogic* mgr)
{
	m_pGameLogicManager = mgr;
	mgr->SetAppPtr(this);
	m_RuntimeModule.push_back(mgr);
}