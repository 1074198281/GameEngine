/*
由于空气密度的性质，大气对三种颜色的光有不同的散射量，因此散射系数通常是波长和高度的函数。
其中波长决定的了散射光的颜色，高度决定了大气的密度，密度的大小体现了散射概率的大小，散射概
率越大，散射出射能量越小。有：
σ(λ,h) = σ(λ,0)ρ(h)，σ(λ,h)为散射函数，σ(λ,0)为海平面处的波长，ρ为高度密度函数。
对于不同方向的散射，需要参考相位函数phase(θ)；相位函数描述了经过散射后，向各个方向的出射光
的分布。

对于瑞利散射有：
σ(λ,0) = float3(5.802, 13.558, 33.1) * 1e-6
ρ(h) = exp(-(h/H_R))，其中H_R为标准高度，瑞利散射的标准高度为8500
瑞利散射的相位函数为：
phase(θ)=3*(1 + cos^2(θ))/(16*π)

对于米氏散射有：
σ(λ,0) = float3(3.996) * 1e-6
ρ(h) = exp(-(h/H_R))，其中H_R为标准高度，米氏散射的标准高度为1200
各向异性项 g，它介于 [-1,1] 之间，用来控制米氏散射波瓣的形状。也是控制日晕大小的参数
正值增加向前散射的光量；负值增加向后散射的光量
米氏散射的相位函数为：
            3     1 - g^2             1 + cos^2(θ)
phase(θ)=  -- * --------- * ---------------------------------
            8π   2 + g^2      (1 + g^2 - 2g * cos(θ))^1.5
*/

#define PI 3.14159265358979
#include "AtmosphereDefine.hlsl"

// 瑞利散射的散射系数
float3 RayleighCoefficient(AtmosphereParam param, float h)
{
    float rho = exp(-(h / param.Rayliegh_Scalar_Height));
    return Rayleigh * rho;
}

// 瑞利散射的相位函数
float RayleighPhase(float cos_theta)
{
    return 3 * (1 + pow(cos(cos_theta), 2)) / (16 * PI);
}

// 米氏散射的散射系数
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