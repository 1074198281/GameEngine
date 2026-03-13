#include "BaseApplication.hpp"
#include "SceneManager.hpp"
#include "AssetLoader.hpp"
#include "../Framework/Parser/OGEX.hpp"
#include "../Framework/Parser/GLTF.hpp"
#include "utility.hpp"
#include "D3d12Application.hpp"
#include <filesystem>
#include <assert.h>


#include "../PCG/Waves.h"

using namespace My;


int SceneManager::Initialize()
{
    int result = 0;
    m_iObjectId = -1;
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
    std::string extension = GetFileExtension(scene_file_name);
    if (extension == "gltf") {
        LoadglTFScene(scene_file_name);
    }
    else if (extension == "ogex") {
        LoadOgexScene(scene_file_name);
    }
    else if (!strcmp(scene_file_name, "Empty"))
    {
        m_pScene = std::make_unique<Scene>();
        m_bEmptyScene = true;
    }
    else {
        printf("Invalid Or Unsupported File Extension!");
        assert(false);
        return -1;
    }

#ifdef _WIN32
    dynamic_cast<D3d12Application*>(m_pApp)->LoadSceneResource(*m_pScene);
#elif _LINUX

#elif _METAL

#else
    m_pScene->LoadResource();
#endif // _WIN32

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

Scene* SceneManager::GetSceneForGui()
{
    return m_pScene.get();
}

uint64_t SceneManager::GetSceneRevision() 
{
    return m_nSceneRevision;
}

void SceneManager::ResetScene()
{
    m_nSceneRevision++;
}

bool SceneManager::IsSceneEmpty()
{
    return m_bEmptyScene;
}

void SceneManager::AddWaves(float x, float y, float centerX, float centerY, float centerZ,
    float stepX, float stepY, float length, float amplitude, float speed)
{
    m_iObjectId++;
    std::unique_ptr<Waves> pWaves = std::make_unique<Waves>("Waves");
    pWaves->SetWavesParam(x, y, centerX, centerY, centerZ, stepX, stepY, length, amplitude, speed, 0);

    m_pScene->Geometries[pWaves->m_Name] = pWaves->GetGeoObject();
    m_pScene->GeometryNodes.emplace(pWaves->m_Name, pWaves->GetGeoNode());

    m_ScenePCGObjects.emplace(m_iObjectId, std::move(pWaves));
}
