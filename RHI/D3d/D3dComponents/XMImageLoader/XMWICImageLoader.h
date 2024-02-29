#pragma once

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <wincodec.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "windowscodecs.lib")

typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

namespace D3dGraphicsCore {
    namespace WICLoader {
        void InitializeWICLoader();

        void FinalizeWICLoader();

        // Helper function to load a PNG image using WIC and get image data pointer
        void* LoadPNGAndGetImageData(const wchar_t* filePath, uint32_t& width, uint32_t& height, uint32_t& pitch, uint64_t& imageSize, DXGI_FORMAT& format);

        DXGI_FORMAT GetFormatByFrame();

        // WIC Factory
        extern Microsoft::WRL::ComPtr<IWICImagingFactory> g_WICFactory;

        // format converter
        extern Microsoft::WRL::ComPtr<IWICFormatConverter> g_FormatConverter;

        // in one processing 
        // a decoder for the PNG file
        extern Microsoft::WRL::ComPtr<IWICBitmapDecoder> g_PNGDecoder;

        // Get the first frame of the image
        extern Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> g_FirstFrame;
    }
}



