#include "WICImageLoader.h"
#include "WinUtility.h"

namespace My {
    Microsoft::WRL::ComPtr<IWICImagingFactory> g_WICFactory;
    Microsoft::WRL::ComPtr<IWICFormatConverter> g_FormatConverter;
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> g_PNGDecoder;
    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> g_FirstFrame;
}

void My::InitializeWICLoader()
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

void My::FinalizeWICLoader()
{
    if (g_WICFactory == nullptr) {
        ASSERT(false, "Factory Or Format Converter Not Initialized ERROR");
        return;
    }
    g_WICFactory.Reset();

    CoUninitialize();
}


void My::LoadPNGAndGetImageData(const wchar_t* filePath, Image* img)
{
    using namespace Microsoft::WRL;
    HRESULT hr;

    if (g_WICFactory == nullptr || g_FormatConverter != nullptr) {
        ASSERT(false, "Factory Or Format Converter Not Initialized ERROR");
        img->data = nullptr;
    }

    hr = g_WICFactory->CreateFormatConverter(&g_FormatConverter);
    if (FAILED(hr)) {
        ASSERT(false, "WICFactory Failed ERROR!");
        img->data = nullptr;
    }

    hr = g_WICFactory->CreateDecoderFromFilename(filePath, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &g_PNGDecoder);
    if (FAILED(hr)) {
        img->data = nullptr;
    }

    hr = g_PNGDecoder->GetFrame(0, &g_FirstFrame);
    if (FAILED(hr)) {
        img->data = nullptr;
    }

    GetFormatByFrame(img);

    // Get image information
    hr = g_FirstFrame->GetSize(&img->Width, &img->Height);
    if (FAILED(hr)) {
        img->data = nullptr;
    }

    hr = g_FormatConverter->Initialize(g_FirstFrame.Get(), GUID_WICPixelFormat32bppPRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeMedianCut);
    if (FAILED(hr)) {
        ASSERT(false, "Format Converter Failed ERROR!");
        img->data = nullptr;
    }

    // Calculate buffer size
    UINT stride = img->Width * 4; // Assuming 32bpp
    img->pitch = stride;
    uint64_t bufferSize = (uint64_t)stride * (uint64_t)img->Height;
    img->data_size = bufferSize;

    // Allocate buffer for image data
    void* imageDataBuffer = malloc(bufferSize);
    if (!imageDataBuffer) {
        img->data = nullptr;
    }

    if (imageDataBuffer) {
        // Copy pixels to buffer
        hr = g_FormatConverter->CopyPixels(nullptr, stride, bufferSize, static_cast<BYTE*>(imageDataBuffer));
        if (FAILED(hr)) {
            free(imageDataBuffer);
            ASSERT(false, "Copy Pixels to Buffer Failed ERROR!");
            img->data = nullptr;
        }
    }

    g_FormatConverter.Reset();
    g_PNGDecoder.Reset();
    g_FirstFrame.Reset();

    img->data = reinterpret_cast<uint8_t*>(imageDataBuffer);
}

void My::GetFormatByFrame(Image* img)
{
    HRESULT hr;

    // 获取像素格式信息
    WICPixelFormatGUID pixelFormat;
    hr = g_FirstFrame->GetPixelFormat(&pixelFormat);
    if (FAILED(hr)) {
        ASSERT(false, "Get Pixel Format ERROR!");
    }
    // 根据获取的 GUID 判断 DXGI_FORMAT
    if (IsEqualGUID(pixelFormat, GUID_WICPixelFormat32bppRGBA)) {
        img->format = PIXEL_FORMAT::RGBA8;
        img->bitcount = 32;
    }
    else if (IsEqualGUID(pixelFormat, GUID_WICPixelFormat32bppBGRA)) {
        img->format = PIXEL_FORMAT::RGBA8;
        img->bitcount = 32;
    }
    else if (IsEqualGUID(pixelFormat, GUID_WICPixelFormat24bppRGB)) {
        img->format = PIXEL_FORMAT::RGB8;
        img->bitcount = 24;
    }
    else if (IsEqualGUID(pixelFormat, GUID_WICPixelFormat24bppBGR)) {
        img->format = PIXEL_FORMAT::RGB8;
        img->bitcount = 24;
    }
    else if (IsEqualGUID(pixelFormat, GUID_WICPixelFormat8bppGray)) {
        img->format = PIXEL_FORMAT::R8;
        img->bitcount = 8;
    }
}