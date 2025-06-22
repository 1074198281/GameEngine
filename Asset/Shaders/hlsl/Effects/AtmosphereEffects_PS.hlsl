//Please don't hate me, if this needs removing let me know.

//Pretty much most of this code is not mine
//I will make attmepts to link to the original shaders if I can find them again
//Apologies to all those that I don't find and apologies it's so messy. I'm sure there are much better ways to put these shaders together.
//Stems from the original...
// The sun, the sky and the clouds. By StillTravelling
// https://www.shadertoy.com/view/tdSXzD

//Ideally I would have like to add a bit of terrain, maybe some planes passing by but my head is far injured for now
//

struct PresentOut
{
    float4 position : SV_POSITION;
    float2 texUV : TEX;
};

cbuffer Constants : register(b0)
{
    float4x4 WorldViewProjection;
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
    float4x4 ViewProjection;
    float3 CameraPosition;
    float iTime;
    float3 iResolution;
    float4 gl_FragCoord;
    float3 iChannelResolution[4];
    float2 iMouse;
};


SamplerState LinearWarp : register(s16);

Texture2D<float4> iChannel0 : register(t0);
Texture2D<float4> iChannel1 : register(t1);
Texture2D<float4> iChannel2 : register(t2);


//precision highp float;
#define cloudy  0.3 //0.0 clear sky density of cloud
#define cld1 0.4 //sort of acts as cloud cover? bigger gap between cld1 and cld2 = more coverage
#define cld2 0.6 //sort of acts as cloud cover?
#define rainvol 0.0 //0.5 //make it rain
#define rainmulti 8.0 // makes clouds darker more density
#define RAINBOW_BRIGHTNESS  0. //show rainbow
#define scatonly 0. //Show certain components only
#define snowvol 0. //Add snow
#define fireworkson 0. // Add fireworks

//cloud directions
#define xaxiscloud -100.0*t //t*5e2 +t left -t right *speed
#define yaxiscloud t*100.0 //0. //hmmm?
#define zaxiscloud 250.0*t //t*6e2 +t away from horizon -t towards horizon *speed


#define auroraINT 1.5
#define lensflare 1.

#define Dfact 0.5 //position of horizon

#define scatter_steps 8
#define scatter_steps2 4

#define TAU 6.283185307179586
#define ANIMATE_CLOUDS 0



#define ENABLE_RAIN 1 //enable rain 
#define ENABLE_DENSE_RAIN 1 //enable DENSITY RAIN
#define ENABLE_DENSE_RAIN2 0
#define ORIG_CLOUD 0
#define SIMPLE_SUN 1
#define ENABLE_MOON 1
#define ENABLE_MOON_2 0
#define ENABLE_SEA 1
#define ENABLE_LENS_FLARE 1
#define ENABLE_BACK_TEXTURE 1
#define EXPENSIVE_CALC 1 //not actually sure if these are expensive calcs!
#define ENABLE_SCAT_OPTIONS 1
#define SOFT_SUN 1
#define NICE_HACK_SUN 1
#define FIREWORK_COUNT 2 //2
#define FIREWORK_DURATION 8.
#define FIREWORK_LOW .75
#define FIREWORK_HIGH 1.05
#define ROCKET_PARTICES 16
#define ROCKET_DURATION 1.5
#define FLASH_DURATION ROCKET_DURATION + .2
#define THRUSTER_SPEED .25
#define EXPLOSION_STRENGTH .03;
#define EXPLOSION_PARTICLES 32 //32


#define cameraheight 5e1 //50m
#define t iTime

#define fov tan(radians(60.0))

#define moonspec 10.
#define moonillumination 1.0 //
#define moon_rotate  0.
#define moonlod 2.
#define MoonINT 2.


#define Rm 120.0


#define rr 0.0000059
#define rg 0.0000121
#define rb 0.0000182
//#define haze 0.01 * (cloudy*20.);
//#define haze  custom5 //0.01 * (cloudy*20.)
#define haze 0.01
#define g0 0.76

#define RI0 20. //Rayleigh Intensity


#define mincloudheight 5000. //5e3 //5000m
#define maxcloudheight 8000. //8e3 affects performance
#define Hr 7.994e3
#define Hm 1.2e3
#define R0 6361e3 //planet radius //6360e3 - 6361e3
#define Ra 6369e3 //atmosphere radius //6380e3 -6369e3
#define rorder 0.
#define SI 10. //Sun Intensity
#define MI0 8. //Mie Intensity


#define bM float3(0.000051, 0.000051, 0.000051)
#define cloudnoise 0.0002 //4e-4 //2e-4
#define phaseR1 0.0596831
#define phaseM1 0.1193662


#define aurora_mt 1. //10

#define aurora_noise 0.06 //0.1
#define aurora_steps 50 //5.
#define aurora_height 0. //1e-5
#define aurorardy -0.05
#define aurora2 0.
#define aurorardz 0. //0
#define aurora_f 0.065 //0.065
#define aurora_sy 1. //0.065

#define aurora_col 0.043 //0.043
#define wavenoise 0.003
#define wave_f 10.
#define wave_t1 4.
#define wave_t2 -1.
#define aur_style 0.
#define cloudtype 0.
#define add_noise 0.

#define eclispethreshold 0.05
#define camera_height 10.
#define camera_H_rotate radians(0.)
#define camera_V_rotate radians(0.)
#define camera_T_rotate radians(0.)
#define camera_depth 0.
#define aurora_ss 5. //5.
#define aurora_ss2 0.002 //15.
#define aurora_of 0.006 //0.006
#define res 1.

//#define 0.0 custom70
#define h1 100.
#define h2 1000.




#define aurora_r 2.15 //2.15
#define aurora_g -1. // -0.5
#define aurora_b 1. // 1.2

#define MIMin1  8.0
#define SEA_HEIGHT 0.6
#define SEA_CHOPPY 4.0
#define SEA_SPEED 0.8
#define SEA_FREQ 0.16
#define SEA_BASE float3(0.1,0.19,0.22)


#define camz 0.001
#define tonemap 9. // not really tonemap, just a quick way to test
#define rb1 1.25
//#define rb3 0.
#define HmMin1 1200.
#define SEA_NUM_STEPS 7.
#define aa1 -1.
#define aa2 0.
#define sat 1.1
#define vib 0.
#define hue 0.
#define spec 40.
#define winddirx 0.
#define rain1 4.
#define rain2 0.05
#define rain3 0.0
#define rain4 0.05
#define rain5 0.1
#define rain6 10000.
#define rain7 1.
#define rain8 0.01
#define rain9 1000.
#define rain10 1000.
#define rain11 0.4
#define rain12 0.6
#define rain13 0.25
#define raleighgam 2.2
#define miegam 1.6
#define raleighgammin 1.0
#define miegamin 1.6
#define specmin 20.
#define sealight 1.
#define specbrightness 0.5
#define mietype 0.


#define lensflareb 1.
#define lensgam 1.4
#define lensflaretype 2.

#define spectype 1.
#define diffusepow 80.
#define RRMin 0.0000025
#define RRMax 0.0000058

#define round(x, f) (floor((x)/(f) + 0.5) * (f))
#define C1(x) clamp(x, 0., 1.)
#define S1(a, b, x) smoothstep(a, b, x)
#define F(x, f) (floor(x * f) / f)
#define saturate(x)	clamp(x, 0.0, 1.0)

#define Argb float3(aurora_r,aurora_g,aurora_b)
#define C  float3(0., -R0, 0.) //planet center

#define ZERO_2 float2(0.0, 0.0)
#define ZERO_3 float3(0.0, 0.0, 0.0)
#define ZERO_4 float4(0.0, 0.0, 0.0, 0.0)
#define ONE_3  float3(1.0, 1.0, 1.0)
#define TWO_POINT_TWO_3 float3(2.2, 2.2, 2.2)
#define INV_TWO_POINT_TWO_3 float3(1.0/2.2, 1.0/2.2, 1.0/2.2)
#define SCAT_3 float3(scat, scat, scat)
#define ONE_MIN_SCAT_3 float3(1.0 - scat, 1.0 - scat, 1.0 - scat)


//const float R0 = 6361e3; //planet radius //6360e3 - 6361e3
//const float Ra = 6369e3; //atmosphere radius //6380e3 -6369e3
const int steps = 8; //16 is fast, 128 or 256 is extreme high
const int stepss = 4; //16 is fast, 16 or 32 is high

const float I = 10.; //sun light power, 10.0 is normal

//const float RI = 10.; //Rayleigh Intensity
//const float MI = 10.; //Mie Intensity
//const float SI = 5.; //Sun Intensity

const float ng = 0.05; //light concentration for sky.76 //.45 //.6  .45 is normaL
const float ng2 = ng * ng;

const float s = 0.999; //light concentration for sun
#if SOFT_SUN
const float s2 = s;
#else
const float s2 = s * s;
#endif

const float moon_s = 0.999; //light concentration for moon
#if SOFT_SUN
const float moon_s2 = moon_s;
#else
const float moon_s2 = moon_s * moon_s;
#endif

//const float Hr = 7.994e3; //Rayleigh scattering top //8e3
//const float Hm = 1.2e3; //Mie scattering top //1.3e3
//Note that there is a specific scale height value HM for the Mie scattering which is usually set to 1.2 km

//float3 bM = float3(21e-6); //normal mie // float3(21e-6)
//float3 bM = float3(50e-6); //high mie
float3 bM2 = float3(5e-6, 5e-6, 5e-6);


//Rayleigh scattering (sky color, atmospheric up to 8km)
//float3 bR = float3(5.5e-6, 13.0e-6, 22.4e-6); //normal earth
float3 bR = float3(5.8e-6, 13.5e-6, 33.1e-6); //original earth
//float3 bR = float3(5.8e-6, 20.5e-6, 33.1e-6); //redder sunset
//float3 bR = float3(0.0000058,0.0000135,0.0000331);
//float3 bR = float3(3.8e-6, 13.5e-6, 33.1e-6);
//float3 bR = float3(5.8e-6, 33.1e-6, 13.5e-6); //purple
//float3 bR = float3( 63.5e-6, 13.1e-6, 50.8e-6 ); //green
//float3 bR = float3( 13.5e-6, 23.1e-6, 115.8e-6 ); //yellow
//float3 bR = float3( 5.5e-6, 15.1e-6, 355.8e-6 ); //yeellow
//float3 bR = float3(3.5e-6, 333.1e-6, 235.8e-6 ); //red-purple

//Wfloat3 C = float3(0., -R0, 0.); //planet center
float3 Ds = normalize(float3(0., .09, -1.)); //sun direction?
float3 Dsm = normalize(float3(0., .09, -1.)); //moon direction?

const float PI = 3.14159265358979323846;
const float PI_2 = 1.57079632679489661923;
const float PI_4 = 0.785398163397448309616;


float cloudyhigh = 0.05; //if cloud2 defined
#if ORIG_CLOUD
float cloudnear = 1.0; //9e3 12e3  //do not render too close clouds on the zenith
float cloudfar = 1e3; //15e3 17e3
#else
float cloudnear = 1.0; //15e3 17e3
float cloudfar = 160e3; //9e3 12e3  //do not render too close clouds on the zenith
#endif

#define S(x, y, z) smoothstep(x, y, z)

float3x3 rotateX(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return float3x3(
        float3(1, 0, 0),
        float3(0, c, -s),
        float3(0, s, c)
    );
}

// Rotation matrix around the Y axis.
float3x3 rotateY(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return float3x3(
        float3(c, 0, s),
        float3(0, 1, 0),
        float3(-s, 0, c)
    );
}

// Rotation matrix around the Z axis.
float3x3 rotateZ(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return float3x3(
        float3(c, -s, 0),
        float3(s, c, 0),
        float3(0, 0, 1)
    );
}
//AURORA_STUFF
float2x2 mm2(in float a){float c = cos(a), s = sin(a);return float2x2(c,s,-s,c);}
float2x2 m2 = float2x2(0.95534, 0.29552, -0.29552, 0.95534);
float tri(in float x){return clamp(abs(frac(x)-.5),0.01,0.49);}
float2 tri2(in float2 p){return float2(tri(p.x)+tri(p.y),tri(p.y+tri(p.x)));}

float triNoise2d(in float2 p, float spd)
{
    float z=1.8;
    float z2=2.5;
	float rz = 0.;
    p *= mm2(p.x * 0.06)._m00_m01;
    float2 bp = p;
	for (float i=0.; i<5.; i++ )
	{
        float2 dg = tri2(bp*1.85)*.75;
        dg *= mm2(t * spd)._m00_m01;
        p -= dg/z2;

        bp *= 1.3;
        z2 *= .45;
        z *= .42;
		p *= 1.21 + (rz-1.0)*.02;
        
        rz += tri(p.x+tri(p.y))*z;
        p *= -m2._m00_m01;
    }
    return clamp(1./pow(rz*29., 1.3),0.,.55);
}


float3 bg(in float3 rd)
{
    float sd = dot(normalize(float3(-0.5, -0.6, 0.9)), rd)*0.5+0.5;
    sd = pow(sd, 5.);
    float3 col = lerp(float3(0.05,0.01,0.15), float3(0.01,0.05,0.15), sd);
    return col*.63;
}



float hash21(in float2 n){ return frac(sin(dot(n, float2(12.9898, 4.1414))) * 43758.5453); }

void aurora(float3 rayorigin, float3 raydirection, out float3 bpos, out float4 col)
{
    //float4 col = float4(0.);
	//float3x3 rotY = rotateX(rain9);
	float3x3 rotY = rotateX(-0.25);
    float4 avgCol = float4(0.0, 0.0, 0.0, 0.0);
    rayorigin.z += t*aurora_height;
	//rayorigin *= fov;
    //raydirection.y += -0.25;
	raydirection.y +=0.1;
	//raydirection.y +=aurorardy;
    raydirection.y *= aurora_sy;
    //raydirection.z += aurorardz;
	raydirection.z += -0.3;
    raydirection = mul(raydirection, rotY);
	


    float mt = aurora_mt;
    float ms = 50.;
    for(int i=0;i<aurora_steps;i++)


        {

            float of = aurora_of * hash21(gl_FragCoord.xy) * smoothstep(0.,15., float(i) * mt);
            float pt = ((.8 + pow(float(i),1.4) * aurora_ss2) - rayorigin.y) / (raydirection.y * 2. + 0.4);
            pt -= of;
            bpos = rayorigin + pt * raydirection ;
			//bpos *= fov;
            float2 p = bpos.zx +sin(t*0.01);
            float rzt = triNoise2d(p, aurora_noise);
            float4 col2 = float4(0,0,0, rzt);
            col2.rgb = (sin(1. -float3(aurora_r, aurora_g, aurora_b) + (float(i) * mt) * aurora_col) * 0.5 + 0.5) * rzt;
            avgCol =  lerp(avgCol, col2, .5);
            col += avgCol * exp2( (-float(i)*mt) * aurora_f - 2.5) * smoothstep(0., aurora_ss, float(i)*mt);
        }
    col *= clamp(raydirection.y * 15. +0.4, 0., 1.);
	col *= auroraINT;
    //return col*auroraINT; //col*2.
}

//END AURORA_STUFF

//SIMPLE SUN STUFF
const float density = 0.5;
const float3 skyColor = float3(0.37, 0.55, 1.0) * (1.0 + 0.0);


float getSunPoint(float2 p, float2 lp){
    return smoothstep(0.036*(fov/2.0), 0.001*(fov/2.0), distance(p, lp)) * 100.0;
}

float getMie(float2 p, float2 lp){
    float mytest = lp.y < 0.5 ? (lp.y+0.5)*pow(0.05,20.0):0.05;
    float disk = clamp(1.0 - pow(distance(p, lp), mytest), 0.0, 1.0);
    return disk*disk*(3.0 - 2.0 * disk) * 0.25 * PI;
}

float3 getSkyAbsorption(float3 x, float y){
    float3 absorption = x * y;
    absorption = pow(absorption, 1.0 - (y + absorption) * 0.5) / x / y;
    return absorption;
}

float3 jodieReinhardTonemap(float3 c){
    float l = dot(c, float3(0.2126, 0.7152, 0.0722));
    float3 tc = c / (c + 1.0);
    return lerp(c / (l + 1.0), tc, tc);
}

float3 getAtmosphericScattering(float2 p, float2 lp, float zenithOffset){
    float zenithnew = density / pow(max(p.y - zenithOffset, 0.0035), 0.75); //zenithDensity(p.y);
    float sunPointDistMult =  clamp(length(max(lp.y + 0.1 - zenithOffset, 0.0)), 0.0, 1.0);
    float3 absorption = getSkyAbsorption(skyColor, zenithnew);
    float3 sunAbsorption = getSkyAbsorption(skyColor, density / pow(max((lp.y + 0.2) - zenithOffset, 0.0035), 0.15)); //zenithDensity(lp.y + 0.2));
    float3 sun3 = getSunPoint(p, lp) * absorption;
    float3 totalSky = sun3; //+ mie2;
    totalSky *= sunAbsorption * 0.5 + 0.5 * length(sunAbsorption);
    float3 newSky = jodieReinhardTonemap(totalSky);
    return newSky;
}
//END SIMPLE SUN STUFF

float3 hash33(float3 p)
{
    p = frac(p * float3(443.8975,397.2973, 491.1871));
    p += dot(p.zxy, p.yxz+19.27);
    return frac(float3(p.x * p.y, p.z*p.x, p.y*p.z));
}


float2 hash21(float p)
{
    float3 p3 = frac(float3(p, p, p) * float3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx + 33.33);
    return frac((p3.xx + p3.yz) * p3.zy);

}

float3 hash31(float p)
{
    float3 p3 = frac(float3(p,p,p) * float3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx+33.33);
    return frac((p3.xxy+p3.yzz)*p3.zyx);
}

float hash11(float p)
{
    p = frac(p * .1031);
    p *= p + 33.33;
    p *= p + p;
    return frac(p);
}

float hashf(float3 p3){
    p3 = frac(p3 * 0.1031);
    p3 += dot(p3,p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

float noisef(float3 x){
    float3 i = floor(x);
    float3 f = frac(x);
    f = f*f*(3.0-2.0*f);
    return lerp(lerp(lerp(hashf(i+float3(0, 0, 0)), 
                       hashf(i+float3(1, 0, 0)),f.x),
                   lerp(hashf(i+float3(0, 1, 0)), 
                       hashf(i+float3(1, 1, 0)),f.x),f.y),
               lerp(lerp(hashf(i+float3(0, 0, 1)), 
                       hashf(i+float3(1, 0, 1)),f.x),
                   lerp(hashf(i+float3(0, 1, 1)), 
                       hashf(i+float3(1, 1, 1)),f.x),f.y),f.z);
}

float remap(float x, float a, float b, float c, float d)
{
    return (((x - a) / (b - a)) * (d - c)) + c;
}


float random(float p)
{
    return frac(52.043*sin(p*205.429));
}
float random2(float p)
{
    return random(p)*2.0-1.0;
}


float3 meteor(float2 uv, float gtime, float delay)
{
    float seed = round(gtime, delay);

    float startTime = (delay - 1.5) * random(seed);
    float time = max(0.0, min(1.0, gtime-seed - startTime));

    float2 start = float2(
        random2(seed),
        0.7 + 0.3 * random(seed+0.1)
    );

    float2 end = start * 0.5;

    uv = uv - lerp(start, end, time);

    end = normalize(end - start);
    uv = mul(uv, float2x2(end.x, end.y, -end.y, end.x));
    uv.x *= 0.1;

    float alpha = 16.0 * pow(time, 2.0) * pow(time - 1.0, 2.0);
    float v = max(0.0, alpha - iResolution.y * length(uv));
    return float3(v, v, v);
}

float3 meteorstorm(float2 uv)
{
    return
        meteor(uv, iTime, 9.5837) +
        meteor(uv, iTime + 15.3, 15.459) +
        meteor(uv, iTime + 125.0, 31.2);
}

void fireworks(in float2 st, out float MI2, out float3 fire, out float3 fireworkCol2)
{
    float2 fireworkPos, particlePos;
    float radius, theta, radiusScale, spark, sparkDistFromOrigin, shimmer,
    shimmerThreshold, fade, fade2, timeHash, timeOffset, rocketPath;
    float3 particleHash, fireworkHash, fireworkCol,finalCol;

    for (int j = 0; j < FIREWORK_COUNT; ++j)

    {
        timeHash = hash11(float(j + 1) * 9.6144 + 4098.6118);
        // original timeHash = hash11(float(j + 1) * 9.6144 + 78.6118);
        timeOffset = float(j + 1) + float(j + 1) * timeHash;

        // This hash changes after each firework cycle (rocket + explosion)
        fireworkHash = hash31(471.5277 * float(j) + 1226.9146 + float(int((iTime+timeOffset) / FIREWORK_DURATION))) * 2. - 1.;
        fireworkCol = fireworkHash * .5 + .5;
        fireworkHash.y = remap(fireworkHash.y, -1., 1., FIREWORK_LOW, FIREWORK_HIGH);

        // Random firework x coordinate but confined to a certain column based on j
        fireworkHash.x = ((float(j) + .5 + fireworkHash.x * .50)/ float(FIREWORK_COUNT)) * 2. - 1.;
        // original fireworkHash.x = ((float(j) + .5 + fireworkHash.x * .25)/ float(FIREWORK_COUNT)) * 2. - 1.;

        // Duration of each firework with a random start time
        float time = fmod(iTime + timeOffset, FIREWORK_DURATION);
         if (time > ROCKET_DURATION)
        {
            //fireworkCol2 = fireworkCol;
            fireworkPos = float2(fireworkHash.x, fireworkHash.y);

    for (int i = 0; i < EXPLOSION_PARTICLES; ++i)
            {
                // Unique hash that yeilds a separate spread pattern for each explosion
                particleHash = hash31(float(j) * 1291.1978 + float(i) * 1619.8196 + 469.7119);
                theta = remap(particleHash.x, 0., 1., 0., TAU); // [0, 2.*PI]
                radiusScale = particleHash.y * EXPLOSION_STRENGTH;

                // Radius expands exponentially over time, i.e. explosion effect
                radius = radiusScale * time * time;

                particlePos = float2(radius * cos(theta), radius * sin(theta));
                particlePos.y -= 8. * pow(particlePos.x, 4.); // fake-ish gravity
                spark = .0003 / pow(length(st - particlePos - fireworkPos), 1.7);
                sparkDistFromOrigin = 2. * length(fireworkPos - particlePos);

                // Shimmering effect for explosion particles
                shimmer = max(0., sqrt(sparkDistFromOrigin) * (sin((iTime + particleHash.y * TAU) * 18.)));
                shimmerThreshold = FIREWORK_DURATION * .6;



                // Fade after a certain time threshold
                fade = C1((FIREWORK_DURATION * 2.) * radiusScale - radius);
				fade2 += fade;			
				
                if(shimmer > 0. && fade>0.){
					
                    fireworkCol2 = fireworkCol;
				}
                finalCol += clamp(lerp(spark, spark * shimmer, smoothstep(shimmerThreshold
					* radiusScale, (shimmerThreshold + 1.) * radiusScale , radius))
                    * fade * fireworkCol,0.0,1.);

            }

             MI2 = fade2;
            // Initial instant flash for the explosion
            if(time < FLASH_DURATION)
            	finalCol += clamp(spark / (.01 + fmod(time, ROCKET_DURATION)),0.,1.);



         }
        else
        {
            rocketPath = fmod(time, ROCKET_DURATION) / ROCKET_DURATION;
            // ease out sine
            rocketPath = sin(rocketPath / (ROCKET_DURATION * .75) * PI * .5);
            fireworkPos = float2(fireworkHash.x, rocketPath * fireworkHash.y);


            // Slight random wiggle for the rocket's path
            fireworkPos.x += sin(st.y * 50. + time) * fireworkCol.z * .0035;

            // Rockets flying before the explosion
            for (int i = 0; i < ROCKET_PARTICES; ++i)
            {
                particleHash = hash31((float(i) * 603.6837) + 1472.3486);

                // rocket trail size
                float t = time * (2. - time);
                radius = fmod(time + particleHash.y, THRUSTER_SPEED) / THRUSTER_SPEED * particleHash.z * .1;

                // Confine theta to a small value for a vertical thrust effect
                theta = remap(particleHash.x, 0., 1., 0., PI * .1) + PI * 1.45;
                particlePos = float2(radius * cos(theta), radius * sin(theta));
                finalCol += clamp(8e-5 / pow(length(st - particlePos - fireworkPos), 1.1)
                    * lerp(float3(1.4, 0.7, 0.2), float3(1.4, 1.4, 1.4), radius * 16.0), 0.0, 10.);
            }
        }
    }


    fire =finalCol;
    //MI2 = 5.;
    MI2 = clamp(MI2,0.,1000.);
}

float3 stars(in float3 p)
{
    float3 c = float3(0.,0.,0.0);
    float res0 = res / 2.0;
    float res1 = 0.;
    if (iResolution.x <iResolution.y){
        res1 = iResolution.y * res0;
    }
    else{
        res1 = iResolution.x * res0;
    }

    for (int i=0;i<4;i++)
    {
        float3 q = frac(p*(.15*res1))-0.5;
        float3 id = floor(p*(.15*res1*(1./res0)));
        float2 rn = hash33(id).xy;
        float c2 = 1.-smoothstep(0.,.6,length(q));
        c2 *= step(rn.x,.0005+float(i)*float(i)*0.001);
        c += c2*(lerp(float3(1.0,0.49,0.1),float3(0.75,0.9,1.),rn.y)*0.1+0.9);
        p *= 1.3;
    }
    return c*c*.8;
}


//tag: change
//float noise(in float2 v) { return textureLod(iChannel0, (v+.5)/256., 0.).r; }
float noise(in float2 v)
{
    return iChannel0.SampleLevel(LinearWarp, (v + .5) / 256., 0.).r;
}

// by iq
float noise(in float3 v) {
	float3 p = floor(v);
    float3 f = frac(v);
	//f = f*f*(3.-2.*f);
	
	float2 uv = (p.xy+float2(37.,17.)*p.z) + f.xy;
	//float2 rg1 = textureLod( iChannel0, (uv+.5)/256., -100.).yx;
    float2 rg1 = iChannel0.SampleLevel(LinearWarp, (uv + 0.5) / 256.0, -100.0).yx;
	return lerp(rg1.x, rg1.y, f.z);
}

float Noise2( in float3 x )
{
    float3 p = floor(x);
    float3 f = frac(x);
	f = f*f*(3.0-2.0*f);

	float2 uv = (p.xy+float2(37.0,17.0)*p.z) + f.xy;
	//float2 rg1 = texture( iChannel0, (uv+ 0.5)/256.0, -100.0).yx;
    float2 rg1 = iChannel0.SampleLevel(LinearWarp, (uv + 0.5) / 256.0, -100.0).yx;
	return lerp( rg1.x, rg1.y, f.z );
}

float fnoise(in float3 v ) {
#if ANIMATE_CLOUDS
	return
		.55 * noise(v) +
		.225 * noise(v*2. + (t +(t*0.0)) *.04) +
		.125 * noise(v*3.99) +
		.0625 * noise(v*8.9);
#else
	return
		.55 * noise(v) +
		.225 * noise(v*2.) +
		.125 * noise(v*3.99) +
		.0625 * noise(v*8.9);
#endif
}

float fnoise2( float3 p, in float t )
{
	p *= .25;
    float f;

	f = 0.5000 * Noise2(p); p = p * 3.02; p.y -= t*.02;
	f += 0.2500 * Noise2(p); p = p * 3.03; p.y += t*.06;
	f += 0.1250 * Noise2(p); p = p * 3.01;
	f += 0.0625   * Noise2(p); p =  p * 3.03;
	f += 0.03125  * Noise2(p); p =  p * 3.02;
	f += 0.015625 * Noise2(p);
    return f;
}



float cloud(float3 p, in float t ) {
    float cld =0.;
    cld = fnoise2(p*cloudnoise,t) + cloudy*0.1;
    cld = smoothstep(cld1+.04, cld2+.04, cld);
    cld *= cld * (5.0*rainmulti);
    return cld+haze;
}

void densities(in float3 pos, in float highclouds, out float rayleigh, out float mie, in float Hm1, in float height_off) {

    float3 d = pos;
    d *=fov;
	//d *=rotateX(0.5);
    //float3 dN = normalize(float3(pos.x,pos.y,fov));
    d.y = 0.0;
    float dist = length(d); //z distance i think

    float cld = 0.;
    float godray =0.;
    float test =0.;
    float test2 =0.;
    float test3 =0.;
    float AR = iResolution.x/iResolution.y;
	float h = length(pos - C) - R0;

    //density_l.z = clamp((1.0 / cosh((height_absorption - height_l) / absorption_falloff)) * density_l.x, 0.0, 1.0);
	if(dist < 500000.){
    if (mincloudheight < h && h < maxcloudheight) {
        //cld = cloud(pos+float3(t*1e3,0., t*1e3),t)*cloudy;
        cld = cloud(pos + float3(xaxiscloud,yaxiscloud, zaxiscloud),t)*cloudy;
        cld *= sin(3.1415*(h-mincloudheight)/mincloudheight) * cloudy;

    }

    #ifdef cloud2
    float cld2 = 0.;
    if (12e3 < h && h < 15.5e3) {
        cld2 = fnoise2(pos*3e-4,t)*cloud(pos*32.0+float3(27612.3, 0.,-t*15e3), t);
        cld2 *= sin(3.1413*(h-12e3)/12e3) * cloudyhigh;
        cld2 = clamp(cld2,0.0,1.0);
    }
    #endif

        float factor = clamp(1.0-((dist - cloudfar)/(cloudfar-cloudnear)),0.,10.);
        cld *= factor;
		


    //FAKE HIGH CLOUDS
    if(highclouds >0.){
    if(d.y > 1000. && d.z > -20000.){
        float2 uv00 = (gl_FragCoord.xy / iResolution.xy);
        float2 uv3 = uv00 * (2.0) - (1.0);
        uv3.x *=AR;
        uv3.y +=Dfact;
        float cldhighvol = cloudy + 0.5;
        float2 st0 =  (uv3 * float2(.5 +(uv00.y +2.) *.75, 10.) + float2(0. + uv00.y *10., t*0.05)) * 0.05;
        float f0 = iChannel0.SampleLevel(LinearWarp, st0, -100.0).y * iChannel0.SampleLevel(LinearWarp, st0 * 0.123, -100.0).x * 0.25;
        float cloud1 = clamp(cldhighvol -.15, 0.0, 1.0);
        float cloudthing = clamp( pow( abs(f0), 15.) * 1.0 * (cloud1*cloud1*5.), 0.0, (uv00.y +.1) *.6);
        test3 = clamp(lerp(cloudthing, cldhighvol,f0),0.,0.5);
    }
        }




    // Add Rain
    if(pos.y < rain6){
    if(rainvol >0.){
        
            //BAKED GODRAY TYPE THING
        float2 uv0 = (gl_FragCoord.xy / iResolution.xy);
            float2 uv = uv0 * (2.0) - (1.0);
            uv.x *=AR;
            uv.y +=Dfact;
            //uv  *= moonsz *M;
            float rainvol0 = rainvol;
            float rainthing;
            float rainangle = winddirx;
            float2 st =  (uv * float2(.5 +(uv0.y +rain1) *rain2, .1) + float2(rain3 + uv0.y *rainangle, t*rain4)) * rain5;
            float f = iChannel0.SampleLevel(LinearWarp, st, -100.0).y * iChannel0.SampleLevel(LinearWarp, st * 0.123, -100.0).x * 1.55;
            float rain = clamp(rainvol0 -.15, 0.0, 1.0);
            rainthing = clamp( pow( abs(f), 15.) * 1.0 * (rain*rain*5.), 0.0, (uv0.y +0.1) *0.6);
            test = clamp(lerp(rainthing, rainvol0,f),0.,0.5);
            float fade = smoothstep(0.5,0.6,abs(1.-uv0.y)); //between pos 0.5 and 0.6 fade 
			//float fade = smoothstep(0.5,2.9,abs(1.-uv0.y)); //between pos 0.5 and 0.6 fade 
            //test = test * fade /8.;
			test = test * fade;
			
            if(rain7 ==1.){
				float2 uv2 = uv0 * (2.0) - (1.0);
				uv2.x *=AR;
				uv2.y +=Dfact;
				float rainthing2;
			    float2 st2 =  (uv2 * float2(.5 +(uv0.y +2.) *.5, .1) + float2(0. + uv0.y *rainangle, t*0.05));
                float f2 = iChannel0.SampleLevel(LinearWarp, st2, -100.0).y * iChannel0.SampleLevel(LinearWarp, st2 * 0.123, -100.0).x * 1.55;
				float rain2a = clamp(rainvol -.15, 0.0, 1.0);
				rainthing2 = clamp( pow( abs(f2), 15.0) * 1.0 * (rain2a*rain2*5.), 0.0, (uv0.y +.1) *.6);
				test2 = clamp(lerp(rainthing2, 0.5*rainvol,f2),0.,0.1);
				float fade2 = smoothstep(rain11,rain12,abs(1.-uv0.y));
				test2 = test2 * fade2;
            }
    }
}
    //End Rain
		/*
		if(dist > rain8){
			cld = smoothstep(0.,rain8,dist);
			//haze = 0.;
			test = 0.;
			test2 = 0.;
		}
		*/
    }
	
    if(tonemap == 4.0){
    rayleigh =  exp(-h/Hr) + (cld*0.5);
        }
    else{
        rayleigh =  exp(-h/Hr);// + (cld*rain10);
    }

    
    if(tonemap == 10.){
        if(h<height_off && h>=0.){
        mie = exp((-h-rain10)/Hm1) + cld * + godray + test + test2 + test3 + haze ;
            }
        else{
            mie = exp((-h)/Hm1) + cld  + godray + test + test2 + test3 + haze;
        }
        //mie +=  (cld * factor);
    }
    else if(tonemap==5.0){
        mie = exp(-h/Hm1) + cld + godray + test + test2 + test3 + haze;
        //mie +=  (cld * factor);
    }
    else{
        mie = exp((-h)/Hm1) + godray + cld + test + test2 + test3 + haze;
    }

    #ifdef cloud2
    mie += cld2;
    #endif

}

void densities(in float3 pos, in float Hm1, out float rayleigh, out float mie) {
	
	float3 d = pos;
    d *=fov;
    //float3 dN = normalize(float3(pos.x,pos.y,fov));
    //d.y = 0.0;
   // float dist = length(d);
	
	float h = length(pos - C) - R0;
	rayleigh =  exp(-h/Hr);
	//float3 d = pos;
    //d.y = 0.0;
    float dist = length(d);
    
	float cld = 0.;
	if (mincloudheight < h && h < maxcloudheight) {
		//cld = cloud(pos+float3(t*1e3,0., t*1e3),t)*cloudy;
        cld = cloud(pos+float3(xaxiscloud,yaxiscloud, zaxiscloud),t)*cloudy;
		cld *= sin(3.1415*(h-mincloudheight)/mincloudheight) * cloudy;
	}
	
	#ifdef cloud2
        float cld2 = 0.;
        if (12e3 < h && h < 15.5e3) {
            cld2 = fnoise2(pos*3e-4,t)*cloud(pos*32.0+float3(27612.3, 0.,-t*15e3), t);
            cld2 *= sin(3.1413*(h-12e3)/12e3) * cloudyhigh;
            cld2 = clamp(cld2,0.0,1.0);
        }
    
    #endif

    #if ORIG_CLOUD
    if (dist<cloudfar) {
        float factor = clamp(1.0-((cloudfar - dist)/(cloudfar-cloudnear)),0.0,1.0);
        cld *= factor;
    }
    #else
/*
    if (dist>cloudfar) {

        float factor = clamp(1.0-((dist - cloudfar)/(cloudfar-cloudnear)),0.0,1.0);
        cld *= factor;
    }
	*/
	 //if (dist>cloudfar) {
        float factor = clamp(1.0-((dist - cloudfar)/(cloudfar-cloudnear)),0.,10.);
        //float factor = clamp(1.0-((dist - cloudfar)/(cloudfar-cloudnear)),0.,1.);
        //float factor = clamp(1.0-((cloudfar - dist)/(cloudfar-cloudnear)),0.,1.);
        cld *= factor;
   // }
	
    #endif

	mie = exp(-h/Hm1) + cld + haze;
	#ifdef cloud2
		mie += cld2;
	#endif
    
}



float escape(in float3 p, in float3 d, in float R) {
	float3 v = p - C;
	float b = dot(v, d);
	float c = dot(v, v) - R*R;
	float det2 = b * b - c;
	if (det2 < 0.) return -1.;
	float det = sqrt(det2);
	float t1 = -b - det, t2 = -b + det;
	return (t1 >= 0.) ? t1 : t2;
}

// this can be explained: http://www.scratchapixel.com/lessons/3d-advanced-lessons/simulating-the-colors-of-the-sky/atmospheric-scattering/

void subscatter(in float step_size_l, in float3 p, in float3 lightsource,in float Hm1,in float highclouds, in float height_off, inout float depthRs, inout float depthMs){
	for (int j = 0; j < 4; ++j)
		{
		float ls = float(j) * step_size_l;
		float3 ps = ( p + lightsource * ls );
		float dRs, dMs;
			#if ENABLE_DENSE_RAIN2
			densities(p,highclouds,dRs,dMs,Hm1,height_off) ;
			#else
			densities(ps,Hm1, dRs, dMs);
			#endif
		depthRs += dRs * step_size_l;
		depthMs += dMs * step_size_l;
		}
}

void lightcalc(in float Ls, inout float3 R, inout float3 M, in float3 BR1, in float3 bM1, in float3 p, in float3 lightsource,in float Hm1,in float highclouds, in float height_off, inout float dR, inout float dM, inout float depthR, inout float depthM){

    if (Ls > 0.) {

        float depthRs = 0., depthMs = 0.;
        float step_size_l = Ls / float(4);
        //subscatterunloop(step_size_l, p, light_dir, Hm1, highclouds, height_off, depthRs,depthMs);
        subscatter(step_size_l, p, lightsource, Hm1, highclouds, height_off, depthRs, depthMs);
        float3 A = exp(-(BR1 * (depthRs + depthR) + bM1  * (depthMs + depthM)));
        R += (A * dR);
        M += A * dM;
    } 
    else {
    }
}

void scatter(float3 camera_position, float3 camera_vector, float3 light_dir, float2 uvMouse, float3 moon_light_dir, float3 aur_pos, float3 aurr, float3 BR1, float3 bM1, float MI, float RI, float Hm1, float g1,out float3 raleigh, out float scat,out float3 themie,out float3 thesun,out float3 themoon, out float depthM,  out float3 auro) {


	float AR = iResolution.x/iResolution.y;
	float Dsfact = (0.5 - (Dfact/2.0));
	float ymax = Dsfact*0.8;
	float hz0 = Dsfact+0.08; //8% above horizon

	float3 light_dir2 = normalize(float3(uvMouse.x-(0.5*AR), 0.08, (fov/-2.0))); //Set a minimum light
	
	float3 bR2 =float3( 13.5e-6, 13.5e-6, 13.5e-6 );
	float3 aR2 = float3( 0.000005, 0.000005, 0.000005 );

    float MIM = 8.;
	
    float g = g1/fov; //light concentration for sky.76 //.45 //.6  .45 is normaL
    float g2 = g * g;
	
	float mg = 0.999;
	float mg2 = mg;
	
	float au = rain8;
	float au2 = au * au;
	
	
	float3 aur_v = aur_pos *4.;

		
	float mu = dot(camera_vector, light_dir);
	float mu_moon = dot(camera_vector,moon_light_dir);
	float mu_aur = dot(camera_vector,aur_v);
	
    float opmu2 = 1. + mu*mu;
    float opmu2_moon = 1. + mu_moon*mu_moon;
	float opmu2_aur = 1. + mu_aur*mu_aur;

	float phaseR = .0596831 * opmu2;
	float phaseM = .1193662 * (1. - g2) * opmu2 / ((2. + g2) * pow(1. + g2 - 2.*g*mu, 1.5));
    float phasenM = .1193662 * (1. - ng2) * opmu2 / ((2. + ng2) * pow(1. + ng2 - 2.*ng*mu, 1.5));
    float phaseS = .1193662 * (1. - s2) * opmu2 / ((2. + s2) * pow(1. + s2 - 2.*s*mu, 1.5));

	float phaseMoon = .1193662 * (1. - mg) * opmu2_moon / ((2. + mg) * pow(1. + mg - 2.*mg*mu_moon, 1.5));
	
	float phaseAur= 0.00596831 * opmu2_aur;
	
	float highclouds = 0.;
	float height_off = 0.;
	float depthR = 0.; 
    depthM=0.0;
    float3 R = ZERO_3, M = ZERO_3, R2 = ZERO_3, M2 = ZERO_3, MR = ZERO_3, MM = ZERO_3, AMR = ZERO_3, AMM = ZERO_3, FMR = ZERO_3, FMM = ZERO_3;
	
	
	float L = escape(camera_position, camera_vector, Ra);

	float step_size_i = 0.;
	step_size_i = L / float(8);
	
	for(int i=0; i < 8; ++i)
	{
		float l = float(i) * step_size_i;
		float3 p = (camera_position + camera_vector * l);
		float dR, dM;
		
		#if ENABLE_DENSE_RAIN
		densities(p,highclouds,dR,dM,Hm1,height_off) ;
		#else
		densities(p,Hm1, dR, dM);
		#endif
		
		dR *= step_size_i; dM *= step_size_i;
		depthR += dR;
		depthM += dM;

		float Ls = escape(p, light_dir, Ra);
		float Ls2 = escape(p, light_dir2, Ra);
		float mLs = escape(p, moon_light_dir,Ra);
		float AmLs = escape(p, aur_v,Ra);
        //float fLs = escape(p, fire,Ra);
		
        //sun light
        
		lightcalc(Ls, R , M, BR1, bM1, p, light_dir, Hm1, highclouds, height_off, dR, dM, depthR, depthM);
		
        //below horizon light
		if (uvMouse.y < hz0) {

			float depthRs = 0., depthMs = 0.;

            float step_size_l = Ls2 / float(4);
            //subscatterunloop(step_size_l, p, light_dir2, Hm1, highclouds, height_off, depthRs,depthMs);
            subscatter(step_size_l, p, light_dir2, Hm1,highclouds, height_off, depthRs, depthMs);

			float3 A = exp(-(BR1 * (depthRs + depthR) + bM1  * (depthMs + depthM)));
			R2 += (A * dR);
			M2 += A * dM; ;
		} 
		else {
		}
        
		
        //moon light
        lightcalc(mLs, MR , MM, bR2, bM1, p, moon_light_dir, Hm1, highclouds, height_off, dR, dM, depthR, depthM);
        //aurora light
        lightcalc(AmLs, AMR , AMM, aR2, bM1, p, aur_v, Hm1, highclouds, height_off, dR, dM, depthR, depthM);
        //fireworks?
        //lightcalc(fLs, FMR , FMM, aR2, bM1, p, fire, Hm1, highclouds, height_off, dR, dM, depthR, depthM);

        
        
	}

	if (uvMouse.y < hz0) {
        themie = (MI) *(M2 * bM1  * (phaseM ));
		depthM = depthM;
    }
	else{
		themie = (MI) *(M * bM1  * (phaseM )); // Mie scattering
		depthM = depthM;
	}

	raleigh = (RI) *(max(R,Rm) * BR1 * phaseR); //Rayleigh scattering

    #if NICE_HACK_SUN
    thesun = (SI) *(M * bM1  *phaseS); //Sun
    #endif
    
    scat = 1.0 - clamp(depthM*3e-5,0.,0.99); 
    
	themoon = (MoonINT * moonillumination) *(MM * bM1 * phaseMoon); //Moon
	auro = (1.) * (AMM * bM1) * phaseAur;
    //firescat = (1.) * (FMM * bM1) * phaseAur;
}

float3 get_camera_vector(float3 resolution, float2 coord, float Dfacta) {
    float2 uv    = (coord.xy / resolution.xy) * 2. - 1.;
    uv.x *= resolution.x / resolution.y;
    uv.y +=Dfacta;

    return normalize(float3(uv.x, uv.y, -fov));
}



const float MAX = 10000.0;
//-----------------------------------------------------
// Lightning functions
//-----------------------------------------------------

float diffuse(float3 n,float3 l,float p) {
    return pow(dot(n,l) * 0.4 + 0.6,p);
}

float specular( const in float3 n, const in float3 l, const in float3 e, const in float s) {    
    float nrm = (s + 8.0) / (3.1415 * 8.0);
    return pow(max(dot(reflect(e,n),l),0.0),s) * nrm;
}

float fresnel( const in float3 n, const in float3 e, float s ) {
    return pow(clamp(1.-dot(n,e), 0., 1.),s);
}
//-----------------------------------------------------
// Math functions
//-----------------------------------------------------

float2 rotate(float angle, float2 v) {
    return float2(cos(angle) * v.x + sin(angle) * v.y, cos(angle) * v.y - sin(angle) * v.x);
}

float boolSub(float a,float b) { 
    return max(a,-b); 
}
float sphere(float3 p,float r) {
    return length(p)-r;
}

float3x3 fromEuler(float3 ang) {
    float2 a1 = float2(sin(ang.x),cos(ang.x));
    float2 a2 = float2(sin(ang.y),cos(ang.y));
    float2 a3 = float2(sin(ang.z),cos(ang.z));
    float3x3 m;
    m[0] = float3(a1.y*a3.y+a1.x*a2.x*a3.x,a1.y*a2.x*a3.x+a3.y*a1.x,-a2.y*a3.x);
    m[1] = float3(-a2.y*a1.x,a1.y*a2.y,a2.x);
    m[2] = float3(a3.y*a1.x*a2.x+a1.y*a3.x,a1.x*a3.x-a1.y*a3.y*a2.x,a2.y*a3.y);
    return m;
}

//-----------------------------------------------------
// Intersection functions (by iq)
//-----------------------------------------------------

float3 nSphere( in float3 pos, in float4 sph ) {
    return (pos-sph.xyz)/sph.w;
}

float iSphere( in float3 ro, in float3 rd, in float4 sph ) {
    float3 oc = ro - sph.xyz;
    float b = dot( oc, rd );
    float c = dot( oc, oc ) - sph.w*sph.w;
    float h = b*b - c;
    if( h<0.0 ) return -1.0;
    return -b - sqrt( h );
}

float iCSphereF( float3 p, float3 dir, float r ) {
    float b = dot( p, dir );
    float c = dot( p, p ) - r * r;
    float d = b * b - c;
    if ( d < 0.0 ) return -MAX;
    return -b + sqrt( d );
}

float2 iCSphere2( float3 p, float3 dir, float r ) {
    float b = dot( p, dir );
    float c = dot( p, p ) - r * r;
    float d = b * b - c;
    if ( d < 0.0 ) return float2( MAX, -MAX );
    d = sqrt( d );
    return float2( -b - d, -b + d );
}

float3 nPlane( in float3 ro, in float4 obj ) {
    return obj.xyz;
}

float iPlane( in float3 ro, in float3 rd, in float4 pla ) {
    return (-pla.w - dot(pla.xyz,ro)) / dot( pla.xyz, rd );
}

//-----------------------------------------------------
// Seascape by TDM
// 
// https://www.shadertoy.com/view/Ms2SD1
//-----------------------------------------------------
//const int   SEA_NUM_STEPS = 7;
const int   SEA_ITER_GEOMETRY = 3;
const int   SEA_ITER_FRAGMENT = 5;

const float SEA_EPSILON	= 1e-3;
#define       SEA_EPSILON_NRM	(0.1 / iResolution.x)
//const float3  SEA_BASE = float3(0.1,0.19,0.22);
const float3  SEA_WATER_COLOR = float3(0.8,0.9,0.6);
//const float3  SEA_WATER_COLOR = float3(0.8,0.9,0.1);
#define SEA_TIME (1.0 + iTime * SEA_SPEED)
const float2x2  sea_octave_m = float2x2(1.6,1.2,-1.2,1.6);

float hash( float2 p ) {
    float h = dot(p,float2(127.1,311.7));	
    return frac(sin(h)*43758.5453123);
}

float sea_noise( in float2 p ) {
    float2 i = floor( p );
    float2 f = frac( p );	

    float2 u = f*f*(3.0-2.0*f);

    return -1.0+2.0*lerp( 
        lerp( hash( i + float2(0.0,0.0) ), 
            hash( i + float2(1.0,0.0) ), 
            u.x),
        lerp( hash( i + float2(0.0,1.0) ), 
            hash( i + float2(1.0,1.0) ), 
            u.x), 
        u.y);
}

float seaOctave( in float2 uv, const in float choppy) {
    if(tonemap!=8.){
        uv += sea_noise(uv); //5ffps difference on laptop sea_noise vs noise
    }
    else{
    uv += noise(uv); //5ffps difference on laptop sea_noise vs noise
        }
    float2 wv = 1.0-abs(sin(uv));
    float2 swv = abs(cos(uv));
    wv = lerp(wv,swv,wv);
    return pow(1.0-pow(wv.x * wv.y,0.65),choppy);
}

float seaMap(float3 p) {
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    float2 uv = p.xz; uv.x *= 0.75;

    float d, h = 0.0;    
    for(int i = 0; i < SEA_ITER_GEOMETRY; i++) {
        d = seaOctave((uv+SEA_TIME)*freq,choppy);
        d += seaOctave((uv-SEA_TIME)*freq,choppy);
        h += d * amp;
        uv *= sea_octave_m._m00_m01;
        freq *= 1.9;
        amp *= 0.22;
        choppy = lerp(choppy,1.0,0.2);
    }
    return p.y - h;
}

float seaMapHigh(float3 p) {
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    float2 uv = p.xz; uv.x *= 0.75;

    float d, h = 0.0;    
    for(int i = 0; i < SEA_ITER_FRAGMENT; i++) {        
        d = seaOctave((uv + SEA_TIME) * freq, choppy);
        d += seaOctave((uv - SEA_TIME) * freq, choppy);
        h += d * amp;        
        uv *= sea_octave_m._m00_m01;
        freq *= 1.9;
        amp *= 0.22;
        choppy = lerp(choppy,1.0,0.2);
    }
    return p.y - h;
}

float3 getSkyColor(float3 e, float3 skycolor) {
    e.y = (max(e.y,0.0)*0.8+0.2)*0.8;
    if (tonemap==3.){
        return  skycolor;
        }
    else{
    return float3(pow(1.0-e.y,2.0), 1.0-e.y, 0.6+(1.0-e.y)*0.4) * 1.1;
        }

}

float3 hsv(float shue, float ssaturation, float value)
{
    float4 t = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(float3(shue, shue, shue) + t.xyz) * 6.0 - float3(t.w, t.w, t.w));
    return value * lerp(float3(t.x, t.x, t.x), clamp(p - float3(t.x, t.x, t.x), 0.0, 1.0), ssaturation);
}
float3 getSeaColor(float3 p, float3 n, float3 l, float3 eye, float3 dist, float3 skycolor,in float spec1, in float3 moon_light,in float mLight, in float scat) {
    
    float RAINBOW_WATER_SATURATION	= 0.35;
    float RAINBOW_WATER_LIGHTNESS		= 0.1; //0.2
    float RAINBOW_WATER_SPEED 		= 0.1;
    float fresnel = clamp(1.0 - dot(n,-eye), 0.0, 1.0);
    fresnel = pow(fresnel,3.0) * 0.5;



    float3 reflected = getSkyColor(reflect(eye,n),skycolor);//*skycolor;

    if(tonemap == 7.0)
    {
        float3 weights_ = float3(0.2125, 0.7154, 0.0721); // sums to 1
        float luminance_ = dot(reflected, weights_);
        reflected = lerp(float3(luminance_, luminance_, luminance_), reflected, 0.1);
    }
    //float3 refraced = SEA_BASE + diffuse(n,l,80.0) * SEA_WATER_COLOR * 0.12; 
    float3 refraced = SEA_BASE + diffuse(n,l,diffusepow) * SEA_WATER_COLOR * 0.12; 


    float3 color = lerp(refraced,skycolor,fresnel) * skycolor ;

    float atten = max(1.0 - dot(dist,dist) * 0.001, 0.0);
    //color += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;
    color +=  clamp(SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.08 * atten, 0.0, 1.0) ;

    //float3 l2 = normalize(float3(l.x,0.5,l.z));
    if(spectype==0.){
        float s = specular(n, l, eye, 60.);
        color += clamp(float3(s, s, s) * skycolor * specbrightness, 0.0, 1.0);
    }
    if(spectype==1.){
        float s = specular(n, l, eye, spec1);
        color += clamp(float3(s, s, s) * skycolor * specbrightness, 0.0, 1.0);
    }
    if(spectype==2.){
        float s = specular(n, l, eye, spec1 * scat);
        color += clamp(float3(s, s, s) * skycolor * specbrightness, 0.0, 1.0);
    }
    if(spectype==3.){
        float s = specular(n, l, eye, clamp(1000.0 * scat, 10., 60.));
        color += clamp(float3(s, s, s) * skycolor * specbrightness, 0.0, 1.0);
    }
    if(spectype==4.){
        float s = specular(n, l, eye, clamp(1000.0 * scat, 10., 60.));
        color += clamp(float3(s, s, s) * skycolor * scat, 0.0, 1.0);
    }
    if(spectype==5.){
        float s = specular(n, l, eye, clamp(1000.0 * scat, 10., 60.));
        color += clamp(float3(s, s, s) * skycolor * scat * specbrightness, 0.0, 1.0);
    }
    if(spectype==6.){
        float s = specular(n, l, eye, spec1);
        color += clamp(float3(s, s, s) * skycolor * scat * specbrightness, 0.0, 1.0);
    }
    if(spectype==7.){
        float s = specular(n, l, eye, 60.);
        color += clamp(float3(s, s, s) * skycolor * scat * specbrightness, 0.0, 1.0);
    }
    //color += clamp(float3(specular(n,l*(1.-scat),eye,spec1))* skycolor * specbrightness, 0.0, 1.0);
    float s = specular(n, moon_light, eye, moonspec);
    color += float3(s, s, s)* skycolor * moonillumination*mLight;

    //color +=lerp(float3(specular(n,l,eye,spec1)),float3(specular(n,moon_light,eye,60.))* moonillumination,0.5)* skycolor;
    color = pow(color, float3(0.65, 0.65, 0.65));
    //color += hsv((p.z * 0.3) - iTime * RAINBOW_WATER_SPEED, RAINBOW_WATER_SATURATION, RAINBOW_WATER_LIGHTNESS);

    return color;
}

float3 seaGetNormal(const in float3 p, const in float eps) {
    float3 n;
    n.y = seaMapHigh(p);    
    n.x = seaMapHigh(float3(p.x+eps, p.y, p.z)) - n.y;
    n.z = seaMapHigh(float3(p.x, p.y, p.z+eps)) - n.y;
    n.y = eps;
    return normalize(n);
}

float seaheightMapTracing(float3 ori, float3 dir, out float3 p) {  
    float tm = 0.0;
    float tx = 1000.0;// really far distance, this could likely be tweaked a bit as desired
    float hx = seaMap(ori + dir * tx);
    if(hx > 0.0) return tx;   
    float hm = seaMap(ori + dir * tm);    
    float tmid = 0.0;
	
    for(int i = 0; i < int(SEA_NUM_STEPS); i++) {

        tmid = lerp(tm,tx, hm/(hm-hx));                   
        p = ori + dir * tmid;                   
        float hmid = seaMap(p);
        if(hmid < 0.0) {
            tx = tmid;
            hx = hmid;
        } else {
            tm = tmid;
            hm = hmid;
        }
    }
    return tmid;
}


float3 seaTransform( in float3 x ) {
    x.yz = rotate( 0.8, x.yz );
    return x;
}

float3 seaUntransform( in float3 x ) {
    x.yz = rotate( -0.8, x.yz );
    return x;
}

float noisef2(float t)
{
    return iChannel0.Sample(LinearWarp, float2(t, .0) / iChannelResolution[0].xy).x;
}
float noisev2(float2 t)
{
    return iChannel0.Sample(LinearWarp, t / iChannelResolution[0].xy).x;
}

float3 lensFlare(float2 uv,float2 pos,float whichtype)
{
    //uv.y +=-Dfact;
    float2 main = uv-pos;
    float2 uvd = uv*(length(uv));

    float ang = atan2(main.x,main.y);
    float dist=length(main); dist = pow(dist,.1);
    float n = noisev2(float2(ang*16.0,dist*32.0));

    float f0 = 1.0/(length(uv-pos)*16.0+6.0);

    //f0 = f0*(sin(noisef2(sin(ang*2.+pos.x+sin(iTime*0.5))*4.0 - cos(ang*4.+pos.y))*16.)*.1 + dist*.1 + .8)*2.2;
    //f0 = f0*(sin(noisef2(sin(ang*2.+pos.x-sin(iTime*0.15))*4.0 - cos(ang*4.+pos.y+sin(iTime*-0.15)))*16.)*.1 + dist*.1 + .8)*2.2;
    f0 = f0*(sin(noisef2(sin(ang*2.+pos.x-(sin(iTime*0.05))*2.)*4.0 - cos(ang*4.+pos.y+cos(iTime*0.15)))*16.)*.1 + dist*.1 + .8)*2.2;

    float f1 = max(0.01-pow(length(uv+1.2*pos),1.9),.0)*7.0;
    float f2,f22,f23;
    float f4,f42,f43;
    float f5,f52,f53;
    float f6,f62,f63;
    

    if(whichtype == 2.){
        f2  = max(1.0 / (1.0 + 32.0 * pow(length(uv - 0.05 - pos), 1.5)), 0.0) * 0.25;
        f22 = max(1.0 / (1.0 + 32.0 * pow(length(uv + 0.0  - pos), 1.5)), 0.0) * 0.23;
        f23 = max(1.0 / (1.0 + 32.0 * pow(length(uv + 0.05 - pos), 1.5)), 0.0) * 0.25;
        //f2 = max(1.0/(1.0+32.*pow(length(uv+0.0-pos),1.5)),.0)*00.25;
        //f22 = max(1.0/(1.0+32.*pow(length(uv+0.0-pos),1.5)),.0)*00.23;
        //f23 = max(1.0/(1.0+32.*pow(length(uv+0.0-pos),1.5)),.0)*00.21;
    }
    else{
        f2  = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.8  * pos), 2.0)), 0.0) * 0.25;
        f22 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.85 * pos), 2.0)), 0.0) * 0.23;
        f23 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.9  * pos), 2.0)), 0.0) * 0.21;
    }
    float2 uvx = lerp(uv,uvd,-0.5);

    if(whichtype == 2.){
        f4 = max(0.01-pow(length(uv+0.0-pos),1.4),.0)*6.0;
        f42 = max(0.01-pow(length(uv+0.0-pos),1.4),.0)*5.0;
        f43 = max(0.01-pow(length(uv+0.0-pos),1.4),.0)*3.0;
    }
    else{
        f4 = max(0.01-pow(length(uvx+0.4*pos),2.4),.0)*6.0;
        f42 = max(0.01-pow(length(uvx+0.45*pos),2.4),.0)*5.0;
        f43 = max(0.01-pow(length(uvx+0.5*pos),2.4),.0)*3.0;
    }

    uvx = lerp(uv,uvd,-.4);

    if(whichtype == 2.){
        f5 = max(0.01-pow(length(uv+0.0-pos),2.5),.0)*2.0;
        f52 = max(0.01-pow(length(uv+0.0-pos),2.5),.0)*2.0;
        f53 = max(0.01-pow(length(uv+0.0-pos),2.5),.0)*2.0;
    }
    else{
        f5 = max(0.01-pow(length(uvx+0.2*pos),5.5),.0)*2.0;
        f52 = max(0.01-pow(length(uvx+0.4*pos),5.5),.0)*2.0;
        f53 = max(0.01-pow(length(uvx+0.6*pos),5.5),.0)*2.0;
    }

    uvx = lerp(uv,uvd,-0.5);

    if(whichtype == 2.){
        f6 = max(0.01-pow(length(uv+0.0-pos),1.6),.0)*6.0;
        f62 = max(0.01-pow(length(uv+0.0-pos),1.6),.0)*3.0;
        f63 = max(0.01-pow(length(uv+0.0-pos),1.6),.0)*5.0;
    }
    else{
        f6 = max(0.01-pow(length(uvx-0.3*pos),1.6),.0)*6.0;
        f62 = max(0.01-pow(length(uvx-0.325*pos),1.6),.0)*3.0;
        f63 = max(0.01-pow(length(uvx-0.35*pos),1.6),.0)*5.0;
    }



    float3 c = ZERO_3;

    if(whichtype == 2.){
        c.r+=f2+f4+f5+f6; 
        c.g+=f22+f42+f52+f62; 
        c.b+=f23+f43+f53+f63;
    }
    else{
        c.r+=f2+f4+f5+f6; 
        c.g+=f22+f42+f52+f62; 
        c.b+=f23+f43+f53+f63;
    }

    c = c * 1.3 - float3(length(uvd) * 0.05, length(uvd) * 0.05, length(uvd) * 0.05);
    c+=float3(f0, f0, f0);

    return c;
}


//SNOW STUFF
float SIZE_RATE = 0.1;
float XSPEED = 0.5;
float YSPEED = 0.75;
float LAYERS = 20.;

float Hash11(float p)
{
    float3 p3 = frac(float3(p, p, p) * 0.1);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z); 
}

float2 Hash22(float2 p)
{
    float3 p3 = frac(float3(p.xyx) * 0.3);
    p3 += dot(p3, p3.yzx+19.19);
    return frac((p3.xx+p3.yz)*p3.zy);
}

float2 Rand22(float2 co)
{
    float x = frac(sin(dot(co.xy ,float2(122.9898,783.233))) * 43758.5453);
    float y = frac(sin(dot(co.xy ,float2(457.6537,537.2793))) * 37573.5913);
    return float2(x,y);
}

float3 SnowSingleLayer(float2 uv,float layer){
    float3 acc = float3(0.0,0.0,0.0);
    uv = uv * (2.0 + layer);
    float xOffset = uv.y * (((Hash11(layer)*2.-1.)*0.5+1.)*XSPEED);
    float yOffset = YSPEED * iTime;
    uv += float2(xOffset,yOffset);
    float2 rgrid = Hash22(floor(uv)+(31.1759*layer));
    uv = frac(uv) - (rgrid*2.-1.0) * 0.35 - 0.5;
    float r = length(uv);
    float circleSize = 0.04*(1.5+0.3*sin(iTime*SIZE_RATE));
    float val = smoothstep(circleSize,-circleSize,r);
    float3 col = float3(val,val,val)* rgrid.x  ;
    return col;
}
// END SNOW

float calcvalue(in float pos, in float min1, in float max1, in float bmin, in float bmax){
	float val = 0.;
	val = clamp(((max1-min1) * ((pos-bmin)/(bmax-bmin))) + min1,min1,max1);
	return val;
}

float3 getPixel(in float2 coord, float time, in float3 skycolor, in float3 light_dir, in float spec1, in float3 moon_light_dir2, in float mLight, in float scat) {
    float2 uv = coord / iResolution.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;
    uv.y += Dfact;
    // ray
    float3 ang = float3(0.0,0.0,0.0);
    float3 ori = float3(xaxiscloud*0.001,3.5,zaxiscloud*0.001);
    //float3 ori = float3(0.0,3.5,time);
    
    // This is the ray direction we are shooting from the camera location ("ori") that we need to light
    // for this pixel.  The -2.0 indicates we are using a focal length of 2.0 - this is just an artistic choice and
    // results in about a 90 degree field of view.
    float3 dir = normalize(float3(uv.x,uv.y,-fov));
    float3 dir2 = dir;
    
    // Distort the ray a bit for a fish eye effect (if you remove this line, it will remove
    // the fish eye effect and look like a realistic perspective).
    // dir.z += length(uv) * 0.15;
    //dir.z += length(uv) * 0.14;
    float3x3 rotZ = rotateZ(camera_T_rotate); //rotate on the Z axis //tilt
    float3x3 rotY = rotateY(camera_H_rotate);  //rotate on the Y axis //left and right
    float3x3 rotX = rotateX(camera_V_rotate);  //rotate on the Y axis //up and down
    // Renormalize the ray direction, and then rotate it based on the previously calculated
    // animation angle "ang".  "fromEuler" just calculates a rotation matrix from a vector of angles.
    // if you remove the " * fromEuler(ang)" part, you will disable the camera rotation animation.
    dir = mul(mul(mul(normalize(dir), rotX), rotY), rotZ);

    // tracing
    // ray-march to the ocean surface (which can be thought of as a randomly generated height map)
    // and store in p
    float3 p;
    seaheightMapTracing(ori,dir,p);
    float3 dist = p - ori; // distance vector to ocean surface for this pixel's ray
    
    // Calculate the normal on the ocean surface where we intersected (p), using
    // different "resolution" (in a sense) based on how far away the ray traveled.  Normals close to
    // the camera should be calculated with high resolution, and normals far from the camera should be calculated with low resolution
    // The reason to do this is that specular effects (or non linear normal based lighting effects) become fairly random at
    // far distances and low resolutions and can cause unpleasant shimmering during motion.
    float3 n = seaGetNormal(p, dot(dist,dist) * SEA_EPSILON_NRM);
    
    // direction of the infinitely far away directional light.  Changing this will change
    // the sunlight direction.
    float3 moon_light = normalize(moon_light_dir2);
    float3 light = normalize(float3(light_dir.x,light_dir.y,light_dir.z));

    /*
    if(mousepos.y < 0.5 - (Dfact/2.0)){
        light.x=sun_pos_x;light.y=sun_pos_y;light.z=sun_pos_z;
    }
    */
    // color
    if (tonemap==3.){
        return lerp(
            skycolor,
            getSeaColor(p,n,light,dir,dist,skycolor,spec1,moon_light,mLight,scat),
            pow(smoothstep(0.0,-0.02,dir.y),0.9));
    }
    else{
        return getSeaColor(p,n,light,dir,dist,skycolor,spec1,moon_light,mLight,scat);

        }
}

#define mul3x(x) 				x * x * x
#define RAINBOW_START_Y					0.0


float2 _smoothstep(in float2 f)
{
    return f * f * (3.0 - 2.0 * f);
}

float3 _smoothstep(in float3 p)
{
    return p * p * 3.0 - 2.0 * mul3x(p);
}



float3 rainbowColor(in float3 ray_dir, in float3 light_dir)
{
    
    float RAINBOW_INTENSITY   		= 0.3;
    float3 RAINBOW_COLOR_RANGE 		= float3(56.0, 53.0, 50.0);  // The angle in degrees for red, green and blue

    float3 	    RAINBOW_DIR 				= float3(-0.2, -0.1, 0.0);
    RAINBOW_DIR = normalize(light_dir);

    float theta = degrees(acos(dot(RAINBOW_DIR, ray_dir)))*1.;
    float3 nd 	= saturate(1.0 - abs((RAINBOW_COLOR_RANGE - theta) * 0.2));
    float3 color  = _smoothstep(nd) * RAINBOW_INTENSITY;

    return color * max((RAINBOW_BRIGHTNESS - 0.75) * 1.5, 0.0);
}

float3 rainbow(float2 fragCoord, float3 light_dir)
{
    //float2 uv = fragCoord.xy / iResolution.xy;
    //float2 p = (-1.0 + 2.0 * uv) * float2(iResolution.x / iResolution.y, 1.0);
    float2 p = fragCoord;
    //p.y +=Dfact ;
    float Dsfact = 0.5 - (Dfact/2.0);
    float3 RAINBOW_POS	= float3(0., -0.5, 0.5);
    float3 color = ZERO_3;


    if (p.y >= RAINBOW_START_Y)
    {
        float3 rainbowW   	  = -normalize(-RAINBOW_POS);
        float3 rainbowUp  	  = normalize(cross(rainbowW, float3(0.0, 1.0, 0.0)));
        float3 rainbowVertical = normalize(cross(rainbowUp, rainbowW));

        float3 dir = normalize(float3(p, fov) - float3(0.0, 0.25, rb1));
        float3 wdDir = normalize(dir.x * rainbowUp + dir.y * rainbowVertical - dir.z * rainbowW);

        color += rainbowColor(wdDir,light_dir);
    }
    return saturate(color);
}

float3 rainbow2(float2 fragCoord, float3 light_dir)
{
    float2 uv = fragCoord.xy / iResolution.xy;
    float2 p = (-1.0 + 2.0 * uv) * float2(iResolution.x / iResolution.y, 1.0);
    p.y +=Dfact ;
    float3 RAINBOW_POS	= float3(4.5, -0.5, 0.5);
    float3 color = ZERO_3;


    if (p.y >= RAINBOW_START_Y)
    {
        float3 rainbowW   	  = -normalize(-RAINBOW_POS);
        float3 rainbowUp  	  = normalize(cross(rainbowW, float3(0.0, 1.0, 0.0)));
        float3 rainbowVertical = normalize(cross(rainbowUp, rainbowW));

        float3 dir = normalize(float3(p, 0.0) - float3(0.0, 0.0, -1.5));
        float3 wdDir = normalize(dir.x * rainbowUp + dir.y * rainbowVertical - dir.z * rainbowW);

        color += rainbowColor(wdDir,float3(-0.2, -0.1, 0.0));
    }
    return saturate(color);
}

float3 rainbowhalo(float2 fragCoord, float3 light_dir)
{

    float2 p = fragCoord;
    //p.y +=Dfact ;
    float Dsfact = 0.5 - (Dfact/2.0);
    float3 RAINBOW_POS	= float3(0., 0., 0.15);
    float3 color = ZERO_3;


    if (p.y >= RAINBOW_START_Y)
    {
        float3 rainbowW   	  = -normalize(-RAINBOW_POS);
        float3 rainbowUp  	  = normalize(cross(rainbowW, float3(0.0, 1.0, 0.0)));
        float3 rainbowVertical = normalize(cross(rainbowUp, rainbowW));

        float3 dir = normalize(float3(p, -fov) - float3(0.0, 0.25, 1.5));
        float3 wdDir = normalize(dir.x * rainbowUp + dir.y * rainbowVertical - dir.z * rainbowW);

        color += rainbowColor(wdDir,light_dir);
    }
    return saturate(color);
}


float3 shiftHue(in float3 col, in float Shift)
{
    float3 P = float3(0.55735, 0.55735, 0.55735) * dot(float3(0.55735, 0.55735, 0.55735), col);
    float3 U = col - P;
    float3 V = cross(float3(0.55735, 0.55735, 0.55735), U);
    col = U * cos(Shift * 6.2832) + V * sin(Shift * 6.2832) + P;
    return col;
}


float4 circle(float2 uv, float2 pos, float rad, float3 color) {
	float d = length(pos - uv) - rad;
	float t = clamp(d, 0.0, 1.0);
	return float4(color, d);
}

float3 circle2(float2 uv, float2 pos, float rad, float3 color) {
	float d = length(pos - uv) - rad;
	float t = clamp(d, 0.0, 1.0);
	return color * d;
}

float3 DrawCircle(float2 uv, float radius, float fade)
{
    // Determine the length of the point away from the "center" of the circle.
    float uvLength = length(uv);
    // Apply color to any coordinate that lies within the circle.
    // Black if coord is outside and white if it is inside the circle.
    // The smoothstep function applies the fade to the circle.
    // The min value is the radius, and max value is the radius minus
    // the "fade" variable.
    // This means the fade is applied within the bounds of the circle.
    float s = smoothstep(radius, radius - fade, uvLength);
    float3 circle = float3(s, s, s);
    
    return float3(circle);
}

float4 main(PresentOut pin) : SV_Target
{
    float AR = iResolution.x/iResolution.y;
    float M = 1.; //res
	float Dsfact = 0.5 - (Dfact/2.0);
	
	float2 uvMouse = (iMouse.xy / iResolution.xy);
    uvMouse.x *= AR;
    
    float2 uvMouseN = ((iMouse.xy / iResolution.xy)) * 2.0 - 1.0;
    
    if (uvMouse.y == 0.) uvMouse.y=(0.5-(0.05*fov)); //initial view 
    if (uvMouse.x == 0.) uvMouse.x=(1.0-(0.05*fov)); //initial view
	
    float2x2 ma = float2x2(cos(moon_rotate), -sin(moon_rotate), sin(moon_rotate), cos(moon_rotate));
	
	float3x3 rotZ = rotateZ(camera_T_rotate); //rotate on the Z axis //tilt
    float3x3 rotY = rotateY(camera_H_rotate);  //rotate on the Y axis //left and right
    float3x3 rotX = rotateX(camera_V_rotate);  //rotate on the Y axis //up and down
    
    float moonsz = 1.0;
    //float moon_y2 = 1.75;
    float moon_x2 = uvMouse.x-(0.5*AR);
    float mlightoffsetx = 0.;
    float moonpos_y =sin((uvMouseN.x) * 1.5);
	
	float uvMoonpos_y = (moonpos_y); 
    float uvMoonpos_y3 = ((moonpos_y) / 2.) + 0.125; //I don't understand what I'm doing here
    float uvMoonpos_y2 = (moonpos_y)-Dsfact; //co-ordinates 0 to 1, 0.5 being center


    float moon_size = moonsz *M;
    
    float2 q = (pin.texUV.xy / (iResolution.xy)) * 2.0 - 1.0;
    //q.x *=AR;
   // q  *= moon_size;
    
    //float mfixx = (iMouse.x / iResolution.x); //*moon_size*AR;
    float mfixy = (uvMoonpos_y2);//*moon_size;

    float2 uv0 = (pin.texUV.xy / iResolution.xy);
    uv0 *= M;

    float2 uv = uv0 * (2.0*M) - (1.0*M);
    uv.x *=AR;

   

	float3 D_moon = normalize(float3(q, -(fov*M)));

    float2x2 mar = float2x2(cos(t*0.9), -sin(t*0.9), sin(t*0.9), cos(t*0.9));


    float2 uvMouse0 = (iMouse.xy / iResolution.xy);
    float uvM = radians(uvMouse0.x*360.);

    float2x2 ma2 = float2x2(cos(uvM), -sin(uvM), sin(uvM), cos(uvM));
    float mlightoffsetx1 = (mlightoffsetx -0.5)/10.0; //From moon phase determines direction of light when not a full moon for example

    float3 camera_position = float3(0,camera_height, camera_depth);
    float3 camera_vector = mul(mul(mul(get_camera_vector(iResolution, pin.texUV, Dfact), rotZ), rotY), rotX);

    //I don't understand why uvMouse.x-(0.5*AR) works (specifically (0.5*AR)) is it because the offset has to be multiplied by AR too?
    float uvMo = uvMouse.x - (0.5 * AR) ;

    
    float3 light_dir = normalize(float3(uvMo, uvMouse.y-Dsfact, (fov/-2.0))); //changes later if (uvMouse.y < hz0)
    //float3 light_dir = normalize(float3(uvMouse.x-(0.5*AR), uvMouse.y-Dsfact, (fov/-2.0))); //changes later if (uvMouse.y < hz0)
    float3 moon_light_dir = normalize(float3(moon_x2, uvMoonpos_y3 , (fov/-2.0)));
    float3 light_dir_original = normalize(float3(uvMouse.x-(0.5*AR), clamp(uvMouse.y,0.,1.), (fov/-2.0)));

	
	

	float3 color =          ZERO_3;
    float3 color2 =         ZERO_3;
	float3 raleigh =        ZERO_3;
    float3 themoon =        ZERO_3;
    float3 thesun =         ZERO_3;
    float3 themie =         ZERO_3;
    float3 star =           ZERO_3;
	float3 lensf =          ZERO_3;
	float3 fireworkCol =    ZERO_3;
	float3 fire =           ZERO_3;
	float3 auro =           ZERO_3;
    float4 aur =            ZERO_4;
    float3 a_pos =          ZERO_3;

    float scat = 0.;
    float depthM = 0.;
	float att = 1.;
    float refatt = 1.;
    float att_moon = 1.;
    float sunatt = 1.;
    float sunscat = 1.;
    float moonatt = 1.;
    float thing = 1.;

    float staratt = 1.1 -min(1.0,pow(uvMouse.y+((Dfact/2.)+0.51),16.));
    float scatatt = 1.0 -min(1.0,clamp((clamp(uvMouse.y,0.,1.)*((Dfact*4.)+2.)),0.55,1.));
	
	
    float auratt = 1. -min(1.0,pow(uvMouse.y+0.51,64.));

	 float fade = smoothstep(0.,0.01,abs(camera_vector.y))*0.1+0.9;
	
	float3 bg1 = ZERO_3;
    bg1 = bg(camera_vector) * fade;
	
	float MI2 = 1.;
	
    float2 uvl = pin.texUV.xy / iResolution.xy - 0.5;
    uvl.x *= iResolution.x/iResolution.y;
	
	
	float hz = Dsfact;
    float hzO = 0.08; //8% above horizon?
    float hz0 = hz+hzO; //0.58
	float hz1 = hz0*0.84; 
	float hz2 = Dsfact*0.84;
	float hd = hz0-hz1;     //0.0928
    float hz1a = hz*0.84;   //0.4872
    float hda = hz-hz1a;     //0.0928

	float g1 = g0;
    float gmax = g0;
    float gmin = 0.1;
	
	float newx = uvMouse.x-(0.5*AR);
	//float newx = 0.;
    float newx2 = 0.;
    float diffEI = (uvMoonpos_y3) - (uvMouse.y-Dsfact);
    float diffEIp = 1.-abs(diffEI/eclispethreshold);
	
	float Hm1 = Hm;
    float HmMax =Hm;
    float HmMin =HmMin1;

	float RR1 = rr;
    float RR1Max = rr;
    float RR1Min = RRMin;
	
	float gam = 1.0;
    float gamMax =miegam;
    float gamMin =miegamin;
	
	
	float rgam = raleighgam;
    float rgamMax =raleighgam;
    float rgamMin =raleighgammin;
	
     //2142
    float spec1 = spec;
    float specMax = spec;
    float specMin = specmin;
    float symax = Dsfact * 0.6;
    float symin = Dsfact;
    
    float spymax = Dsfact * 0.8;
    float spymin = Dsfact;
	float uvyD = (uvMouse.y-Dsfact);
	
	float ymax = Dsfact * 0.8;
    float ymin = Dsfact * 0.6;
	
	float lensb = lensflareb;
    float lensbMax = lensflareb;
	
	
	float mLight = 0.0;
    float mLightMax = 1.;
    float mLightMin = 0.0;
	
	float bM1 = bM.x;
    float bMMax = bM.x;
    float bMMin = 0.000021;
	
	float3 BM1 = bM;
	
	float MI1 = MI0;
    float MImax = MI0;
    float MImin = MIMin1;
	
	float RI1 = RI0;
    float RImax = RI1;
    float RImin = 0.;
	
	float EI = 0.;
    float EImax = MI0;
    float EImin1 = 0.;
	float RE =rain13;
	
    float EImin = clamp(((EImax-EImin1)* ((moonillumination-1.)/(0.-1.))) + EImin1,EImin1,EImax);
	
	float sLight = 0.;
    float sLightMax =2.;
    float sLightMin =hzO;
	

    bM1 = clamp(((bMMax-bMMin) * ((uvMouse.y-(Dsfact*1.5))/(1.-(Dsfact*1.5)))) +   bMMin,bMMin,bMMax);
    float3 bM11 = float3(bM1, bM1, bM1);
	
	float3 BR1;
	
	float uvMyin_ymaxymin = (uvMouse.y-ymin)/(ymax-ymin);
	float uvMyin_ymaxymin2 = (uvMouse.y-ymax)/(0.5-ymax);
	float uvMyin_ymaxymin3 = (uvMouse.y-Dsfact)/(0.5-Dsfact);
	float uvMyin_ymaxymin4 = (uvMouse.y-hz0)/(0.5-hz0);
	float uvMyin_ymaxymin5 = (uvMouse.y-Dsfact)/(0.3-Dsfact);
	float uvMyin_ymaxyminA = (uvMouse.y-ymax)/(Dsfact-ymax);

	

    if (sealight == 0.0)
    {
        uvyD = sLight;
    }
	
	newx2 = uvMouse.x-(0.5*AR);
    float3 sea_light_dir = normalize(float3(newx2, uvyD, (fov/-2.0)));
    float3 moon_light_dir2 = ZERO_3;
	
#if EXPENSIVE_CALC
	mLight = calcvalue(uvMouse.y, mLightMin, mLightMax,spymin,spymax);
	sLight = calcvalue(uvMouse.y, sLightMin, sLightMax,hz0,1.);
	spec1 = clamp(((specMin-specMax) * ((uvMouse.y-Dsfact)/(0.75-Dsfact))) + specMax,specMin,specMax);
	rgam = clamp(((rgamMax-rgamMin) * (uvMyin_ymaxymin3)) + rgamMin,rgamMin,rgamMax);
	gam = clamp(((gamMin-gamMax) * (uvMyin_ymaxymin2)) + gamMax,gamMin,gamMax);
#endif
	
	if(tonemap==9.)
    {
#if EXPENSIVE_CALC
        Hm1 = clamp(((HmMax-HmMin) * (uvMyin_ymaxymin4)) + HmMin,HmMax,HmMin);
        RR1 = clamp(((RR1Max-RR1Min) * (uvMyin_ymaxymin5)) + RR1Min,RR1Min,RR1Max);
#endif
        BR1 = float3(RR1,rg,rb);
    }
    else
    {
        BR1 = float3(RR1,rg,rb);
    }
	
	
    if (uvMouse.y < Dsfact) 
    {
        light_dir = normalize(float3(newx, -(0.01), (fov/-2.0)));
		moon_light_dir2 = normalize(float3((uvMouse.x-mlightoffsetx1)-(0.5*AR), uvMoonpos_y2 , (fov/-2.0)));
		spec1 = clamp(((specMax-specMin) * ((uvMouse.y-spymax)/(symin-spymax))) + specMin,specMin,specMax);
		gam = clamp(((gamMax-gamMin) * (uvMyin_ymaxymin)) + gamMin,gamMin,gamMax);
		lensb = clamp(((lensbMax-0.) * (uvMyin_ymaxyminA)) + 0.,0.,lensbMax);
        att = 1.;
        sunatt = 0.; 
        staratt = min(0.7,staratt);
		
        if(tonemap != 9.0)
        {
#if EXPENSIVE_CALC
            Hm1 = clamp(((HmMax - HmMin) * ((uvMouse.y - spymax) / (symin - spymax))) + HmMin, HmMin, HmMax);
#endif
        }
        else
        {
            Hm1 = clamp((((HmMin - HmMax) * ((uvMouse.y - 0.25) / (0.25 - 0.2)))) + HmMin, HmMax, HmMin);
		    //Hm1 = 100.;
        }
    }
	
    if (uvMouse.y < hz0)
    {
        sLight = clamp(((sLightMax - sLightMin) * ((uvMouse.y - symin) / (symax - symin))) + sLightMin, sLightMin, sLightMax);
        sea_light_dir = normalize(float3(newx2, sLight, (fov / -2.0)));
        g1 = clamp(((gmax - gmin) * (uvMyin_ymaxymin)) + gmin, gmin, gmax);
        MI1 = clamp(((MImax - MImin) * (uvMyin_ymaxymin)) + MImin, MImin, MImax);
		
		
        if (tonemap != 9.)
        {
            Hm1 = clamp(((HmMax - HmMin) * (uvMyin_ymaxymin)) + HmMin, HmMin, HmMax);

        }
		
		
	
        if (uvMouse.y < Dsfact * 0.95)
        {
            //Do not add eclipse effect once sun is below horizon
        }
        else
        {
            if (diffEI >= -eclispethreshold && diffEI <= eclispethreshold)
            {
                EI = clamp((((EImax - EImin) * (abs(diffEI) / (eclispethreshold - 0.))) + EImin), EImin, EImax);
                RE = clamp((((rain13 - 1.) * (abs(diffEI) / (eclispethreshold - 0.))) + 1.), 0., 1.);
                MI1 = EI;
            }
        }

        RI1 = clamp(((RImax - RImin) * ((uvMouse.y - ymin) / (ymax - ymin))) + RImin, RImin, RImax);
 
        att = 1.;
        staratt = min(0.7, staratt);
    }
    else
    {
        //eclipse
        if (diffEI >= -eclispethreshold && diffEI <= eclispethreshold)
        {
            EI = clamp((((EImax - EImin) * (abs(diffEI) / (eclispethreshold - 0.))) + EImin), EImin, EImax);
            RE = clamp((((rain13 - 1.) * (abs(diffEI) / (eclispethreshold - 0.))) + 1.), 0., 1.);
            MI1 = EI;
        }
    }


    //b = milkyway b1 = fireworks
    float2 b,b1,b2;
    b = b1 = b2 = (pin.texUV.xy / (iResolution.xy)) * 2.0 - 1.0; //center coordinates?
    b1.y +=Dfact;

    float z = 1.2;
    if (AR<1.0){
        z = 2.4;
    }
    else {
        z = 1.2;
    }

    float zoom = (2.0 *AR) * z;
    b   /=zoom;
    b.x *= AR;
    b1.x *= AR;
	
	    //draw the badly implemented sun
#if SIMPLE_SUN
    float2 uv1 = (pin.texUV.xy / iResolution.xy);
    uv1 *= M;
    uv1.x *= AR;

    float zenithoffset = (1. - Dfact) * 0.48;

    float3 sun2 = getAtmosphericScattering(uv1, uvMouse, zenithoffset);
#endif

    //draw the reflection
    if (camera_vector.y < -0.01)
    {
        float L = -camera_position.y / camera_vector.y;
        camera_position = camera_position + camera_vector * L;
        camera_vector.y = -camera_vector.y;
        q.y = (Dfact * -(moonsz * 2.)) - q.y;
        b1.y = -b1.y;
        b.y = -b.y;

        refatt = .6;
        att_moon = .2;
        star = stars(float3(mul(camera_vector.xy, ma2), camera_vector.z - camz));
        if (uvMouse.y < Dsfact)
        {
            aurora(float3(0., camera_height / 10000., (camera_depth / 10000.) - 6.7), camera_vector, a_pos, aur);
            aur = smoothstep(0.0, 2.5, aur);
        }
        else
        {
            aur = aur;
        }
    }
    else
    {
        float L1 = camera_position.y / camera_vector.y;
        float3 O1 = camera_position + camera_vector * L1;
        float3 D1 = ONE_3;
        D1 = normalize(camera_vector + float3(1., 0.0009 * sin(t + 6.2831 * noise(O1.xz + float2(0., t * 0.8))), 0.));
        D1.xy = mul(D1.xy, ma2);
        star = stars(D1);
        if (uvMouse.y < Dsfact)
        {
            aurora(float3(0., camera_height / 10000., (camera_depth / 10000.) - 6.7), camera_vector, a_pos, aur);
            aur = smoothstep(0.0, 1.5, aur) * fade;
        }
        else
        {
            aur = aur;
        }
    }
	
	q.y > (0.2 * (moon_size/2.)/M) ? thing = 1.:  thing = (q.y / ((moon_size/2.)/M))*5.;

    q.x += -uvMouseN.x;
    q.y += -mfixy;
    
    q.x *= AR;

    //q *= ma;
    //q += 0.5;
	
    b.xy = mul(b.xy, ma2);
    b +=0.5;
	
    if (fireworkson > 0.)
    {
        fireworks(b1, MI2, fire, fireworkCol);
    }
    
    
    
    //float3 moon_texture1= textureLod(iChannel1,q, moonlod ).rgb;
    float3 moon_texture1 = iChannel1.SampleLevel(LinearWarp, q, moonlod).rgb;
    //moon_texture1 = pow(moon_texture1,float3(2.2));
	
    float4 layer1 = float4(0.0, 0.0, 0.0, 1.0);
    //float3 layer2 = circle2(q.xy, q.xy*0.5, 0.1, float3(1.,1.,1.));
    float3 layer2 = DrawCircle(q,0.05,0.01);
    float3 moon_texture = layer2*moon_texture1.rgb;
    

    //float3 moon_texture = textureLod(iChannel1,q, 4. ).xyz;
    moon_texture = pow(moon_texture, TWO_POINT_TWO_3);
    
#if ENABLE_MOON_2
    themoon *= moon_texture;
    themoon *= att_moon;
    color += themoon;
#endif

#if ENABLE_MOON
    moon_texture *=att_moon;
	//moon_texture.rgb = pow(moon_texture.rgb,TWO_POINT_TWO_3);

#endif


    float3 Dn = camera_vector;
 
    Dn.xy = mul(Dn.xy, ma2);
    Dn += 0.5;


    //float3 back_texture = textureLod(iChannel2,float2(b.x,b.y), 0. ).xyz;
    float3 back_texture = iChannel2.SampleLevel(LinearWarp, float2(b.x, b.y), 0).xyz;
    back_texture = pow(back_texture, TWO_POINT_TWO_3) * 0.8;
	

    float3 newmie = ZERO_3;
    float3 nightmie = ZERO_3;
	

    scatter(camera_position, camera_vector,light_dir,uvMouse,moon_light_dir,a_pos,aur.rgb,BR1,bM11,MI1,RI1,Hm1,g1,raleigh,scat,themie,thesun,themoon,depthM,auro);
 
    
	float seed = round(t, 1.);
    float metx = random(seed);

	float3 met = meteorstorm(float2(camera_vector.x - metx,camera_vector.y+0.5));
	
	float scatfactor = ((abs(camera_vector.y)/0.5) * 0.00003) +0.00001;

    scat = 1.0 - clamp(depthM*scatfactor,0.,1.);

	
	themie *=  refatt;
	raleigh *= refatt;//  *  (scat);
	
		
    if (uvMouse.y > hz0)
    {
        
    }
    else
    {
        sunscat = scat - (-1. + ((uvMouse.y - hz0) / (hz0 - hz1)));
    }
	

	
	float ralatt = 1.0;
	float mieatt = 1.0;
    if (uvMouse.y >= Dsfact)
    {
    }
    else
    {
        float3 weights2_ = float3(0.2125, 0.7154, 0.0721); // sums to 1
        float newmie2 = dot(themie, weights2_);
        newmie = lerp(themie, float3(newmie2, newmie2, newmie2), 1. - (max(hz2, uvMouse.y) - hz2) / 0.04);
        ralatt = (max(0.2, uvMouse.y) - 0.2) / (0.25 - 0.2);
        mieatt = clamp((max(hz2, uvMouse.y) - hz2) * 25., 0.15, 1.0);

        color += float3(0.0, 0.0, 0.01 * (1. - (max(hz2, uvMouse.y) - hz2) / 0.04));
        themie = newmie;
        themie = themie;
		
    }
	
	color += back_texture.rgb * scat * scatatt;

    themoon *= moonatt;
    thesun *=  sunatt;
  
    star *=    staratt * refatt * scat;
    met *=     staratt * refatt * scat;
	fire *=     staratt * refatt * scat;
    

    float3 mcolor = ((SCAT_3)) * themie;

    if (diffEI >= -eclispethreshold && diffEI <= eclispethreshold)
    {
        float3 newsun = clamp(thesun, 0., 1.);
        float3 newsun2 = clamp(sun2, 0., 1.);
        color += (themoon * moonillumination * scat) * (1. - diffEIp);
        color += moon_texture.rgb * scat * (1. - diffEIp);
        float3 moon2 = clamp(moon_texture.rgb * 1000., 0., 1.);
        //moon2 = lerp(moon_texture.rgb,moon2,diffEIp);
        color += clamp(newsun - (moon2), 0., 1.) * refatt * (scat);
        color += clamp(newsun2 - (moon2), 0., 1.) * refatt * (scat);
    }
    else
    {
        if (tonemap == 0. || tonemap == 9.)
        {
            mcolor *= moon_texture.rgb * 30.;
            color += lerp(mcolor, color, 0.9);
        }
        color += sun2 * (scat);
        color += thesun * (scat);
        color += (themoon * moonillumination * scat);
        color += moon_texture.rgb * scat;
		
    }
    
    color += clamp(auro * (aur.rgb * 2. * themie) * scatatt * refatt, 0., 1.);


    if (tonemap != 12.)
    {
        raleigh = lerp(raleigh * refatt, raleigh * refatt * scat, RE);
    }
    else
    {
        raleigh *= refatt * (scat);
    }

    if (tonemap == 1. || tonemap == 9.)
    {
        color += pow(raleigh * ralatt, float3(rgam, rgam, rgam));
    }
    else
    {
        color += raleigh * ralatt;
    }

    if (tonemap == 2. || tonemap == 9.)
    {
        color += pow(themie, float3(gam, gam, gam));

    }
    else
    {
        color += themie;
    }

    color += pow(star, TWO_POINT_TWO_3);
    color += pow(met, TWO_POINT_TWO_3);

    float3 firescatter = ((ONE_MIN_SCAT_3)) * themie;
    firescatter *= (fireworkCol * MI2);
    color += lerp(firescatter, color, 0.5) * scatatt;
    color += fire;
    //color += clamp(fire,0.0,2.0);

    float3 aur_col = bg1 * (1. - aur.a) + (aur.rgb * 2.) * (scat);
    aur_col = pow(aur_col, TWO_POINT_TWO_3);
    aur_col *= scatatt * refatt;
    color += aur_col;

    //float3 acolor = clamp(((float3(1.0-scat)))*(themie-0.01),0.,1.);
    //acolor *=  aur.rgb*(2.);
    //color += lerp(acolor,color,0.5) *scatatt;


#if ENABLE_RAIN
    if (rainvol > 0.)
    {
        float rainangle = winddirx;
        float2 st = (uv * float2(.5 + (uv0.y + 2.) * .5, .02) + float2(t * .5 + uv0.y * rainangle, t * 0.1));
        //float2 st =  uv * float2(.5 +(uv0.y +2.) *.5, .02) + float2(t*.5 + uv0.y*.2, t*.1);
        float f = iChannel0.SampleLevel(LinearWarp, st, -100.0).y * iChannel0.SampleLevel(LinearWarp, st * float2(0.773, 1.0), -100.0).x * 1.55;
        float rain = clamp(rainvol - .15, 0.0, 1.0); //amount of rain
        f = clamp(pow(abs(f), 15.0) * 1.0 * (rain * rain * 5.0), 0.0, (uv0.y + .1) * .6);
        color = lerp(color, float3(0.15, 0.15, 0.15), f);
    }
#endif

    color += rainbow(camera_vector.xy, light_dir_original) * refatt * clamp(min(RI1 / RI0, MI1 / MI0) * ralatt, 0., 1.) * clamp(scat + 0.1, 0., 1.);

    if (snowvol > 0.)
    {
        float2 uv2 = (pin.texUV - .5 * iResolution.xy) / iResolution.y;

        float3 acc = float3(0, 0, 0);
        for (float i = 0.; i < LAYERS; i++)
        {
            acc += SnowSingleLayer(uv2, i);
        }


        color += acc.rgb;
    }


#if ENABLE_LENS_FLARE
    if (lensflare > 0.)
    {
        float2 mouse = iMouse.xy / iResolution.xy - 0.5;
        mouse.x *= iResolution.x / iResolution.y; //fix aspect ratio
        if (lensflaretype == 1.)
        {
            lensf = float3(1.4, 1.2, 1.0) * lensFlare(camera_vector.xy, normalize(float3(uvMouse.x - (0.5 * AR), uvMouse.y - Dsfact, (fov / -2.0))).xy, lensflaretype); //*clamp

        }
        else
        {
            lensf = float3(1.4, 1.2, 1.0) * lensFlare(uvl, mouse, lensflaretype); //*clamp( flare+.3, 0., 1.);
        }
		//lensf += flamingSun(uvl,mouse);
        lensf = clamp(lensf * scat * lensb, 0., 1.);
        lensf = pow(lensf, float3(lensgam, lensgam, lensgam));
        color += lensf * ralatt;
    }
#endif

    color = pow(color, INV_TWO_POINT_TWO_3);
    //color =pow(color, float3(1./2.2));

#if ENABLE_SEA
#define AA
#ifdef AA
    int aa1a = int(aa1);
    int aa2a = int(aa2);
    float aadiff = abs(aa2 - aa1) + 1.;
    float aapow = pow(aadiff, 2.);
    float3 seacolor = ZERO_3;
    for (int i = aa1a; i <= aa2a; i++)
    {
        for (int j = aa1a; j <= aa2a; j++)
        {
            float2 uv = pin.texUV + float2(i, j) / aadiff;
            seacolor += getPixel(uv, t, color, sea_light_dir, spec1, moon_light_dir2, mLight, scat);
        }
    }
    seacolor /= aapow;
#else
	float3 seacolor = getPixel(pin.texUV, t,color,sea_light_dir,spec1,moon_light_dir2,mLight,scat);
#endif
    if (tonemap == 3.)
    {
        color = seacolor;
    }
    else
    {
        float L2 = camera_position.y / camera_vector.y;
        if (L2 < 1.)
        {

            color = lerp(color, seacolor, pow(smoothstep(0.0, -0.05, -camera_vector.y), 0.5));
        }
    }
#endif

    if (scatonly == 1.)
    {
        color = SCAT_3; //white is sky
    }
    if (scatonly == 2.)
    {
        color = ONE_MIN_SCAT_3; //white is cloud
    }
    if (scatonly == 3.)
    {
        color = float3(0.5 - scat, 0.5 - scat, 0.5 - scat);
    }
    if (scatonly == 4.)
    {
        color = (ONE_MIN_SCAT_3) * pow(float3(0.0, 1., 1.5) * (auroraINT * 0.1) * scatatt, TWO_POINT_TWO_3);
    }
    if (scatonly == 5.)
    {
        color = themie; //white is cloud;
    }
    if (scatonly == 6.)
    {
        color = 1. - themie; //white is sky;
    }

    if (scatonly == 7.)
    {
        color = SCAT_3 * themie; //white is sky;
    }

    if (scatonly == 8.)
    {
        color = ONE_MIN_SCAT_3 * themie; //white is sky;
    }

    if (scatonly == 9.)
    {
        color = SCAT_3 * (1.0 - themie); //white is sky;
    }

    if (scatonly == 10.)
    {
        color = ONE_MIN_SCAT_3 * (1.0 - themie); //white is sky;
    }

    if (scatonly == 11.)
    {
        float3 acolor2 = clamp(((ONE_MIN_SCAT_3)) * (themie - 0.01), 0., 1.);
        acolor2 *= aur.rgb * (20.);
        color = acolor2 * scatatt;
    }
    if (scatonly == 12.)
    {
        color = SCAT_3 * themie; //white is sky;
        color += raleigh;
    }

    if (scatonly == 13.)
    {
        color = SCAT_3 + themie;
        color += raleigh;
    }
    if (scatonly == 14.)
    {
        color = SCAT_3;
        color += raleigh;
    }
    if (scatonly == 15.)
    {
        color = themie;
        color += raleigh;
    }
    if (scatonly == 16.)
    {
    //color = themie;
        color = raleigh;
    }
    if (scatonly == 17.)
    {
    //color = themie;
        color = SCAT_3 * raleigh;
    }
    if (scatonly == 18.)
    {
    //color = themie;
        color = lensf;
    }
    if (scatonly == 19.)
    {
        color = auro;
    }
    if (scatonly == 20.)
    {
        color = auro * ((aur.rgb) * themie) * scatatt * refatt;
    }
    if (scatonly == 21.)
    {
        color = auro * ((aur.rgb)) * scatatt * refatt;
    }
    if (scatonly == 22.)
    {
        color = aur.rgb;
    }
    if (scatonly == 23.)
    {
        color = aur_col;
    }
    if (scatonly == 24.)
    {
        color = aur_col;
        color += auro * ((aur.rgb) * themie) * scatatt * refatt;
    }


    float3 weights_ = float3(0.2125, 0.7154, 0.0721); // sums to 1
    float luminance_ = dot(color, weights_);
    color = lerp(float3(luminance_, luminance_, luminance_), color, sat);

    //vibrance
    float average = (color.r + color.g + color.b) / 3.;
    float mx = max(color.r, max(color.g, color.b));
    float vibrance = (mx - average) * vib;
    color = clamp(color - (mx - color) * vibrance, 0., 1.);

    color = shiftHue(color, hue);
    float4 fragColor = float4(color, 1.);
    //fragColor = float4(pow(color, float3(1.0/2.2)), 1.); //Gamma Correction
    //fragColor = float4(pow(color, float3(1.0/1.5)), 1.);
    
    return fragColor;

}