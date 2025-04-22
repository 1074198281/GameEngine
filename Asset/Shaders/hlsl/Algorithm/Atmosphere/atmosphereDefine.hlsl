#define Rayleigh  (float3(5.802, 13.558, 33.1) * 1e-6)
#define Mie  (float3(3.996, 3.996, 3.996) * 1e-6)

struct AtmosphereParam
{
    float Rayliegh_Scalar_Height;   // scalar height is 8500
    float Mie_Scalar_Height;        // scalar height is 1200
    float Mie_Anisotropy_G;
    float padding0;
};

