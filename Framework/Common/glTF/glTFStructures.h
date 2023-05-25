#pragma once
#include "../GeomMath/geommath.hpp"
#include <string>
#include <unordered_map>

#pragma pack(4)
namespace My {

#define glTF_DataType_SCALAR		"SCALAR"
#define glTF_DataType_VEC3			"VEC3"
#define glTF_DataType_MAT4			"MAT4"

	//Material
	/* pbr流程相关参数 所有参数均为可选 没有必须项 */
	typedef struct {
		std::vector<float> baseColorFactor;					//基础色系数
		int baseColorTextureIndex = -1;					//基础色贴图索引
		float metallicFactor = -1;						//金属度系数
		float roughnessFactor = -1;						//粗糙度系数
		int metallicRoughnessTextureIndex = -1;			//金属度或粗糙度贴图索引
	} pbrMetallicRoughness;

	typedef struct {
		int normalTextureIndex = -1;						//法线贴图索引	必须
		int texCoord = -1;								//在primitives.attributes中的位置，如果是0则为TEXCOORD_0  可选
		float scale = -1;								//缩放系数  可选  scaledNormal = normalize((<sampled normal texture value> * 2.0 - 1.0) * vec3(<normal scale>, <normal scale>, 1.0))来设置
	} normalTexture;

	typedef struct {
		int occlusionTextureIndex = -1;					//遮蔽贴图索引  必须
		int texCoord = -1;								//在primitives.attributes中的位置，如果是0则为TEXCOORD_0  可选
		float strength = -1;								//遮蔽强度 可选
	} occlusionTexture;

	typedef struct {
		int positionIndex = -1;
		int normalIndex = -1;
		int tangentIndex = -1;
		int texcoord0Index = -1;
	} attributes;

	typedef struct {
		attributes atrribute;
		int indices = -1;
		int materialIndex = -1;
		int mode = -1;
	} primitives;

	//basic structures
	typedef struct {
		int BufferView = -1;
		int ByteOffset = -1;
		int componentType = -1;
		int count = -1;
		std::vector<int> dataBoundaryIndexMin;
		std::vector<float> dataBoundaryVertexMin;
		std::vector<int> dataBoundaryIndexMax;
		std::vector<float> dataBoundaryVertexMax;
		std::string dataType;
	} glTF_Accessor;

	typedef struct {
		std::string generator;
		std::string version;
	} glTF_Asset;

	typedef struct {
		int bufferIndex = -1;
		int byteLength = -1;
		int byteOffset = -1;
		int targetType = -1;
	} glTF_BufferView;

	typedef struct {
		int byteLength = -1;
		std::string uri;
	} glTF_Buffer;

	typedef struct {
		std::string image;
	} glTF_Image;

	typedef struct {
		std::string name;
		pbrMetallicRoughness pbr;
		normalTexture normal;
		occlusionTexture occlusion;
	} glTF_Material;

	typedef struct {
		std::string name;
		std::vector<primitives> primitives;
	} glTF_Mesh;

	typedef struct {
		std::string name;
		int meshIndex = -1;
	} glTF_Node;

	typedef struct {

	} glTF_Sampler;

	typedef struct {
		int nodeIndex = -1;
	} glTF_Scene;

	typedef struct {
		int samplerIndex = -1;
		int sourceIndex = -1;
	} glTF_Texture;


}

#pragma pack(pop)