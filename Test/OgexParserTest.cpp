#include <iostream>
#include <string>
#include "AssetLoader.hpp"
#include "MemoryManager.hpp"
#include "../Framework/Parser/OGEX.hpp"

using namespace My;

namespace My {
    MemoryManager* g_pMemoryManager = new MemoryManager();
    AssetLoader* g_pAssetLoader = new AssetLoader();
}

template<typename T>
static std::ostream& operator<<(std::ostream& out, std::unordered_map<std::string, std::shared_ptr<T>> map)
{
    for (auto p : map)
    {
        out << *p.second << std::endl;
    }

    return out;
}

int main(int, char**)
{
    g_pMemoryManager->Initialize();
    g_pAssetLoader->Initialize();

    std::string ogex_text = g_pAssetLoader->SyncOpenAndReadTextFileToString("Scene/Example.ogex");

    OgexParser* ogex_parser = new OgexParser();
    std::unique_ptr<Scene> pScene = ogex_parser->Parse(ogex_text);
    delete ogex_parser;

    std::cout << "Dump of Scene Graph" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << *pScene->SceneGraph << std::endl;

    std::cout << "Dump of Cameras" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << pScene->Cameras << std::endl;

    std::cout << "Dump of Lights" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << pScene->Lights << std::endl;

    std::cout << "Dump of Geometries" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << pScene->Geometries << std::endl;

    std::cout << "Dump of Materials" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << pScene->Materials << std::endl;

    g_pAssetLoader->Finalize();
    g_pMemoryManager->Finalize();

    delete g_pAssetLoader;
    delete g_pMemoryManager;

    return 0;
}
