#pragma once

#include "PCGCommonObject.h"
#include "../GeomMath/base_math.hpp"



namespace My::PCG
{
	enum WAVE_TYPE
	{
		WAVE_TYPE_SIN = 0,		// 正弦波
		WAVE_TYPE_K_POWER,		// K次幂函数波

		WAVE_TYPE_MAX,
	};


	class Waves : public PCGCommonObject
	{
	public:
		Waves(std::string name);
		~Waves();

	public:
		void SetWavesParam(double x, double y, double centerX, double centerY, double centerZ, double stepX, double stepY,
			double waveLength, double waveAmplitude, double waveSpeed, double waveDir);


	private:
		void GenerateWavesMesh();

	public:
		double m_dSpaceX;			// 波的X方向大小
		double m_dSpaceY;			// 波的Y方向大小
		double m_dCenterX;			// 中心X
		double m_dCenterY;			// 中心Y
		double m_dCenterZ;			// 中心Z
		double m_dStepX;			// 拆分网格时X方向的数量
		double m_dStepY;			// 拆分网格时Y方向的数量

		double m_dWaveLength;		// 波长L 波峰与波峰直接的距离 波长与角频率关系为 L*ω=2π
		double m_dAmplitude;		// 振幅A 水平面到波峰的高度
		double m_dSpeed;			// 速度S 每秒钟波峰移动的距离 把速度表示成相函数 φ=S*2π/L
		double m_dDirection;		// 方向D 垂直于波阵面的水平向量
	
		WAVE_TYPE m_eWaveType;		// 波的类型

	private:
		std::vector<Vector3f> m_Vertex;
		std::vector<Vector3f> m_VertexColor;
		std::vector<Vector3i32> m_Indice;


	};
}