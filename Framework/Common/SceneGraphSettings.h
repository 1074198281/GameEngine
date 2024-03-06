#pragma once


namespace My {
	typedef struct TextureTransform {
		float offset[2];
		float scale[2];
		float rotation;
	} TextureTransform;

	typedef struct TextureParamFactor {
		float BaseColorFactor[4];
		float MetallicRoughnessFactor[2];
		float EmissiveFactor[3];
		float NormalTextureScaleFactor;
	} TextureParamFactor;
}