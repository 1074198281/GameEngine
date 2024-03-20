#include "MyViewer.hpp"
#include "BaseApplication.hpp"

int My::MyViewerLogic::Initialize()
{
	int result = 0;

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
		char* default_loading = strcat(directory, "/Scene/glTF/ToyCar/glTF/ToyCar.gltf");
		pSceneManager->LoadScene("Scene/glTF/ToyCar/glTF/ToyCar.gltf");
	}
	return 0;
}