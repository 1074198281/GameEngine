#include <iostream>
#include <memory>
#include "GraphicsManager.hpp"
#include "BaseApplication.hpp"
#include "ForwardGeometryPass.hpp"
#include "OverlayPass.hpp"
#include "Scene.hpp"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

using namespace My;

int GraphicsManager::Initialize()
{
    int result = 0;
    m_nSceneRevision = 0;
    m_nFrameIndex = 0;
    m_Frames.resize(MAX_FRAME_COUNT);

    m_DrawPasses.push_back(std::make_shared<ForwardGeometryPass>(this));
    m_DrawPasses.push_back(std::make_shared<OverlayPass>(this));

    return result;
}

void GraphicsManager::Finalize()
{
    EndScene();
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

    assert(m_Frames.size());
    BeginFrame(m_Frames[m_nFrameIndex]);
    //StartGUIFrame();
    Draw();
    //EndGUIFrame();
    EndFrame(m_Frames[m_nFrameIndex]);
    Present();
}

void GraphicsManager::Clear()
{
}

void GraphicsManager::Draw()
{
    auto& frame = m_Frames[m_nFrameIndex];
    for (auto& pass : m_DrawPasses) {
        pass->BeginPass(frame);
        pass->Draw(frame);
        pass->EndPass(frame);
    }
}

void GraphicsManager::Resize(uint32_t width, uint32_t height)
{
}


void GraphicsManager::BeginScene(const Scene& scene)
{
    if (scene.GeometryNodes.size()) {
        initializeGeometries(scene);
    }
    initializeSkybox(scene);

    initializeFixedHandle();
}

void GraphicsManager::EndScene()
{

}

void GraphicsManager::UpdateFrameConstants(Frame& frame)
{
    auto pSceneManager = dynamic_cast<BaseApplication*>(m_pApp)->GetSceneManager();
    if (pSceneManager) {
        auto& scene = pSceneManager->GetSceneForRendering();
        for (auto& batch : frame.BatchContexts) {
            if (batch->Node->GetMaterialCount() == 1) {
                auto name = batch->Node->GetMaterialRef(0);
                batch->BaseColorFactor[0] = scene.Materials.at(name)->GetBaseColorFactor()[0];
                batch->BaseColorFactor[1] = scene.Materials.at(name)->GetBaseColorFactor()[1];
                batch->BaseColorFactor[2] = scene.Materials.at(name)->GetBaseColorFactor()[2];
                batch->BaseColorFactor[3] = scene.Materials.at(name)->GetBaseColorFactor()[3];

                batch->MetallicRoughnessFactor[0] = scene.Materials.at(name)->GetMetallicFactor();
                batch->MetallicRoughnessFactor[1] = scene.Materials.at(name)->GetRoughnessFactor();

                batch->EmissiveFactor[0] = scene.Materials.at(name)->GetEmissivsFactor()[0];
                batch->EmissiveFactor[1] = scene.Materials.at(name)->GetEmissivsFactor()[1];
                batch->EmissiveFactor[2] = scene.Materials.at(name)->GetEmissivsFactor()[2];

                batch->NormalTextureScale = scene.Materials.at(name)->GetNormalScaleFactor();
            }
        }
    }

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
