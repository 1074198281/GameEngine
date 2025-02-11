#include "MyViewer.hpp"
#include "BaseApplication.hpp"

int My::MyViewerLogic::Initialize()
{
	int result = 0;

	std::cout << "[AppInitialize] key help: speed up: page up, speed down: page down. R to reset." << std::endl;

	ExecuteCommandLine();

	return result;
}

void My::MyViewerLogic::Finalize()
{

}

void My::MyViewerLogic::Tick()
{

}

int My::MyViewerLogic::ExecuteCommandLine()
{
	int cmdCount = dynamic_cast<BaseApplication*>(m_pApp)->GetCommandLineParametersCount();
	ISceneManager* pSceneManager = dynamic_cast<BaseApplication*>(m_pApp)->GetSceneManager();
	if (cmdCount > 1) {

	}
	else {
		// default loading settings
		char directory[256];
		strcpy(directory, _ASSET_RESOURCE_DIRECTORY);
		int loadIndex = -1;
		std::vector<std::string> filePath;
		filePath.push_back("Scene/glTF/ToyCar/glTF/ToyCar.gltf");
		filePath.push_back("Scene/glTF/Box/glTF/box.gltf");
		filePath.push_back("Scene/glTF/LightTest/LightTest.gltf");
		filePath.push_back("Scene/glTF/ray_tracing/ray_tracing.gltf");

		//const char* ScenePath = "Scene/glTF/ToyCar/glTF/ToyCar.gltf";
		//const char* ScenePath = "Scene/glTF/Box/glTF/box.gltf";
		//const char* ScenePath = "Scene/glTF/LightTest/LightTest.gltf";
		//const char* ScenePath = "Scene/glTF/ray_tracing/ray_tracing.gltf";
		//char* default_loading = strcat(directory, "/Scene/glTF/ToyCar/glTF/ToyCar.gltf");
		std::cout << "[Scene Select] 0. toycar   1.box   2.lighttest   3.ray_tracing" << std::endl;
		std::cout << "[Scene Select] Select Load Index: ";
		std::cin >> loadIndex;
		if (loadIndex < 0) {

		} else {
			pSceneManager->LoadScene(filePath[loadIndex].c_str());
		}
	}
	return 0;
}