#pragma once

#include "../SceneGraph/SceneNodeGeometry.hpp"

namespace My
{
	class Waves
	{
	public:
		Waves();
		~Waves();

	public:
		void SetWavesParam(double x, double y, double centerX, double centerY, double stepX, double stepY,
			double waveLength, double waveAmplitude, double waveSpeed, double waveDir);


	private:


	public:
		SceneGeometryNode* GetWavesNode() { return m_pWavesNode.get(); }
		SceneObjectGeometry* GetWavesObject() { return m_pWavesObject.get(); }

	public:
		double m_dSpaceX;			// 波的X方向大小
		double m_dSpaceY;			// 波的Y方向大小
		double m_dCenterX;			// 中心X
		double m_dCenterY;			// 中心Y
		double m_dCenterZ;			// 中心Z
		double m_dSplitStepX;		// 拆分网格时X方向的步长
		double m_dSplitStepY;		// 拆分网格时Y方向的步长

		double m_dWaveLength;		// 波长L 波峰与波峰直接的距离 波长与角频率关系为 L*ω=2π
		double m_dAmplitude;		// 振幅A 水平面到波峰的高度
		double m_dSpeed;			// 速度S 每秒钟波峰移动的距离 把速度表示成相函数 φ=S*2π/L
		double m_dDirection;		// 方向D 垂直于波阵面的水平向量
	
	private:
		std::unique_ptr<SceneGeometryNode> m_pWavesNode;
		std::unique_ptr<SceneObjectGeometry> m_pWavesObject;
	};
}