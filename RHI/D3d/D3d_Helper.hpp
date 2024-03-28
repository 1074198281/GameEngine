#pragma once

#include <d3d12.h>
#include "Image.hpp"

inline void GetDXGIFormat(const My::PIXEL_FORMAT& pixel_format, DXGI_FORMAT& dxgi_format)
{
    switch (pixel_format) {
    case My::PIXEL_FORMAT::RGBA8:
        dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM;
        break;
    case My::PIXEL_FORMAT::RGB8:
        dxgi_format = DXGI_FORMAT_B8G8R8X8_UNORM;
        break;
    case My::PIXEL_FORMAT::R8:
        dxgi_format = DXGI_FORMAT_R8_UNORM;
        break;
    default:
        dxgi_format = DXGI_FORMAT_UNKNOWN;
    }
}

inline DXGI_FORMAT GetDXGIFormat(const My::PIXEL_FORMAT& pixel_format)
{
    DXGI_FORMAT dxgi_format;
    switch (pixel_format) {
    case My::PIXEL_FORMAT::RGBA8:
        dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM;
        break;
    case My::PIXEL_FORMAT::RGB8:
        dxgi_format = DXGI_FORMAT_B8G8R8X8_UNORM;
        break;
    case My::PIXEL_FORMAT::R8:
        dxgi_format = DXGI_FORMAT_R8_UNORM;
        break;
    default:
        dxgi_format = DXGI_FORMAT_UNKNOWN;
    }

    return dxgi_format;
}