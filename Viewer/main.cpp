#include <cstdio>
#include <chrono>
#include <thread>
#include "BaseApplication.hpp"
#include "AssetLoader.hpp"
#include "SceneManager.hpp"
#include "GraphicsManager.hpp"
#include "PhysicsManager.hpp"
#include "MemoryManager.hpp"
#include "MyViewer.hpp"

using namespace My;
#define WINPIX_DEBUG
#ifdef WINPIX_DEBUG
#include <filesystem>
#include <ShlObj.h>
static std::wstring GetLatestWinPixGpuCapturerPath_Cpp17()
{
	LPWSTR programFilesPath = nullptr;
	SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, NULL, &programFilesPath);

	std::filesystem::path pixInstallationPath = programFilesPath;
	pixInstallationPath /= "Microsoft PIX";

	std::wstring newestVersionFound;

	for (auto const& directory_entry : std::filesystem::directory_iterator(pixInstallationPath))
	{
		if (directory_entry.is_directory())
		{
			if (newestVersionFound.empty() || newestVersionFound < directory_entry.path().filename().c_str())
			{
				newestVersionFound = directory_entry.path().filename().c_str();
			}
		}
	}

	if (newestVersionFound.empty())
	{
		// TODO: Error, no PIX installation found
	}

	return pixInstallationPath / newestVersionFound / L"WinPixGpuCapturer.dll";
}

#endif // WINPIX_DEBUG



#ifdef USING_DX12
#include "D3d12Application.hpp"
#include "D3d/D3d12Config.hpp"
#elif USING_OPENGL

#elif USING_METAL

#elif USING_OPENGLES

#elif USING_VULKAN

#endif // USING_DX12


int main(int argc, char** argv) {
#ifdef WINPIX_DEBUG
	if (GetModuleHandle((LPCSTR)L"WinPixGpuCapturer.dll") == 0)
	{
		LoadLibrary((LPCSTR)GetLatestWinPixGpuCapturerPath_Cpp17().c_str());
	}
#endif
	int ret;
	GfxConfiguration config(8, 8, 8, 8, 24, 0, 0, 960, 540);

	D3d12Application app(config);

	app.SetCommandLineParameters(argc, argv);

	MyViewerLogic gameLogic;

	AssetLoader assetLoader;
	TGraphicsManager graphicsManager;
	MemoryManager memoryManager;
	SceneManager sceneManager;
	PhysicsManager physicsManager;
	InputManager inputManager;

	app.RegisterManagerModule(&gameLogic);
	app.RegisterManagerModule(&assetLoader);
	app.RegisterManagerModule(&graphicsManager);
	app.RegisterManagerModule(&inputManager);
	app.RegisterManagerModule(&memoryManager);
	app.RegisterManagerModule(&physicsManager);
	app.RegisterManagerModule(&sceneManager);

	app.CreateMainWindow();
	app.Initialize();

	while (!app.IsQuit()) {
		app.Tick();
	}

	app.Finalize();

	return 0;
}
