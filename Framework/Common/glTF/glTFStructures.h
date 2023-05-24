#pragma once
#include "../GeomMath/geommath.hpp"
#include <string>
#include <unordered_map>

#pragma pack(4)
namespace My {

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
	/* pbr������ز��� ���в�����Ϊ��ѡ û�б����� */
	typedef struct {
		Vector4f baseColorFactor;					//����ɫϵ��
		int baseColorTextureIndex;					//����ɫ��ͼ����
		float metallicFactor;						//������ϵ��
		float roughnessFactor;						//�ֲڶ�ϵ��
		int metallicRoughnessTextureIndex;			//�����Ȼ�ֲڶ���ͼ����
	} pbrMetallicRoughness;

	typedef struct {
		int normalTextureIndex;						//������ͼ����	����
		int texCoord;								//��primitives.attributes�е�λ�ã������0��ΪTEXCOORD_0  ��ѡ
		float scale;								//����ϵ��  ��ѡ  scaledNormal = normalize((<sampled normal texture value> * 2.0 - 1.0) * vec3(<normal scale>, <normal scale>, 1.0))������
	} normalTexture;

	typedef struct {
		int occlusionTextureIndex;					//�ڱ���ͼ����  ����
		int texCoord;								//��primitives.attributes�е�λ�ã������0��ΪTEXCOORD_0  ��ѡ
		float strength;								//�ڱ�ǿ�� ��ѡ
	} occlusionTexture;


	//basic structures
	typedef struct {
		int BufferView;
		int ByteOffset;
		int componentType;
		int count;
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
		int bufferIndex;
		int byteLength;
		int byteOffset;
		glTF_BufferView_Target targetType;
	} glTF_BufferView;

	typedef struct {
		int byteLength;
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

	} glTF_Mesh;

	typedef struct {
		std::string name;
		int meshIndex;
	} glTF_Node;

	typedef struct {

	} glTF_Sampler;

	typedef struct {
		int nodeIndex;
	} glTF_Scene;

	typedef struct {
		int samplerIndex;
		int sourceIndex;
	} glTF_Texture;


}

#pragma pack(pop)