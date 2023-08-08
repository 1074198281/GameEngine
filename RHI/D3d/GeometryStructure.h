#pragma once
#include "Core/Resource/GpuBuffer.h"
#include <vector>
#include <math.h>
#include "d3dx12.h"
#include <DirectXMath.h>


namespace My {
	typedef struct RenderObject {
		D3dGraphicsCore::StructuredBuffer VertexBuffer;
		D3dGraphicsCore::ByteAddressBuffer IndexBuffer;
		UINT indexCountPerInstance;
		UINT InstanceCount;
	} RenderObject;
}
