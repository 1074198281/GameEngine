struct PresentOut
{
    float4 position : SV_POSITION;
    float2 texUV : TEX;
};

SamplerState LinearWarp : register(s16);
SamplerState LinearClamp : register(s17);
SamplerState PointWarp : register(s18);
SamplerState PointClamp : register(s19);
SamplerState AnisotropicWarp : register(s20);
SamplerState AnisotropicClamp : register(s21);

Texture2D<float4> PresentMap : register(t0);

cbuffer OverlaySubParams : register(b0)
{
    float2 _ScreenSize;
    float _Time;
    float _padding0;
};

float3 noiseGrid(float2 uv)
{
    float pt = uv.x * 0.12345 + uv.y * 0.54321;
    float3 pt3 = frac(float3(pt * 0.98765, pt * 0.87654, pt * 0.76543));
    pt3 += dot(pt3, pt3.yzx);
    pt3 = frac(float3((pt3.x + pt3.y) * pt.x, (pt3.y + pt3.z) * pt3.x, (pt3.z + pt3.x) * pt3.y));
    
    return pt3;
}

float noise(float n)
{
    return frac(sin(n * 12345.678) * 87654.321);
}

float staticDrops(float2 uv, float t)
{
    uv *= 20;
    float2 id = floor(uv);
    float3 noise = noiseGrid(id);
    
    uv = frac(uv) - 0.5;
    float2 p = (noise.xy - 0.5) * 0.7;
    float radius = length(uv - p);
    
    float fade = frac(0.002 * t + noise.z);
    float c = smoothstep(0.3, 0, radius) * frac(noise.z * 10) * fade;
    
    return c;
}

float2 dynamicDrops(float2 uv, float t)
{
    float2 originUV = uv;
    uv.y -= 0.002 * t;
    
    float2 scale = float2(6, 1);
    float2 scale2 = scale * 2;
    
    float2 id = floor(uv * scale2);
    uv.y -= noise(id.x);
    
    id = floor(uv * scale2);
    float3 noise3 = noiseGrid(id);
    float2 st = frac(uv * scale2);
    
    float d = length(st);
    
    return d;
    return float2(0, 0);
}

float2 drops(float2 uv, float t)
{
    float sd = staticDrops(uv, t);
    float2 dd = dynamicDrops(uv, t);
    
    return dd;
    return float2(sd, sd) + dd;
}

float4 main(PresentOut PresentIn) : SV_Target0
{
    float2 uv = float2(PresentIn.texUV.x, PresentIn.texUV.y);
    uv.x *= _ScreenSize.x / _ScreenSize.y;
    
    float t = _Time;
    
    float2 c = drops(uv, t);
    return c.x;
    
    float2 finalUV = PresentIn.texUV + c;
    //finalUV.x /= (_ScreenSize.x / _ScreenSize.y + 1);
    //finalUV.y /= 2;
    return PresentMap.Sample(AnisotropicWarp, finalUV);
}