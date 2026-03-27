#include <vector>

#include "Waves.h"
#include "../SceneGraph/SceneObjectTypeDef.hpp"


using namespace My::PCG;

Waves::Waves(std::string name)
{
	m_Name = name;
	m_pGeoNode = std::make_shared<SceneGeometryNode>();
	m_pGeoObject = std::make_shared<SceneObjectGeometry>();
}

Waves::~Waves()
{

}

void Waves::SetWavesParam(double x, double y, double centerX, double centerY, double centerZ, double stepX, double stepY,
	double waveLength, double waveAmplitude, double waveSpeed, double waveDir)
{
	m_dSpaceX = x;
	m_dSpaceY = y;
	m_dCenterX = centerX;
	m_dCenterY = centerY;
	m_dCenterZ = centerZ;
	m_dStepX = stepX;
	m_dStepY = stepY;
	m_dWaveLength = waveLength;
	m_dAmplitude = waveAmplitude;
	m_dSpeed = waveSpeed;
	m_dDirection = waveDir;

	GenerateWavesMesh();
}

void Waves::GenerateWavesMesh()
{
	double dStepX = m_dSpaceX / m_dStepX;
	double dStepY = m_dSpaceY / m_dStepY;
	double dMinX = m_dCenterX - m_dSpaceX / 2;
	double dMaxX = m_dCenterX + m_dSpaceX / 2;
	double dMinY = m_dCenterY - m_dSpaceY / 2;
	double dMaxY = m_dCenterY + m_dSpaceY / 2;

	int vertexCount = ((m_dStepX + 1) * (m_dStepY + 1));

	m_Vertex.reserve(vertexCount);
	double y = dMinY;
	double x = dMinX;
#ifdef _DEBUG
	int v_count = 0;
#endif // _DEBUG

	for (; y <= dMaxY + EP; y += dStepY)
	{
		for (; x <= dMaxX + EP; x += dStepX)
		{
			Vector3f _v = Vector3f(x, m_dCenterZ, y);
			m_Vertex.push_back(_v);

#ifdef _DEBUG
			//v_count++;
			//std::cout << "vertex " << v_count << " : " << _v.x << ", " << _v.y << ", " << _v.z << std::endl;
#endif
		}

		x = dMinX;
	}
	SceneObjectVertexArray _v_array("POSITION", 0, kVertexDataTypeFloat3, m_Vertex.data(), 3 * (m_dStepX + 1) * (m_dStepY + 1));
	
	//m_VertexColor.reserve(vertexCount);
	//for (int v = 0; v < vertexCount; v++) {
	//	m_VertexColor.push_back(Vector3f(0, 0, 255));
	//}
	//SceneObjectVertexArray _v_color_array("COLOR0", 0, kVertexDataTypeFloat3, m_VertexColor.data(), 3 * (m_dStepX + 1) * (m_dStepY + 1));

	m_Indice.reserve(2 * m_dStepX * m_dStepY);
	for (int sx = 0; sx < m_dStepX; sx++) {
		for (int sy = 0; sy < m_dStepY; sy++) {
			Vector3i32 _i_1 = Vector3i32(sx, sx + m_dStepX + 1, sx + 1);
			Vector3i32 _i_2 = Vector3i32(sx + 1, sx + m_dStepX + 1, sx + 1 + m_dStepX + 1);
			m_Indice.push_back(_i_1);
			m_Indice.push_back(_i_2);
		}
	}
	SceneObjectIndexArray _i_array(0, 0, kIndexDataTypeInt32, m_Indice.data(), 3 * 2 * m_dStepX * m_dStepY);

	std::shared_ptr<SceneObjectMesh> _mesh = std::make_shared<SceneObjectMesh>();
	_mesh->AddVertexArray(std::move(_v_array));
	//_mesh->AddVertexArray(std::move(_v_color_array));
	_mesh->AddIndexArray(std::move(_i_array));
	_mesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypeTriList);

	m_pGeoObject->AddMesh(_mesh);
	
	m_pGeoNode->AddSceneObjectRef(m_Name);
	m_pGeoNode->SetVisibility(true);
	m_pGeoNode->SetIfCastShadow(true);
}