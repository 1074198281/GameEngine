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

// Parse command line, read configuration, initialize all sub modules
int My::BaseApplication::Initialize()
{
    int ret = 0;

    std::cout << m_Config;

	std::cerr << "Initialize Memory Manager: ";
	if ((ret = g_pMemoryManager->Initialize()) != 0) {
		//printf("Memory Manager Initialize failed, will exit now.");
		std::cerr << "Failed. err = " << ret;
		return ret;
	}
	std::cerr << "Success" << std::endl;

	std::cerr << "Initialize Asset Loader: ";
	if ((ret = g_pAssetLoader->Initialize()) != 0) {
		printf("Asset Loader Initialize failed, will exit now.");
		std::cerr << "Failed. err = " << ret;
		return ret;
	}
	std::cerr << "Success" << std::endl;

	std::cerr << "Initialize Graphics Manager: ";
	if ((ret = g_pSceneManager->Initialize()) != 0) {
		printf("Scene Manager Initialize failed, will exit now.");
		std::cerr << "Failed. err = " << ret;
		return ret;
	}
	std::cerr << "Success" << std::endl;

	std::cerr << "Initialize Graphics Manager: ";
	if ((ret = g_pGraphicsManager->Initialize()) != 0) {
		std::cerr << "Failed. err = " << ret;
		return ret;
	}
	std::cerr << "Success" << std::endl;

	std::cerr << "Initialize Input Manager: ";
	if ((ret = g_pInputManager->Initialize()) != 0) {
		std::cerr << "Failed. err = " << ret;
		return ret;
	}
	std::cerr << "Success" << std::endl;

	std::cerr << "Initialize Physics Manager: ";
	if ((ret = g_pPhysicsManager->Initialize()) != 0) {
		std::cerr << "Failed. err = " << ret;
		return ret;
	}
	std::cerr << "Success" << std::endl;

    return ret;
}

// Finalize all sub modules and clean up all runtime temporary files.
void My::BaseApplication::Finalize()
{
	g_pInputManager->Finalize();
	g_pGraphicsManager->Finalize();
	g_pPhysicsManager->Finalize();
	g_pSceneManager->Finalize();
	g_pAssetLoader->Finalize();
	g_pMemoryManager->Finalize();
}

// One cycle of the main loop
void My::BaseApplication::Tick()
{
	g_pInputManager->Tick();
	g_pGraphicsManager->Tick();
	g_pPhysicsManager->Tick();
	g_pSceneManager->Tick();
	g_pAssetLoader->Tick();
	g_pMemoryManager->Tick();
}

bool My::BaseApplication::IsQuit()
{
    return m_bQuit;
}

void My::BaseApplication::SetCommandLineParameters(int argc, char** argv)
{
	m_nArgC = argc;
	m_ppArgV = argv;
}

int My::BaseApplication::LoadScene()
{
	//g_pSceneManager->LoadScene("Scene/OpenGEX/Example.ogex");
	//g_pSceneManager->LoadScene("Scene/glTF/Cube/glTF/Cube.gltf");
	//g_pSceneManager->LoadScene("Scene/glTF/Sponza/glTF/Sponza.gltf");
	g_pSceneManager->LoadScene("Scene/glTF/ToyCar/glTF/ToyCar.gltf");
	//g_pSceneManager->LoadScene("Scene/glTF/MetalRoughSpheresOut/glTF/MetalRoughSpheres.gltf");
	//g_pSceneManager->LoadScene("Scene/glTF/Sponza/Sponza.gltf");
	//g_pSceneManager->LoadScene("Scene/glTF/ABeautifulGame/glTF/ABeautifulGame.gltf");		//这个需要处理一下jpeg算法，DCT和IDCT离散余弦变换和逆离散余弦变换
	//g_pSceneManager->LoadScene("Scene/glTF/BoomBoxWithAxes/glTF/BoomBoxWithAxes.gltf");
	//g_pSceneManager->LoadScene("Scene/NC/tar.txt");

	return 0;
}
