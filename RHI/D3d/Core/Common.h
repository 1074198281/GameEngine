#pragma once

#include <DirectXMath.h>
#include <intrin.h>

#define INLINE __forceinline

namespace Math
{
    using namespace DirectX;

    INLINE XMVECTOR SplatZero()
    {
        return XMVectorZero();
    }

#if !defined(_XM_NO_INTRINSICS_) && defined(_XM_SSE_INTRINSICS_)

    INLINE XMVECTOR SplatOne(XMVECTOR zero = SplatZero())
    {
        __m128i AllBits = _mm_castps_si128(_mm_cmpeq_ps(zero, zero));
        return _mm_castsi128_ps(_mm_slli_epi32(_mm_srli_epi32(AllBits, 25), 23));	// return 0x3F800000
        //return _mm_cvtepi32_ps(_mm_srli_epi32(SetAllBits(zero), 31));				// return (float)1;  (alternate method)
    }

#if defined(_XM_SSE4_INTRINSICS_)
    INLINE XMVECTOR CreateXUnitVector(XMVECTOR one = SplatOne())
    {
        return _mm_insert_ps(one, one, 0x0E);
    }
    INLINE XMVECTOR CreateYUnitVector(XMVECTOR one = SplatOne())
    {
        return _mm_insert_ps(one, one, 0x0D);
    }
    INLINE XMVECTOR CreateZUnitVector(XMVECTOR one = SplatOne())
    {
        return _mm_insert_ps(one, one, 0x0B);
    }
    INLINE XMVECTOR CreateWUnitVector(XMVECTOR one = SplatOne())
    {
        return _mm_insert_ps(one, one, 0x07);
    }
    INLINE XMVECTOR SetWToZero(FXMVECTOR vec)
    {
        return _mm_insert_ps(vec, vec, 0x08);
    }
    INLINE XMVECTOR SetWToOne(FXMVECTOR vec)
    {
        return _mm_blend_ps(vec, SplatOne(), 0x8);
    }
#else
    INLINE XMVECTOR CreateXUnitVector(XMVECTOR one = SplatOne())
    {
        return _mm_castsi128_ps(_mm_srli_si128(_mm_castps_si128(one), 12));
    }
    INLINE XMVECTOR CreateYUnitVector(XMVECTOR one = SplatOne())
    {
        XMVECTOR unitx = CreateXUnitVector(one);
        return _mm_castsi128_ps(_mm_slli_si128(_mm_castps_si128(unitx), 4));
    }
    INLINE XMVECTOR CreateZUnitVector(XMVECTOR one = SplatOne())
    {
        XMVECTOR unitx = CreateXUnitVector(one);
        return _mm_castsi128_ps(_mm_slli_si128(_mm_castps_si128(unitx), 8));
    }
    INLINE XMVECTOR CreateWUnitVector(XMVECTOR one = SplatOne())
    {
        return _mm_castsi128_ps(_mm_slli_si128(_mm_castps_si128(one), 12));
    }
    INLINE XMVECTOR SetWToZero(FXMVECTOR vec)
    {
        __m128i MaskOffW = _mm_srli_si128(_mm_castps_si128(_mm_cmpeq_ps(vec, vec)), 4);
        return _mm_and_ps(vec, _mm_castsi128_ps(MaskOffW));
    }
    INLINE XMVECTOR SetWToOne(FXMVECTOR vec)
    {
        return _mm_movelh_ps(vec, _mm_unpackhi_ps(vec, SplatOne()));
    }
#endif

#else // !_XM_SSE_INTRINSICS_

    INLINE XMVECTOR SplatOne() { return XMVectorSplatOne(); }
    INLINE XMVECTOR CreateXUnitVector() { return g_XMIdentityR0; }
    INLINE XMVECTOR CreateYUnitVector() { return g_XMIdentityR1; }
    INLINE XMVECTOR CreateZUnitVector() { return g_XMIdentityR2; }
    INLINE XMVECTOR CreateWUnitVector() { return g_XMIdentityR3; }
    INLINE XMVECTOR SetWToZero(FXMVECTOR vec) { return XMVectorAndInt(vec, g_XMMask3); }
    INLINE XMVECTOR SetWToOne(FXMVECTOR vec) { return XMVectorSelect(g_XMIdentityR3, vec, g_XMMask3); }

#endif

    enum EZeroTag { kZero, kOrigin };
    enum EIdentityTag { kOne, kIdentity };
    enum EXUnitVector { kXUnitVector };
    enum EYUnitVector { kYUnitVector };
    enum EZUnitVector { kZUnitVector };
    enum EWUnitVector { kWUnitVector };

}
