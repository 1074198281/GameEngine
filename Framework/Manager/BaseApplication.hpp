#pragma once
#include "IApplication.hpp"
#include "GraphicsManager.hpp"
#include "MemoryManager.hpp"
#include "AssetLoader.hpp"
#include "SceneManager.hpp"
#include "InputManager.hpp"
#include "PhysicsManager.hpp"
#include "MyViewer.hpp"

namespace My {
    class BaseApplication : __implements IApplication
    {
    public:
        BaseApplication() = default;
        explicit BaseApplication(GfxConfiguration& cfg);
        int Initialize() override;
        void Finalize() override;
        // One cycle of the main loop
        void Tick() override;

        bool IsQuit() override;

        inline GfxConfiguration& GetConfiguration() { return m_Config; }

        virtual int CreateMainWindow() { return 0; }

        virtual void* GetMainWindow() { return nullptr; }

        virtual IApplication* GetApplication() { return this; }

    public:
        void SetCommandLineParameters(int argc, char** argv) override;
        int GetCommandLineParametersCount() override;
        const char* GetCommandLineParameters(int index) override;
    
    public:
        void RegisterManagerModule(IGraphicsManager* mgr);
        void RegisterManagerModule(IMemoryManager* mgr);
        void RegisterManagerModule(IAssetLoader* mgr);
        void RegisterManagerModule(IInputManager* mgr);
        void RegisterManagerModule(ISceneManager* mgr);
        void RegisterManagerModule(IPhysicsManager* mgr);
        void RegisterManagerModule(IGameLogic* mgr);

        IGraphicsManager* GetGraphicsManager() { return m_pGraphicsManager; }
        IMemoryManager* GetMemoryManager() { return m_pMemoryManager; }
        IAssetLoader* GetAssetLoader() { return m_pAssetLoader; }
        IInputManager* GetInputManager() { return m_pInputManager; }
        ISceneManager* GetSceneManager() { return m_pSceneManager; }
        IPhysicsManager* GetPhysicsManager() { return m_pPhysicsManager; }

    protected:
        // Flag if need quit the main loop of the application
        static bool m_bQuit;
        GfxConfiguration m_Config;

        int m_nArgC;
        char** m_ppArgV;

        IGraphicsManager* m_pGraphicsManager;
        IMemoryManager* m_pMemoryManager;
        IAssetLoader* m_pAssetLoader;
        IInputManager* m_pInputManager;
        ISceneManager* m_pSceneManager;
        IPhysicsManager* m_pPhysicsManager;
        IGameLogic* m_pGameLogicManager;

        std::vector<IRuntimeModule*> m_RuntimeModule;
    };
}
