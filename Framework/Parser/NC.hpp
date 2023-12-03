#pragma once
#include "SceneParser.hpp"
#include "SceneObject.hpp"
#include <fstream>
#include <vector>
#include <memory>

namespace NCParser {
	class NCParser : __implements My::SceneParser
	{
	public:
		NCParser() {} ;
		~NCParser() {} ;
	private:
		void ProcessNCData(std::shared_ptr<My::BaseSceneNode> Node, My::Scene& Scene, std::string NCPath)
		{
			std::shared_ptr<My::SceneGeometryNode> GeoNode = std::make_shared<My::SceneGeometryNode>("NC");
			std::shared_ptr<My::SceneObjectGeometry> GeoObject = std::make_shared<My::SceneObjectGeometry>();

			std::shared_ptr<My::SceneObjectMesh> GeoMesh;
			GeoMesh = std::make_shared<My::SceneObjectMesh>();

			std::ifstream file;
			std::string NCFilePath = _WORKING_DIRECTORY + std::string("/Asset/") + std::string(NCPath);
			file.open(NCFilePath);
			std::string currLine;
			std::vector<float> vertex;
			std::vector<int> index;
			int count = 0;

			while (std::getline(file, currLine)) {
				size_t posX = currLine.find('X');
				size_t posY = currLine.find('Y');
				size_t posZ = currLine.find('Z');

				float x = atof(currLine.substr(posX + 1, posY - posX).c_str());
				float y = atof(currLine.substr(posY + 1, posZ - posY).c_str());
				float z = atof(currLine.substr(posZ + 1, currLine.size() - posY).c_str());

				vertex.push_back(x);
				vertex.push_back(y);
				vertex.push_back(z);

				index.push_back(count);
				count++;
			}
			float* pVertexData = new float[vertex.size()];
			memcpy(pVertexData, vertex.data(), sizeof(float) * vertex.size());
			float* pIndexData = new float[index.size()];
			memcpy(pIndexData, index.data(), sizeof(int) * index.size());

			My::SceneObjectVertexArray& _v_array = *new My::SceneObjectVertexArray("POSITION", 0, My::VertexDataType::kVertexDataTypeFloat3, pVertexData, count * 3);
			GeoMesh->AddVertexArray(std::move(_v_array));
			My::SceneObjectIndexArray& _i_array = *new My::SceneObjectIndexArray(-1, 0, My::kIndexDataTypeInt32, pIndexData, count);
			GeoMesh->AddIndexArray(std::move(_i_array));
			GeoMesh->SetPrimitiveType(My::kPrimitiveTypeLineStrip);

			GeoObject->AddMesh(GeoMesh);
			GeoNode->AddSceneObjectRef("NC");
			Scene.LUT_Name_GeometryNode.emplace("NC", GeoNode);
			Scene.GeometryNodes.emplace("NC", GeoNode);
			Scene.Geometries["NC"] = GeoObject;
		}

	public:
		virtual std::unique_ptr<My::Scene> Parse(const std::string& buf, std::string RootNodeName)
		{
			std::unique_ptr<My::Scene> pScene(new My::Scene("NC Scene"));

			std::shared_ptr<My::BaseSceneNode> base_node = pScene->SceneGraph;
			std::shared_ptr<My::BaseSceneNode> StructureNode;
			StructureNode = std::make_shared<My::SceneEmptyNode>(RootNodeName);

			std::shared_ptr<My::BaseSceneNode> LinkNode;
			LinkNode = std::make_shared<My::SceneEmptyNode>("LinkNode");


			ProcessNCData(LinkNode, *pScene, buf);

			base_node->AppendChild(std::move(StructureNode));
			return pScene;
		}

	private:
		NCParser(const NCParser& rhs);
		NCParser& operator=(const NCParser& rhs);
	};
}
