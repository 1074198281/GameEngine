#pragma once

#include "XM_Header.h"
#include "XM_Vector.h"

namespace XM_Math {
#define CREATE_SIMD_FUNCTIONS( TYPE ) \
	inline TYPE Sqrt( TYPE s ) { return TYPE(DirectX::XMVectorSqrt(s)); } \
	inline TYPE Recip( TYPE s ) { return TYPE(DirectX::XMVectorReciprocal(s)); } \
	inline TYPE RecipSqrt( TYPE s ) { return TYPE(DirectX::XMVectorReciprocalSqrt(s)); } \
	inline TYPE Floor( TYPE s ) { return TYPE(DirectX::XMVectorFloor(s)); } \
	inline TYPE Ceiling( TYPE s ) { return TYPE(DirectX::XMVectorCeiling(s)); } \
	inline TYPE Round( TYPE s ) { return TYPE(DirectX::XMVectorRound(s)); } \
	inline TYPE Abs( TYPE s ) { return TYPE(DirectX::XMVectorAbs(s)); } \
	inline TYPE Exp( TYPE s ) { return TYPE(DirectX::XMVectorExp(s)); } \
	inline TYPE Pow( TYPE b, TYPE e ) { return TYPE(DirectX::XMVectorPow(b, e)); } \
	inline TYPE Log( TYPE s ) { return TYPE(DirectX::XMVectorLog(s)); } \
	inline TYPE Sin( TYPE s ) { return TYPE(DirectX::XMVectorSin(s)); } \
	inline TYPE Cos( TYPE s ) { return TYPE(DirectX::XMVectorCos(s)); } \
	inline TYPE Tan( TYPE s ) { return TYPE(DirectX::XMVectorTan(s)); } \
	inline TYPE ASin( TYPE s ) { return TYPE(DirectX::XMVectorASin(s)); } \
	inline TYPE ACos( TYPE s ) { return TYPE(DirectX::XMVectorACos(s)); } \
	inline TYPE ATan( TYPE s ) { return TYPE(DirectX::XMVectorATan(s)); } \
	inline TYPE ATan2( TYPE y, TYPE x ) { return TYPE(DirectX::XMVectorATan2(y, x)); } \
	inline TYPE Lerp( TYPE a, TYPE b, TYPE t ) { return TYPE(DirectX::XMVectorLerpV(a, b, t)); } \
    inline TYPE Lerp( TYPE a, TYPE b, float t ) { return TYPE(DirectX::XMVectorLerp(a, b, t)); } \
	inline TYPE Max( TYPE a, TYPE b ) { return TYPE(DirectX::XMVectorMax(a, b)); } \
	inline TYPE Min( TYPE a, TYPE b ) { return TYPE(DirectX::XMVectorMin(a, b)); } \
	inline TYPE Clamp( TYPE v, TYPE a, TYPE b ) { return Min(Max(v, a), b); } \
	inline BoolVector operator<  ( TYPE lhs, TYPE rhs ) { return DirectX::XMVectorLess(lhs, rhs); } \
	inline BoolVector operator<= ( TYPE lhs, TYPE rhs ) { return DirectX::XMVectorLessOrEqual(lhs, rhs); } \
	inline BoolVector operator>  ( TYPE lhs, TYPE rhs ) { return DirectX::XMVectorGreater(lhs, rhs); } \
	inline BoolVector operator>= ( TYPE lhs, TYPE rhs ) { return DirectX::XMVectorGreaterOrEqual(lhs, rhs); } \
	inline BoolVector operator== ( TYPE lhs, TYPE rhs ) { return DirectX::XMVectorEqual(lhs, rhs); } \
	inline TYPE Select( TYPE lhs, TYPE rhs, BoolVector mask ) { return TYPE(DirectX::XMVectorSelect(lhs, rhs, mask)); }


	CREATE_SIMD_FUNCTIONS(Scalar)
	CREATE_SIMD_FUNCTIONS(Vector3)
	CREATE_SIMD_FUNCTIONS(Vector4)

#undef CREATE_SIMD_FUNCTIONS

	inline float Sqrt(float s) { return Sqrt(Scalar(s)); }
	inline float Recip(float s) { return Recip(Scalar(s)); }
	inline float RecipSqrt(float s) { return RecipSqrt(Scalar(s)); }
	inline float Floor(float s) { return Floor(Scalar(s)); }
	inline float Ceiling(float s) { return Ceiling(Scalar(s)); }
	inline float Round(float s) { return Round(Scalar(s)); }
	inline float Abs(float s) { return s < 0.0f ? -s : s; }
	inline float Exp(float s) { return Exp(Scalar(s)); }
	inline float Pow(float b, float e) { return Pow(Scalar(b), Scalar(e)); }
	inline float Log(float s) { return Log(Scalar(s)); }
	inline float Sin(float s) { return Sin(Scalar(s)); }
	inline float Cos(float s) { return Cos(Scalar(s)); }
	inline float Tan(float s) { return Tan(Scalar(s)); }
	inline float ASin(float s) { return ASin(Scalar(s)); }
	inline float ACos(float s) { return ACos(Scalar(s)); }
	inline float ATan(float s) { return ATan(Scalar(s)); }
	inline float ATan2(float y, float x) { return ATan2(Scalar(y), Scalar(x)); }
	inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }
	inline float Max(float a, float b) { return a > b ? a : b; }
	inline float Min(float a, float b) { return a < b ? a : b; }
	inline float Clamp(float v, float a, float b) { return Min(Max(v, a), b); }

	inline Scalar Length(Vector3 v)
	{
		return Scalar(DirectX::XMVector3Length(v));
	}

	inline Scalar LengthSquare(Vector3 v)
	{
		return Scalar(DirectX::XMVector3LengthSq(v));
	}

	inline Scalar LengthRecip(Vector3 v)
	{
		return Scalar(DirectX::XMVector3ReciprocalLength(v));
	}

	inline Scalar Dot(Vector3 v1, Vector3 v2) {
		return Scalar(DirectX::XMVector3Dot(v1, v2));
	}

	inline Scalar Dot(Vector4 v1, Vector4 v2) {
		return Scalar(DirectX::XMVector4Dot(v1, v2));
	}
}