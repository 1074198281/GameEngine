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
	/* pbr������ز��� ���в�����Ϊ��ѡ û�б����� */
	typedef struct {
		std::vector<float> baseColorFactor;					//����ɫϵ��
		int baseColorTextureIndex = -1;					//����ɫ��ͼ����
		float metallicFactor = -1;						//������ϵ��
		float roughnessFactor = -1;						//�ֲڶ�ϵ��
		int metallicRoughnessTextureIndex = -1;			//�����Ȼ�ֲڶ���ͼ����
	} pbrMetallicRoughness;

	typedef struct {
		int normalTextureIndex = -1;						//������ͼ����	����
		int texCoord = -1;								//��primitives.attributes�е�λ�ã������0��ΪTEXCOORD_0  ��ѡ
		float scale = -1;								//����ϵ��  ��ѡ  scaledNormal = normalize((<sampled normal texture value> * 2.0 - 1.0) * vec3(<normal scale>, <normal scale>, 1.0))������
	} normalTexture;

	typedef struct {
		int occlusionTextureIndex = -1;					//�ڱ���ͼ����  ����
		int texCoord = -1;								//��primitives.attributes�е�λ�ã������0��ΪTEXCOORD_0  ��ѡ
		float strength = -1;								//�ڱ�ǿ�� ��ѡ
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