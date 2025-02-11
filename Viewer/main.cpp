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
#include "pix3.h"
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

#include <windows.h>
#include <strsafe.h>
void ErrorExit(LPCTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	//MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	std::cout << "[PixDebugError] " << (LPCTSTR)lpDisplayBuf << std::endl;

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	//ExitProcess(dw);
}

#endif // WINPIX_DEBUG


// Graphics Interface
#ifdef USING_DX12
#include "D3d12Application.hpp"
#include "D3d/D3d12Config.hpp"
#elif USING_OPENGL

#elif USING_METAL

#elif USING_OPENGLES

#elif USING_VULKAN

#endif // USING_DX12

#ifdef USING_PHYSICS_PHYSX
#include "PhysXPhysicsManager.hpp"
#elif USING_PHYSICS_BULLET3
#include "Bullet3PhysicsManager.hpp"
#elif USING_PHYSICS_MY

#endif


int main(int argc, char** argv) {
#ifdef WINPIX_DEBUG
	auto current_path = std::filesystem::current_path();
	auto runtimePath = current_path / L"Debug\\WinPixEventRuntime.dll";
	auto capturerPath = current_path / L"Debug\\WinPixGpuCapturer.dll";
	LoadLibrary(TEXT((LPCSTR)runtimePath.c_str()));
	ErrorExit(TEXT("LoadDLLRuntime"));
	LoadLibrary(TEXT((LPCSTR)capturerPath.c_str()));
	ErrorExit(TEXT("LoadDLLCapturer"));

	HMODULE hModule = GetModuleHandle((LPCSTR)L"WinPixEventRuntime.dll");
	//ErrorExit(TEXT("GetRuntimeModule"));

	hModule = GetModuleHandle((LPCSTR)L"WinPixGpuCapturer.dll");
	if (hModule == 0)
	{
		ErrorExit(TEXT("GetCapturerModule"));

		if (!LoadLibrary((LPCSTR)GetLatestWinPixGpuCapturerPath_Cpp17().c_str())) {
			ErrorExit(TEXT("GetBaseModule"));
		}
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
	TPhysicsManager physicsManager;
	InputManager inputManager;

	app.RegisterManagerModule(&gameLogic);
	app.RegisterManagerModule(&assetLoader);
	app.RegisterManagerModule(&sceneManager);
	app.RegisterManagerModule(&inputManager);
	app.RegisterManagerModule(&physicsManager);
	app.RegisterManagerModule(&graphicsManager);
	app.RegisterManagerModule(&memoryManager);

	app.CreateMainWindow();
	app.Initialize();

	while (!app.IsQuit()) {
		app.Tick();
	}

	app.Finalize();

	return 0;
}
