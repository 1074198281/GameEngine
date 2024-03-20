#include <iostream>
#include "GraphicsManager.hpp"
#include "BaseApplication.hpp"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

using namespace My;

int GraphicsManager::Initialize()
{
    int result = 0;
    m_nSceneRevision = 0;
    m_nFrameIndex = 0;
    return result;
}

void GraphicsManager::Finalize()
{
}

void GraphicsManager::Tick()
{
    auto pSceneManager = dynamic_cast<BaseApplication*>(m_pApp)->GetSceneManager();

    if (pSceneManager) {
        auto rev = pSceneManager->GetSceneRevision();
        if (rev == 0) return;
        assert(m_nSceneRevision <= rev);
        if (m_nSceneRevision < rev) {
            EndScene();

            std::cerr << "[GraphicsManager] Detected Scene Changed, reinitialize buffers" << std::endl;
            auto& scene = pSceneManager->GetSceneForRendering();
            BeginScene(scene);
            m_nSceneRevision = rev;
        }
    }

    //assert(m_Frames.size());
    //BeginFrame(m_Frames[m_nFrameIndex]);
    //ImGui::NewFrame();
    Draw();
    //ImGui::EndFrame();
    //ImGui::Render();
    //EndFrame(m_Frames[m_nFrameIndex]);
    Present();
}

void GraphicsManager::Clear()
{
}

void GraphicsManager::Draw()
{
}

void GraphicsManager::Resize(uint32_t width, uint32_t height)
{
}


void GraphicsManager::BeginScene(const Scene& scene)
{

}

void GraphicsManager::EndScene()
{

}

void GraphicsManager::UpArrowKeyDown()
{
}

void GraphicsManager::DownArrowKeyDown()
{
}

void GraphicsManager::LeftArrowKeyDown()
{
}

void GraphicsManager::RightArrowKeyDown()
{
}

void GraphicsManager::NumPadKeyDown(int64_t key)
{
}

void GraphicsManager::NumPadKeyUp(int64_t key)
{
}

void GraphicsManager::FunctionKeyDown(int64_t key)
{
}

void GraphicsManager::FunctionKeyUp(int64_t key)
{
}
