#pragma once
#include "StructureSettings.h"

namespace My {

#define GENERATE_TEXTURE_INFO(v, HeapIdx, Handle, ImageInfo) \
	v->iHeapIndex = HeapIdx; \
	v->handle = Handle; \
	v->iSize = std::get<0>(ImageInfo); \
	v->iWidth = std::get<1>(ImageInfo); \
	v->iHeight = std::get<2>(ImageInfo); \
	v->iPitchOrLinearSize = std::get<3>(ImageInfo); \
	v->iDepth = std::get<4>(ImageInfo); \
	v->iMipMapCount = std::get<5>(ImageInfo); \

}
