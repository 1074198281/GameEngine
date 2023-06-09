#include "SceneManager.hpp"
#include "AssetLoader.hpp"
#include "../Framework/Parser/OGEX.hpp"
#include "../Framework/Parser/GLTF.hpp"
#include "D3d/Core/Common/Utility.h"
#include <filesystem>

using namespace My;


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
    std::string extension = scene.substr(pos + 1);
    if (extension == "gltf") {
        LoadglTFScene(scene_file_name);
    }
    else if (extension == "ogex") {
        LoadOgexScene(scene_file_name);
    }
}

void SceneManager::LoadOgexScene(const char* ogex_scene_file_name)
{
    std::string ogex_text = g_pAssetLoader->SyncOpenAndReadTextFileToString(ogex_scene_file_name);

    OgexParser ogex_parser;
    m_pScene = ogex_parser.Parse(ogex_text);
}

void SceneManager::LoadglTFScene(const char* glTF_scene_file_name)
{
    std::string glTF_text = g_pAssetLoader->SyncOpenAndReadTextFileToString(glTF_scene_file_name);
    
    std::filesystem::path currentpath = std::filesystem::current_path();
    std::wstring filepath = currentpath.wstring() + Utility::UTF8ToWideString(std::string(glTF_scene_file_name));

    glTF::GLTFParser glTF_parser;
    glTF_parser.Parse(Utility::UTF8ToWideString(g_pAssetLoader->m_AssetPath));
}

const Scene& SceneManager::GetSceneForRendering()
{
    return *m_pScene;
}
