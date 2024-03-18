#include "BaseApplication.hpp"
#include "SceneManager.hpp"
#include "AssetLoader.hpp"
#include "../Framework/Parser/OGEX.hpp"
#include "../Framework/Parser/GLTF.hpp"
#include <filesystem>
#include <assert.h>

using namespace My;


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

int SceneManager::LoadScene(const char* scene_file_name)
{
    // now we only has ogex scene parser, call it directly
    std::string scene(scene_file_name);
    size_t pos = scene.find_last_of('.');
    std::string extension = scene.substr(pos + 1);
    if (extension == "gltf") {
        LoadglTFScene(scene_file_name);
        m_pScene->LoadResource();
    }
    else if (extension == "ogex") {
        LoadOgexScene(scene_file_name);
        m_pScene->LoadResource();
    }
    else {
        printf("Invalid Or Unsupported File Extension!");
        assert(false);
        return -1;
    }

    m_nSceneRevision++;

    return 0;
}

void SceneManager::LoadOgexScene(const char* ogex_scene_file_name)
{
    IAssetLoader* pAssetLoad = reinterpret_cast<BaseApplication*>(m_pApp)->GetAssetLoader();
    std::string ogex_text = pAssetLoad->SyncOpenAndReadTextFileToString(ogex_scene_file_name);
    std::string assetName = std::string(ogex_scene_file_name);

    OgexParser ogex_parser;
    m_pScene = ogex_parser.Parse(ogex_text, ogex_scene_file_name);

    if (!m_pScene) {
        assert(false);
        printf("Scene Is Empty!");
    }
}

void SceneManager::LoadglTFScene(const char* glTF_scene_file_name)
{
    IAssetLoader* pAssetLoad = reinterpret_cast<BaseApplication*>(m_pApp)->GetAssetLoader();
    std::string glTF_text = pAssetLoad->SyncOpenAndReadTextFileToString(glTF_scene_file_name);
    std::string assetName = std::string(glTF_scene_file_name);

    glTF::GLTFParser glTF_parser;
    m_pScene = glTF_parser.Parse(glTF_scene_file_name, glTF_scene_file_name);

    if (!m_pScene) {
        assert(false);
        printf("Scene Is Empty!");
    }
}

const Scene& SceneManager::GetSceneForRendering()
{
    return *m_pScene;
}

uint64_t SceneManager::GetSceneRevision() 
{
    return m_nSceneRevision;
}

void SceneManager::ResetScene()
{
    m_nSceneRevision++;
}
