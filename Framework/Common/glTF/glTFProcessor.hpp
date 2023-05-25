#pragma once
#include "glTFStructures.h"
#include "rapidjson\document.h"
#include "rapidjson\stringbuffer.h"

#ifndef BRACKETS
#define LEFT_BRACE			'{'
#define RIGHT_BRACE			'}'
#define LEFT_BRACKET		'['
#define RIGHT_BRACKET		']'

using namespace rapidjson;

namespace My {
	namespace glTF {
		class CGLTFProcessor {
		public:
			CGLTFProcessor();
			~CGLTFProcessor() {};
		public:
			bool ProcessText(const std::string& buf);
		private:
			void ProcessAccessor(rapidjson::Value& Value);
			void ProcessAsset(rapidjson::Value& Value);
			void ProcessBufferViews(rapidjson::Value& Value);
			void ProcessBuffers(rapidjson::Value& Value);
			void ProcessImages(rapidjson::Value& Value);
			void ProcessMaterials(rapidjson::Value& Value);
			void ProcessMeshes(rapidjson::Value& Value);
			void ProcessNodes(rapidjson::Value& Value);
			void ProcessSamplers(rapidjson::Value& Value);
			void ProcessScene(rapidjson::Value& Value);
			void ProcessScenes(rapidjson::Value& Value);
			void ProcessTextures(rapidjson::Value& Value);
			
		private:
			std::vector<glTF_Accessor> m_Accessors;
			std::vector<glTF_Asset> m_Asset;
			std::vector<glTF_BufferView> m_BufferViews;
			std::vector<glTF_Buffer> m_Buffers;
			std::vector<glTF_Image> m_Images;
			std::vector<glTF_Material> m_Materials;
			std::vector<glTF_Mesh> m_Meshes;
			std::vector<glTF_Node> m_Nodes;
			std::vector<glTF_Sampler> m_Samplers;
			std::vector<glTF_Scene> m_Scenes;
			std::vector<glTF_Texture> m_Textures;
		};

		CGLTFProcessor::CGLTFProcessor()
		{

		}

		void CGLTFProcessor::ProcessAccessor(rapidjson::Value& Value)
		{
			Type type = Value.GetType();
			if (type == kArrayType) {
				// accessor array
				for (auto it = Value.Begin(); it != Value.End(); it++) {
					glTF_Accessor acc;
					auto object = it->GetObject();
					acc.BufferView = object["bufferView"].GetInt();
					acc.ByteOffset = object["byteOffset"].GetInt();
					acc.componentType = object["componentType"].GetInt();
					acc.count = object["count"].GetInt();
					acc.dataType = object["type"].GetString();

					rapidjson::Value& maxValue = object["max"];
					for (auto maxit = maxValue.Begin(); maxit != maxValue.End(); maxit++) {
						(acc.dataType != "SCALAR") ? acc.dataBoundaryVertexMax.push_back(maxit->GetFloat()) 
							: acc.dataBoundaryIndexMax.push_back(maxit->GetInt());
					}
					m_Accessors.push_back(acc);
				}
			}
			else {
				assert(false);
			}
		}

		void CGLTFProcessor::ProcessAsset(rapidjson::Value& Value)
		{
			glTF_Asset asset;
			Type type = Value.GetType();
			auto object = Value.GetObject();
			if (object.HasMember("generator")) {
				asset.generator = object["generator"].GetString();
			}
			if (object.HasMember("version")) {
				asset.version = object["version"].GetString();
			}
			m_Asset.push_back(asset);
		}

		void CGLTFProcessor::ProcessBufferViews(rapidjson::Value& Value)
		{
			Type type = Value.GetType();
			if (type == kArrayType) {
				for (auto it = Value.Begin(); it != Value.End(); it++) {
					glTF_BufferView bufferView;
					auto object = it->GetObject();
					bufferView.bufferIndex = object["buffer"].GetInt();
					bufferView.byteLength = object["byteLength"].GetInt();
					bufferView.byteOffset = object["byteOffset"].GetInt();
					bufferView.targetType = object["target"].GetInt();
					m_BufferViews.push_back(bufferView);
				}
			}
			else {
				assert(false);
			}
		}

		void CGLTFProcessor::ProcessBuffers(rapidjson::Value& Value)
		{
			Type type = Value.GetType();
			if (type == kArrayType) {
				for (auto it = Value.Begin(); it != Value.End(); it++) {
					glTF_Buffer buffer;
					auto object = it->GetObject();
					buffer.byteLength = object["byteLength"].GetInt();
					buffer.uri = object["uri"].GetString();
					m_Buffers.push_back(buffer);
				}
			}
		}

		void CGLTFProcessor::ProcessImages(rapidjson::Value& Value)
		{
			Type type = Value.GetType();
			if (type == kArrayType) {
				for (auto it = Value.Begin(); it != Value.End(); it++) {
					glTF_Image image;
					auto object = it->GetObject();
					image.image = object["uri"].GetString();
					m_Images.push_back(image);
				}
			}
			else {
				assert(false);
			}
		}

		void CGLTFProcessor::ProcessMaterials(rapidjson::Value& Value)
		{
			Type type = Value.GetType();
			if (type == kArrayType) {
				for (auto it = Value.Begin(); it != Value.End(); it++) {
					glTF_Material material;
					auto object = it->GetObject();
					if (object.HasMember("name")) {
						material.name = object["name"].GetString();
					}

					if (object.HasMember("pbrMetallicRoughness")) {
						rapidjson::Value& pbr = object["pbrMetallicRoughness"];
						if (pbr.HasMember("baseColorFactor")) {
							if (pbr.GetObject()["baseColorFactor"].GetType() == kArrayType) {
								for (auto it = pbr.GetObject()["baseColorFactor"].Begin(); it != pbr.GetObject()["baseColorFactor"].End(); it++) {
									material.pbr.baseColorFactor.push_back(it->GetFloat());
								}
							}
						}
						if (pbr.GetObject()["baseColorTexture"].HasMember("index")) {
							material.pbr.baseColorTextureIndex = pbr.GetObject()["baseColorTexture"]["index"].GetInt();
						}
						if (pbr.GetObject()["metallicRoughnessTexture"].HasMember("index")) {
							material.pbr.metallicRoughnessTextureIndex = pbr.GetObject()["metallicRoughnessTexture"]["index"].GetInt();
						}
						if (pbr.GetObject().HasMember("metallicFactor")) {
							material.pbr.metallicFactor = pbr.GetObject()["metallicFactor"].GetFloat();
						}
						if (pbr.GetObject().HasMember("roughnessFactor")) {
							material.pbr.metallicFactor = pbr.GetObject()["roughnessFactor"].GetFloat();
						}
					}

					/* remain
					normalTexture normal;
					occlusionTexture occlusion;
					*/
					m_Materials.push_back(material);

				}
			}
		}

		void CGLTFProcessor::ProcessMeshes(rapidjson::Value& Value)
		{
			Type type = Value.GetType();
			if (type == kArrayType) {
				for (auto it = Value.Begin(); it != Value.End(); it++) {
					glTF_Mesh mesh;
					auto object = it->GetObject();
					if (object.HasMember("name")) {
						mesh.name = object["name"].GetString();
					}
					if (object.HasMember("primitives")) {
						rapidjson::Value& prim = object["primitives"];
						if (prim.GetType() == kArrayType) {
							for (auto itp = prim.Begin(); itp != prim.End(); itp++) {
								primitives primit;
								if (itp->HasMember("attributes")) {
									if (itp->GetObject()["attributes"].GetType() == kObjectType) {
										if (itp->GetObject()["attributes"].HasMember("POSITION")) {
											primit.atrribute.positionIndex = itp->GetObject()["attributes"].GetObject()["POSITION"].GetInt();
										}
										if (itp->GetObject()["attributes"].HasMember("NORMAL")) {
											primit.atrribute.positionIndex = itp->GetObject()["attributes"].GetObject()["NORMAL"].GetInt();
										}
										if (itp->GetObject()["attributes"].HasMember("TANGENT")) {
											primit.atrribute.positionIndex = itp->GetObject()["attributes"].GetObject()["TANGENT"].GetInt();
										}
										if (itp->GetObject()["attributes"].HasMember("TEXCOORD_0")) {
											primit.atrribute.positionIndex = itp->GetObject()["attributes"].GetObject()["TEXCOORD_0"].GetInt();
										}
									}
								}
								if (itp->HasMember("indices")) {
									primit.indices = itp->GetObject()["indices"].GetInt();
								}
								if (itp->HasMember("material")) {
									primit.materialIndex = itp->GetObject()["material"].GetInt();
								}
								if (itp->HasMember("mode")) {
									primit.mode = itp->GetObject()["mode"].GetInt();
								}

								mesh.primitives.push_back(primit);
							}
						}
					}
					m_Meshes.push_back(mesh);
				}
			}
		}

		void CGLTFProcessor::ProcessNodes(rapidjson::Value& Value)
		{
			Type type = Value.GetType();
			if (type == kArrayType) {

			}
		}

		void CGLTFProcessor::ProcessSamplers(rapidjson::Value& Value)
		{
			Type type = Value.GetType();
		}

		void CGLTFProcessor::ProcessScene(rapidjson::Value& Value)
		{
			Type type = Value.GetType();
		}

		void CGLTFProcessor::ProcessScenes(rapidjson::Value& Value)
		{
			Type type = Value.GetType();
		}

		void CGLTFProcessor::ProcessTextures(rapidjson::Value& Value)
		{
			Type type = Value.GetType();
		}

		bool CGLTFProcessor::ProcessText(const std::string& buf)
		{
			Document d;
			d.Parse(buf.c_str());

			if (d.HasParseError()) {
				return false;
			}

			if (d.HasMember("accessors")) {
				Value& accessorValue = d["accessors"];
				ProcessAccessor(accessorValue);
			}

			if (d.HasMember("asset")) {
				Value& assetValue = d["asset"];
				ProcessAsset(assetValue);
			}

			if (d.HasMember("bufferViews")) {
				Value& bufferViewsValue = d["bufferViews"];
				ProcessBufferViews(bufferViewsValue);
			}

			if (d.HasMember("buffers")) {
				Value& BuffersValue = d["buffers"];
				ProcessBuffers(BuffersValue);
			}

			if (d.HasMember("images")) {
				Value& imagesValue = d["images"];
				ProcessImages(imagesValue);
			}

			if (d.HasMember("materials")) {
				Value& materialValue = d["materials"];
				ProcessMaterials(materialValue);
			}

			if (d.HasMember("meshes")) {
				Value& meshesValue = d["meshes"];
				ProcessMeshes(meshesValue);
			}

			if (d.HasMember("nodes")) {
				Value& nodesValue = d["nodes"];
				ProcessNodes(nodesValue);
			}

			if (d.HasMember("samplers")) {
				Value& samplersValue = d["samplers"];
				ProcessSamplers(samplersValue);
			}

			if (d.HasMember("scene")) {
				Value& sceneValue = d["scene"];
				ProcessScene(sceneValue);
			}

			if (d.HasMember("scenes")) {
				Value& scenesValue = d["scenes"];
				ProcessScenes(scenesValue);
			}

			if (d.HasMember("textures")) {
				Value& texturesValue = d["textures"];
				ProcessTextures(texturesValue);
			}

			return true;
		}
	}
}

#endif // !BRACKETS