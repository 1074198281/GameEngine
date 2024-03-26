#pragma once

#include "Image.hpp"
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

namespace My {
    void InitializeWICLoader();

    void FinalizeWICLoader();

    // Helper function to load a PNG image using WIC and get image data pointer
    void LoadPNGAndGetImageData(const wchar_t* filePath, Image* img);

    void GetFormatByFrame(Image* img);

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



