#include "../GeomMath/geommath.hpp"
#include <string>
#include <unordered_map>

namespace My {

	//Accessors
	enum glTF_ComponentType {
		glTF_ComponentType_NONE = 0,
		glTF_ComponentType_UNSIGNED_SHORT = 5123,
		glTF_ComponentType_FLOAT = 5126,
	};

#define glTF_DataType_SCALAR		"SCALAR"
#define glTF_DataType_VEC3			"VEC3"
#define glTF_DataType_MAT4			"MAT4"

	//bufferView
	enum glTF_BufferView_Target {
		glTF_BufferView_NONE = 0,
		glTF_BufferView_ArrayBuffer = 34962,
		glTF_BufferView_ElementArrayBuffer = 34963,
	};

	//Material
	/* pbr流程相关参数 所有参数均为可选 没有必须项 */
	typedef struct {
		Vector4f baseColorFactor;					//基础色系数
		int baseColorTextureIndex;					//基础色贴图索引
		float metallicFactor;						//金属度系数
		float roughnessFactor;						//粗糙度系数
		int metallicRoughnessTextureIndex;			//金属度或粗糙度贴图索引
	} pbrMetallicRoughness;

	typedef struct {
		int normalTextureIndex;						//法线贴图索引	必须
		int texCoord;								//在primitives.attributes中的位置，如果是0则为TEXCOORD_0  可选
		float scale;								//缩放系数  可选  scaledNormal = normalize((<sampled normal texture value> * 2.0 - 1.0) * vec3(<normal scale>, <normal scale>, 1.0))来设置
	} normalTexture;

	typedef struct {
		int occlusionTextureIndex;					//遮蔽贴图索引  必须
		int texCoord;								//在primitives.attributes中的位置，如果是0则为TEXCOORD_0  可选
		float strength;								//遮蔽强度 可选
	} occlusionTexture;


	//basic structures
	typedef struct {
		int BufferView;
		int ByteOffset;
		glTF_ComponentType componentType;
		int count;
		union {
			int index;
			Vector3f vertex;
		} dataBoundaryMin;
		union {
			int index;
			Vector3f vertex;
		} dataBoundaryMax;
		std::string dataType;
	} glTF_Accessors;

	typedef struct {
		std::string generator;
		std::string version;
	} glTF_Asset;

	typedef struct {
		int bufferIndex;
		int byteLength;
		int byteOffset;
		glTF_BufferView_Target targetType;
	} glTF_BufferViews;

	typedef struct {
		int byteLength;
		std::string uri;
	} glTF_Buffers;

	typedef struct {
		std::vector<std::string> images;
	} glTF_Images;

	typedef struct {
		std::string name;
		pbrMetallicRoughness pbr;
		normalTexture normal;
		occlusionTexture occlusion;
	} glTF_Materials;

	typedef struct {
		std::string name;

	} glTF_Meshes;

	typedef struct {
		std::string name;
		int meshIndex;
	} glTF_Nodes;

	typedef struct {

	} glTF_Samplers;

	typedef struct {

	} glTF_Scenes;

	typedef struct {
		int samplerIndex;
		int sourceIndex;
	} glTF_Textures;
}
