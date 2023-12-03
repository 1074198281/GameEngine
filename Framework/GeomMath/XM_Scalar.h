#pragma once

#include "XM_Header.h"

namespace XM_Math {
	class Scalar {
	public:
		Scalar() {}
		Scalar(const Scalar& s) { m_vec = s; }
		Scalar(float f) { m_vec = DirectX::XMVectorReplicate(f); }
		explicit Scalar(DirectX::FXMVECTOR vec) { m_vec = vec; }
		explicit Scalar(EZeroTag) { m_vec = SplatZero(); }
		explicit Scalar(EIdentityTag) { m_vec = SplatOne(); }

		inline operator DirectX::XMVECTOR() const { return m_vec; }
		inline operator float() const { return DirectX::XMVectorGetX(m_vec); }
	private:
		DirectX::XMVECTOR m_vec;
	};

	inline Scalar operator- (Scalar s) { return Scalar(DirectX::XMVectorNegate(s)); }
	inline Scalar operator+ (Scalar s1, Scalar s2) { return Scalar(DirectX::XMVectorAdd(s1, s2)); }
	inline Scalar operator- (Scalar s1, Scalar s2) { return Scalar(DirectX::XMVectorSubtract(s1, s2)); }
	inline Scalar operator* (Scalar s1, Scalar s2) { return Scalar(DirectX::XMVectorMultiply(s1, s2)); }
	inline Scalar operator/ (Scalar s1, Scalar s2) { return Scalar(DirectX::XMVectorDivide(s1, s2)); }
	inline Scalar operator+ (Scalar s1, float s2) { return s1 + Scalar(s2); }
	inline Scalar operator- (Scalar s1, float s2) { return s1 - Scalar(s2); }
	inline Scalar operator* (Scalar s1, float s2) { return s1 * Scalar(s2); }
	inline Scalar operator/ (Scalar s1, float s2) { return s1 / Scalar(s2); }
	inline Scalar operator+ (float s1, Scalar s2) { return Scalar(s1) + s2; }
	inline Scalar operator- (float s1, Scalar s2) { return Scalar(s1) - s2; }
	inline Scalar operator* (float s1, Scalar s2) { return Scalar(s1) * s2; }
	inline Scalar operator/ (float s1, Scalar s2) { return Scalar(s1) / s2; }

	inline bool operator<  (Scalar lhs, float rhs) { return (float)lhs < rhs; }
	inline bool operator<= (Scalar lhs, float rhs) { return (float)lhs <= rhs; }
	inline bool operator>  (Scalar lhs, float rhs) { return (float)lhs > rhs; }
	inline bool operator>= (Scalar lhs, float rhs) { return (float)lhs >= rhs; }
	inline bool operator== (Scalar lhs, float rhs) { return (float)lhs == rhs; }
	inline bool operator<  (float lhs, Scalar rhs) { return lhs < (float)rhs; }
	inline bool operator<= (float lhs, Scalar rhs) { return lhs <= (float)rhs; }
	inline bool operator>  (float lhs, Scalar rhs) { return lhs > (float)rhs; }
	inline bool operator>= (float lhs, Scalar rhs) { return lhs >= (float)rhs; }
	inline bool operator== (float lhs, Scalar rhs) { return lhs == (float)rhs; }
}