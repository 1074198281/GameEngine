#include "XMWICImageLoader.h"
#include "Core/Utility.h"

namespace D3dGraphicsCore {
    namespace WICLoader {
        Microsoft::WRL::ComPtr<IWICImagingFactory> g_WICFactory;
        Microsoft::WRL::ComPtr<IWICFormatConverter> g_FormatConverter;
        Microsoft::WRL::ComPtr<IWICBitmapDecoder> g_PNGDecoder;
        Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> g_FirstFrame;
    }
}

void D3dGraphicsCore::WICLoader::InitializeWICLoader()
{
    using namespace Microsoft::WRL;
    HRESULT hr;
    hr = CoInitialize(nullptr);
    if (FAILED(hr)) {
        ASSERT(false, "CoInitialize Failed ERROR!");
        return;
    }

    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_WICFactory));
    if (FAILED(hr)) {
        ASSERT(false, "WICFactory Failed ERROR!");
        return;
    }

    // Initialize the format converter
    //hr = g_WICFactory->CreateFormatConverter(&g_FormatConverter);
    //if (FAILED(hr)) {
    //    ASSERT(false, "WICFactory Failed ERROR!");
    //    return;
    //}

}

void D3dGraphicsCore::WICLoader::FinalizeWICLoader()
{
    if (g_WICFactory == nullptr) {
        ASSERT(false, "Factory Or Format Converter Not Initialized ERROR");
        return;
    }
    g_WICFactory.Reset();

    CoUninitialize();
}


void* D3dGraphicsCore::WICLoader::LoadPNGAndGetImageData(const wchar_t* filePath, uint32_t& width, uint32_t& height, uint32_t& pitch, uint64_t& imageSize)
{
    using namespace Microsoft::WRL;
    HRESULT hr;

    if (g_WICFactory == nullptr || g_FormatConverter != nullptr) {
        ASSERT(false, "Factory Or Format Converter Not Initialized ERROR");
        return nullptr;
    }

    hr = g_WICFactory->CreateFormatConverter(&g_FormatConverter);
    if (FAILED(hr)) {
        ASSERT(false, "WICFactory Failed ERROR!");
        return nullptr;
    }

    hr = g_WICFactory->CreateDecoderFromFilename(filePath, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &g_PNGDecoder);
    if (FAILED(hr)) {
        return nullptr;
    }

    hr = g_PNGDecoder->GetFrame(0, &g_FirstFrame);
    if (FAILED(hr)) {
        return nullptr;
    }

    // Get image information
    hr = g_FirstFrame->GetSize(&width, &height);
    if (FAILED(hr)) {
        return nullptr;
    }

    hr = g_FormatConverter->Initialize(g_FirstFrame.Get(), GUID_WICPixelFormat32bppPRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeMedianCut);
    if (FAILED(hr)) {
        ASSERT(false, "Format Converter Failed ERROR!");
        return nullptr;
    }

    // Calculate buffer size
    UINT stride = width * 4; // Assuming 32bpp
    pitch = stride;
    uint64_t bufferSize = (uint64_t)stride * (uint64_t)height;
    imageSize = bufferSize;

    // Allocate buffer for image data
    void* imageDataBuffer = malloc(bufferSize);
    if (!imageDataBuffer) {
        return nullptr;
    }

    // Copy pixels to buffer
    hr = g_FormatConverter->CopyPixels(nullptr, stride, bufferSize, static_cast<BYTE*>(imageDataBuffer));
    if (FAILED(hr)) {
        free(imageDataBuffer);
        ASSERT(false, "Copy Pixels to Buffer Failed ERROR!");
        return nullptr;
    }

    g_FormatConverter.Reset();
    g_PNGDecoder.Reset();
    g_FirstFrame.Reset();

    return imageDataBuffer;
}
