#include "SceneManager.hpp"
#include "AssetLoader.hpp"
#include "../Framework/Parser/OGEX.hpp"

using namespace My;
using namespace std;

SceneManager::~SceneManager()
{
}

int SceneManager::Initialize()
{
    int result = 0;
    return result;
}

void SceneManager::Finalize()
{
}

void SceneManager::Tick()
{
}

void SceneManager::LoadScene(const char* scene_file_name)
{
    // now we only has ogex scene parser, call it directly
    std::string scene(scene_file_name);
    size_t pos = scene.find_last_of('.');
    std:string extension = scene.substr(pos + 1);
    if (extension == "gltf") {
        LoadglTFScene(scene_file_name);
    }
    else if (extension == "ogex") {
        LoadOgexScene(scene_file_name);
    }
}

void SceneManager::LoadOgexScene(const char* ogex_scene_file_name)
{
    string ogex_text = g_pAssetLoader->SyncOpenAndReadTextFileToString(ogex_scene_file_name);

    OgexParser ogex_parser;
    m_pScene = ogex_parser.Parse(ogex_text);
}

void SceneManager::LoadglTFScene(const char* glTF_scene_file_name)
{
    string glTF_text = g_pAssetLoader->SyncOpenAndReadTextFileToString(glTF_scene_file_name);

}

const Scene& SceneManager::GetSceneForRendering()
{
    return *m_pScene;
}
