#pragma once
#include "../GeomMath/geommath.hpp"

namespace My {

    enum class PIXEL_FORMAT : uint16_t {
        UNKNOWN,
        R8,
        RG8,
        RGB8,
        RGBA8,
        R16,
        RG16,
        RGB16,
        RGBA16,
        R32,
        RG32,
        RGB32,
        RGBA32,
        R10G10B10A2,
        R5G6B5,
        D24R8,
        D32
    };

    typedef struct _Image {
        uint32_t Width;
        uint32_t Height;
        R8G8B8A8Unorm* data;
        uint32_t bitcount;
        uint32_t pitch;
        size_t  data_size;
    } Image;


    std::ostream& operator<<(std::ostream& out, const Image& image);
}

