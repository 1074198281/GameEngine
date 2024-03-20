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
#include "D3d12Application.hpp"
#include "D3d/D3d12Config.hpp"

using namespace My;

int main(int argc, char** argv) {
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
