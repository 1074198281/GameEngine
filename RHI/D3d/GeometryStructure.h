#include "../GeomMath/geommath.hpp"
#include <vector>
#include <math.h>
#include "d3dx12.h"
#include <DirectXMath.h>

namespace My {
	typedef enum
	{
		INPUTLAYOUT_POS = 0,
		INPUTLAYOUT_POS_TEX,
		INPUTLAYOUT_POS_NORMAL_TEX,
		INPUTLAYOUT_POS_NORMAL_TANGENT_TEX,
	} INPUTLAYOUT_TYPE;

	struct Vertex {
		Vector3f pos;
	};

	struct VertexT {
		Vector3f pos;
		Vector2f tex;
	};

	struct VertexNT {
		Vector3f pos;
		Vector3f normal;
		Vector2f tex;
	};

	struct VertexNTT {
		Vector3f pos;
		Vector3f normal;
		Vector3f tangent;
		Vector2f tex;
	};

	//temp
	typedef struct {
		DirectX::XMFLOAT4X4 mat;
	} ConstantBuffer;

	const D3D12_INPUT_ELEMENT_DESC InputLayout1 = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	const D3D12_INPUT_ELEMENT_DESC InputLayout2 = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	const D3D12_INPUT_ELEMENT_DESC InputLayout3 = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	const D3D12_INPUT_ELEMENT_DESC InputLayout4 = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
}
