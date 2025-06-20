/*
���ڿ����ܶȵ����ʣ�������������ɫ�Ĺ��в�ͬ��ɢ���������ɢ��ϵ��ͨ���ǲ����͸߶ȵĺ�����
���в�����������ɢ������ɫ���߶Ⱦ����˴������ܶȣ��ܶȵĴ�С������ɢ����ʵĴ�С��ɢ���
��Խ��ɢ���������ԽС���У�
��(��,h) = ��(��,0)��(h)����(��,h)Ϊɢ�亯������(��,0)Ϊ��ƽ�洦�Ĳ�������Ϊ�߶��ܶȺ�����
���ڲ�ͬ�����ɢ�䣬��Ҫ�ο���λ����phase(��)����λ���������˾���ɢ������������ĳ����
�ķֲ���

��������ɢ���У�
��(��,0) = float3(5.802, 13.558, 33.1) * 1e-6
��(h) = exp(-(h/H_R))������H_RΪ��׼�߶ȣ�����ɢ��ı�׼�߶�Ϊ8500
����ɢ�����λ����Ϊ��
phase(��)=3*(1 + cos^2(��))/(16*��)

��������ɢ���У�
��(��,0) = float3(3.996) * 1e-6
��(h) = exp(-(h/H_R))������H_RΪ��׼�߶ȣ�����ɢ��ı�׼�߶�Ϊ1200
���������� g�������� [-1,1] ֮�䣬������������ɢ�䲨�����״��Ҳ�ǿ������δ�С�Ĳ���
��ֵ������ǰɢ��Ĺ�������ֵ�������ɢ��Ĺ���
����ɢ�����λ����Ϊ��
            3     1 - g^2             1 + cos^2(��)
phase(��)=  -- * --------- * ---------------------------------
            8��   2 + g^2      (1 + g^2 - 2g * cos(��))^1.5
*/

#define PI 3.14159265358979
#include "AtmosphereDefine.hlsl"

// ����ɢ���ɢ��ϵ��
float3 RayleighCoefficient(AtmosphereParam param, float h)
{
    float rho = exp(-(h / param.Rayliegh_Scalar_Height));
    return Rayleigh * rho;
}

// ����ɢ�����λ����
float RayleighPhase(float cos_theta)
{
    return 3 * (1 + pow(cos(cos_theta), 2)) / (16 * PI);
}

// ����ɢ���ɢ��ϵ��
float3 MieCoefficient(AtmosphereParam param, float h)
{
    float rho = exp(-(h / param.Mie_Scalar_Height));
    return Mie * rho;
}

float MiePhase(AtmosphereParam param, float cos_theta)
{
    float a = 3 / (8 * PI);
    float b = (1 - param.Mie_Anisotropy_G * param.Mie_Anisotropy_G) / (2 + param.Mie_Anisotropy_G * param.Mie_Anisotropy_G);
    float c = 1 + pow(cos(cos_theta), 2);
    float d = pow((1 + param.Mie_Anisotropy_G * param.Mie_Anisotropy_G - 2 * param.Mie_Anisotropy_G * cos(cos_theta)), 1.5);
    return a * b * (c / d);
}