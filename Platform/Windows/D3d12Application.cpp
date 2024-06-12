#include "D3d12Application.hpp"
#include "WICImageLoader.h"
#include "WinUtility.h"
#include "D3d/D3d12RHI.h"
#include <tchar.h>


void My::D3d12Application::Finalize()
{
    WindowsApplication::Finalize();
    FinalizeWICLoader();
}

int My::D3d12Application::CreateMainWindow()
{
	WindowsApplication::CreateMainWindow();


    auto getFramebufferSize = [this](uint32_t& width, uint32_t& height) {
        GetFrameBufferSize(width, height);
    };

    auto getWindowHandler = [this]() -> HWND {
        return (HWND)GetMainWindow();
    };

    auto getGfxConfigHandler = [this]() { return GetConfiguration(); };


    m_GraphicsRHI = new D3dGraphicsCore::D3d12RHI();
    // 设置回调函数
    m_GraphicsRHI->SetQueryFrameBufferSize(getFramebufferSize);
    m_GraphicsRHI->SetGetWindowHandleProc(getWindowHandler);
    m_GraphicsRHI->SetGetGfxconfiguration(getGfxConfigHandler);

    m_GraphicsRHI->StartUp();

    InitializeWICLoader();

	return 0;
}

void My::D3d12Application::LoadSceneResource(Scene& scene)
{
    for (auto& material : scene.Materials) {
        for (int type = 0; type < SceneObjectMaterial::kpbrType; type++) {
            std::shared_ptr<SceneObjectTexture> pTexture = material.second->GetTexture((SceneObjectMaterial::TextureType)type);
            if (pTexture) {
                Image img;
                std::string fullPath = std::string(_ASSET_RESOURCE_DIRECTORY) + "/" + pTexture->GetName();
                std::wstring wpath = UTF8ToWideString(fullPath);
                LoadPNGAndGetImageData(wpath.c_str(), &img);
                pTexture->SetTextureImage(img);
            }
        }
    }
}