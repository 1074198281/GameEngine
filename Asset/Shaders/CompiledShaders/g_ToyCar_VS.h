#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer cbCommon
// {
//
//   float4x4 gModelMatrix;             // Offset:    0 Size:    64
//   float4x4 gViewMatrix;              // Offset:   64 Size:    64
//   float4x4 gProjMatrix;              // Offset:  128 Size:    64
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      ID      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- ------- -------------- ------
// cbCommon                          cbuffer      NA          NA     CB0            cb1      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// POSITION                 0   xyz         0     NONE   float   xyz 
// NORMAL                   0   xyz         1     NONE   float   xyz 
// TEXCOORD                 0   xy          2     NONE   float   xy  
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float   xyzw
// POSITION                 0   xyzw        1     NONE   float   xyzw
// NORMAL                   0   xyz         2     NONE   float   xyz 
// TEXCOORD                 0   xy          3     NONE   float   xy  
//
vs_5_1
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[1:1][12], immediateIndexed, space=0
dcl_input v0.xyz
dcl_input v1.xyz
dcl_input v2.xy
dcl_output_siv o0.xyzw, position
dcl_output o1.xyzw
dcl_output o2.xyz
dcl_output o3.xy
dcl_temps 7
mul r0.xyzw, CB0[1][4].yyyy, CB0[1][9].xyzw
mad r0.xyzw, CB0[1][4].xxxx, CB0[1][8].xyzw, r0.xyzw
mad r0.xyzw, CB0[1][4].zzzz, CB0[1][10].xyzw, r0.xyzw
mad r0.xyzw, CB0[1][4].wwww, CB0[1][11].xyzw, r0.xyzw
mul r1.xyzw, CB0[1][5].yyyy, CB0[1][9].xyzw
mad r1.xyzw, CB0[1][5].xxxx, CB0[1][8].xyzw, r1.xyzw
mad r1.xyzw, CB0[1][5].zzzz, CB0[1][10].xyzw, r1.xyzw
mad r1.xyzw, CB0[1][5].wwww, CB0[1][11].xyzw, r1.xyzw
mul r2.xyzw, CB0[1][6].yyyy, CB0[1][9].xyzw
mad r2.xyzw, CB0[1][6].xxxx, CB0[1][8].xyzw, r2.xyzw
mad r2.xyzw, CB0[1][6].zzzz, CB0[1][10].xyzw, r2.xyzw
mad r2.xyzw, CB0[1][6].wwww, CB0[1][11].xyzw, r2.xyzw
mul r3.xyzw, CB0[1][7].yyyy, CB0[1][9].xyzw
mad r3.xyzw, CB0[1][7].xxxx, CB0[1][8].xyzw, r3.xyzw
mad r3.xyzw, CB0[1][7].zzzz, CB0[1][10].xyzw, r3.xyzw
mad r3.xyzw, CB0[1][7].wwww, CB0[1][11].xyzw, r3.xyzw
mul r4.xyzw, r1.xyzw, CB0[1][0].yyyy
mad r4.xyzw, CB0[1][0].xxxx, r0.xyzw, r4.xyzw
mad r4.xyzw, CB0[1][0].zzzz, r2.xyzw, r4.xyzw
mad r4.xyzw, CB0[1][0].wwww, r3.xyzw, r4.xyzw
mul r5.xyzw, r1.xyzw, CB0[1][1].yyyy
mad r5.xyzw, CB0[1][1].xxxx, r0.xyzw, r5.xyzw
mad r5.xyzw, CB0[1][1].zzzz, r2.xyzw, r5.xyzw
mad r5.xyzw, CB0[1][1].wwww, r3.xyzw, r5.xyzw
mul r6.xyzw, r1.xyzw, CB0[1][2].yyyy
mad r6.xyzw, CB0[1][2].xxxx, r0.xyzw, r6.xyzw
mad r6.xyzw, CB0[1][2].zzzz, r2.xyzw, r6.xyzw
mad r6.xyzw, CB0[1][2].wwww, r3.xyzw, r6.xyzw
mul r1.xyzw, r1.xyzw, CB0[1][3].yyyy
mad r0.xyzw, CB0[1][3].xxxx, r0.xyzw, r1.xyzw
mad r0.xyzw, CB0[1][3].zzzz, r2.xyzw, r0.xyzw
mad r0.xyzw, CB0[1][3].wwww, r3.xyzw, r0.xyzw
mul r1.xyzw, r5.xyzw, v0.yyyy
mad r1.xyzw, v0.xxxx, r4.xyzw, r1.xyzw
mad r1.xyzw, v0.zzzz, r6.xyzw, r1.xyzw
add o0.xyzw, r0.xyzw, r1.xyzw
mul r0.xyzw, v0.yyyy, CB0[1][1].xyzw
mad r0.xyzw, v0.xxxx, CB0[1][0].xyzw, r0.xyzw
mad r0.xyzw, v0.zzzz, CB0[1][2].xyzw, r0.xyzw
add o1.xyzw, r0.xyzw, CB0[1][3].xyzw
mov o2.xyz, v1.xyzx
mov o3.xy, v2.xyxx
ret 
// Approximately 43 instruction slots used
#endif

const BYTE g_pToyCar_VS[] =
{
     68,  88,  66,  67, 131, 248, 
     59,  78, 114, 246,  36, 170, 
    201,  42, 147,  77, 253, 234, 
    242,  62,   1,   0,   0,   0, 
    252,  10,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    184,   1,   0,   0,  44,   2, 
      0,   0, 196,   2,   0,   0, 
     96,  10,   0,   0,  82,  68, 
     69,  70, 124,   1,   0,   0, 
      1,   0,   0,   0, 112,   0, 
      0,   0,   1,   0,   0,   0, 
     60,   0,   0,   0,   1,   5, 
    254, 255,   0,   5,   0,   0, 
     84,   1,   0,   0,  19,  19, 
     68,  37,  60,   0,   0,   0, 
     24,   0,   0,   0,  40,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    100,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  99,  98, 
     67, 111, 109, 109, 111, 110, 
      0, 171, 171, 171, 100,   0, 
      0,   0,   3,   0,   0,   0, 
    136,   0,   0,   0, 192,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   1, 
      0,   0,   0,   0,   0,   0, 
     64,   0,   0,   0,   2,   0, 
      0,   0,  24,   1,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  60,   1,   0,   0, 
     64,   0,   0,   0,  64,   0, 
      0,   0,   2,   0,   0,   0, 
     24,   1,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     72,   1,   0,   0, 128,   0, 
      0,   0,  64,   0,   0,   0, 
      2,   0,   0,   0,  24,   1, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 103,  77, 
    111, 100, 101, 108,  77,  97, 
    116, 114, 105, 120,   0, 102, 
    108, 111,  97, 116,  52, 120, 
     52,   0, 171, 171,   3,   0, 
      3,   0,   4,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     13,   1,   0,   0, 103,  86, 
    105, 101, 119,  77,  97, 116, 
    114, 105, 120,   0, 103,  80, 
    114, 111, 106,  77,  97, 116, 
    114, 105, 120,   0,  77, 105, 
     99, 114, 111, 115, 111, 102, 
    116,  32,  40,  82,  41,  32, 
     72,  76,  83,  76,  32,  83, 
    104,  97, 100, 101, 114,  32, 
     67, 111, 109, 112, 105, 108, 
    101, 114,  32,  49,  48,  46, 
     49,   0,  73,  83,  71,  78, 
    108,   0,   0,   0,   3,   0, 
      0,   0,   8,   0,   0,   0, 
     80,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   7,   7,   0,   0, 
     89,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,   7,   7,   0,   0, 
     96,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,   3,   3,   0,   0, 
     80,  79,  83,  73,  84,  73, 
     79,  78,   0,  78,  79,  82, 
     77,  65,  76,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0, 171, 171, 171,  79,  83, 
     71,  78, 144,   0,   0,   0, 
      4,   0,   0,   0,   8,   0, 
      0,   0, 104,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   0, 116,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,  15,   0, 
      0,   0, 125,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,   7,   8, 
      0,   0, 132,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      3,   0,   0,   0,   3,  12, 
      0,   0,  83,  86,  95,  80, 
     79,  83,  73,  84,  73,  79, 
     78,   0,  80,  79,  83,  73, 
     84,  73,  79,  78,   0,  78, 
     79,  82,  77,  65,  76,   0, 
     84,  69,  88,  67,  79,  79, 
     82,  68,   0, 171, 171, 171, 
     83,  72,  69,  88, 148,   7, 
      0,   0,  81,   0,   1,   0, 
    229,   1,   0,   0, 106,   8, 
      0,   1,  89,   0,   0,   7, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
     95,   0,   0,   3, 114,  16, 
     16,   0,   0,   0,   0,   0, 
     95,   0,   0,   3, 114,  16, 
     16,   0,   1,   0,   0,   0, 
     95,   0,   0,   3,  50,  16, 
     16,   0,   2,   0,   0,   0, 
    103,   0,   0,   4, 242,  32, 
     16,   0,   0,   0,   0,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3, 114,  32,  16,   0, 
      2,   0,   0,   0, 101,   0, 
      0,   3,  50,  32,  16,   0, 
      3,   0,   0,   0, 104,   0, 
      0,   2,   7,   0,   0,   0, 
     56,   0,   0,  11, 242,   0, 
     16,   0,   0,   0,   0,   0, 
     86, 133,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      4,   0,   0,   0,  70, 142, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   9,   0, 
      0,   0,  50,   0,   0,  13, 
    242,   0,  16,   0,   0,   0, 
      0,   0,   6, 128,  48,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   4,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      8,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     50,   0,   0,  13, 242,   0, 
     16,   0,   0,   0,   0,   0, 
    166, 138,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      4,   0,   0,   0,  70, 142, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  10,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  50,   0, 
      0,  13, 242,   0,  16,   0, 
      0,   0,   0,   0, 246, 143, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   4,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  11,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  56,   0,   0,  11, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  86, 133,  48,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   5,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      9,   0,   0,   0,  50,   0, 
      0,  13, 242,   0,  16,   0, 
      1,   0,   0,   0,   6, 128, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   5,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   8,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  50,   0,   0,  13, 
    242,   0,  16,   0,   1,   0, 
      0,   0, 166, 138,  48,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   5,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     10,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,  13, 242,   0, 
     16,   0,   1,   0,   0,   0, 
    246, 143,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      5,   0,   0,   0,  70, 142, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  11,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,  11, 242,   0,  16,   0, 
      2,   0,   0,   0,  86, 133, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   6,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   9,   0,   0,   0, 
     50,   0,   0,  13, 242,   0, 
     16,   0,   2,   0,   0,   0, 
      6, 128,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      6,   0,   0,   0,  70, 142, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   8,   0, 
      0,   0,  70,  14,  16,   0, 
      2,   0,   0,   0,  50,   0, 
      0,  13, 242,   0,  16,   0, 
      2,   0,   0,   0, 166, 138, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   6,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  10,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,  50,   0,   0,  13, 
    242,   0,  16,   0,   2,   0, 
      0,   0, 246, 143,  48,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   6,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     11,   0,   0,   0,  70,  14, 
     16,   0,   2,   0,   0,   0, 
     56,   0,   0,  11, 242,   0, 
     16,   0,   3,   0,   0,   0, 
     86, 133,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      7,   0,   0,   0,  70, 142, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   9,   0, 
      0,   0,  50,   0,   0,  13, 
    242,   0,  16,   0,   3,   0, 
      0,   0,   6, 128,  48,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   7,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      8,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     50,   0,   0,  13, 242,   0, 
     16,   0,   3,   0,   0,   0, 
    166, 138,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      7,   0,   0,   0,  70, 142, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  10,   0, 
      0,   0,  70,  14,  16,   0, 
      3,   0,   0,   0,  50,   0, 
      0,  13, 242,   0,  16,   0, 
      3,   0,   0,   0, 246, 143, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   7,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  11,   0,   0,   0, 
     70,  14,  16,   0,   3,   0, 
      0,   0,  56,   0,   0,   9, 
    242,   0,  16,   0,   4,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  86, 133, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  50,   0,   0,  11, 
    242,   0,  16,   0,   4,   0, 
      0,   0,   6, 128,  48,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      4,   0,   0,   0,  50,   0, 
      0,  11, 242,   0,  16,   0, 
      4,   0,   0,   0, 166, 138, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      2,   0,   0,   0,  70,  14, 
     16,   0,   4,   0,   0,   0, 
     50,   0,   0,  11, 242,   0, 
     16,   0,   4,   0,   0,   0, 
    246, 143,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     70,  14,  16,   0,   4,   0, 
      0,   0,  56,   0,   0,   9, 
    242,   0,  16,   0,   5,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  86, 133, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  50,   0,   0,  11, 
    242,   0,  16,   0,   5,   0, 
      0,   0,   6, 128,  48,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      5,   0,   0,   0,  50,   0, 
      0,  11, 242,   0,  16,   0, 
      5,   0,   0,   0, 166, 138, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  70,  14,  16,   0, 
      2,   0,   0,   0,  70,  14, 
     16,   0,   5,   0,   0,   0, 
     50,   0,   0,  11, 242,   0, 
     16,   0,   5,   0,   0,   0, 
    246, 143,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     70,  14,  16,   0,   5,   0, 
      0,   0,  56,   0,   0,   9, 
    242,   0,  16,   0,   6,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  86, 133, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   2,   0, 
      0,   0,  50,   0,   0,  11, 
    242,   0,  16,   0,   6,   0, 
      0,   0,   6, 128,  48,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   2,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      6,   0,   0,   0,  50,   0, 
      0,  11, 242,   0,  16,   0, 
      6,   0,   0,   0, 166, 138, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   2,   0, 
      0,   0,  70,  14,  16,   0, 
      2,   0,   0,   0,  70,  14, 
     16,   0,   6,   0,   0,   0, 
     50,   0,   0,  11, 242,   0, 
     16,   0,   6,   0,   0,   0, 
    246, 143,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      2,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     70,  14,  16,   0,   6,   0, 
      0,   0,  56,   0,   0,   9, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  86, 133, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,  50,   0,   0,  11, 
    242,   0,  16,   0,   0,   0, 
      0,   0,   6, 128,  48,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  50,   0, 
      0,  11, 242,   0,  16,   0, 
      0,   0,   0,   0, 166, 138, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,  70,  14,  16,   0, 
      2,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     50,   0,   0,  11, 242,   0, 
     16,   0,   0,   0,   0,   0, 
    246, 143,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  56,   0,   0,   7, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  70,  14,  16,   0, 
      5,   0,   0,   0,  86,  21, 
     16,   0,   0,   0,   0,   0, 
     50,   0,   0,   9, 242,   0, 
     16,   0,   1,   0,   0,   0, 
      6,  16,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      4,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,   9, 242,   0, 
     16,   0,   1,   0,   0,   0, 
    166,  26,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      6,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
      0,   0,   0,   7, 242,  32, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   9, 242,   0,  16,   0, 
      0,   0,   0,   0,  86,  21, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  50,   0, 
      0,  11, 242,   0,  16,   0, 
      0,   0,   0,   0,   6,  16, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     50,   0,   0,  11, 242,   0, 
     16,   0,   0,   0,   0,   0, 
    166,  26,  16,   0,   0,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   2,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   9, 
    242,  32,  16,   0,   1,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     48,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,  54,   0,   0,   5, 
    114,  32,  16,   0,   2,   0, 
      0,   0,  70,  18,  16,   0, 
      1,   0,   0,   0,  54,   0, 
      0,   5,  50,  32,  16,   0, 
      3,   0,   0,   0,  70,  16, 
     16,   0,   2,   0,   0,   0, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 148,   0,   0,   0, 
     43,   0,   0,   0,   7,   0, 
      0,   0,   0,   0,   0,   0, 
      7,   0,   0,   0,  40,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0
};