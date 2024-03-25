#pragma once
#include <iostream>
#include "cbuffer.h"
#include "SceneNodeGeometry.hpp"

#define MAX_FRAME_COUNT 3

namespace My {

	typedef struct DrawFrameContext : PerFrameConstants {

	} DrawFrameContext;

	typedef struct DrawBatchContext : PerBatchConstants, MaterialConstants {
		int BatchIndex;
		std::shared_ptr<SceneGeometryNode> Node;
		MaterialTextures Material;
	} DrawBatchContext;

	typedef struct Frame : GlobalTextures {
		int FrameIndex;
		DrawFrameContext FrameContext;
		std::vector<std::shared_ptr<DrawBatchContext> > BatchContexts;
		LightInfo LightInfomation;
		Vector4f ClearColor;
		std::vector<Texture> ColorBuffers;
		Texture DepthTexture;
		//bool RenderToTexture = false;
		//bool EnableMSAA = false;
		//bool ClearRT = false;
	} Frame;
}