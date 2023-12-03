#pragma once
#include <DirectXMath.h>
#include <emmintrin.h>

#include "XM_Scalar.h"


namespace XM_Math {

    class Vector4;
	class Vector3 {
    public:
        inline Vector3() {}
        inline Vector3(float x, float y, float z) { m_vec = DirectX::XMVectorSet(x, y, z, z); }
        inline Vector3(const DirectX::XMFLOAT3& v) { m_vec = DirectX::XMLoadFloat3(&v); }
        inline Vector3(const Vector3& v) { m_vec = v; }
        inline Vector3(Scalar s) { m_vec = s; }
        inline explicit Vector3(Vector4 vec);
        inline explicit Vector3(DirectX::FXMVECTOR vec) { m_vec = vec; }
        inline explicit Vector3(EZeroTag) { m_vec = SplatZero(); }
        inline explicit Vector3(EIdentityTag) { m_vec = SplatOne(); }
        inline explicit Vector3(EXUnitVector) { m_vec = CreateXUnitVector(); }
        inline explicit Vector3(EYUnitVector) { m_vec = CreateYUnitVector(); }
        inline explicit Vector3(EZUnitVector) { m_vec = CreateZUnitVector(); }

        inline operator DirectX::XMVECTOR() const { return m_vec; }

        inline Scalar GetX() const { return Scalar(DirectX::XMVectorSplatX(m_vec)); }
        inline Scalar GetY() const { return Scalar(DirectX::XMVectorSplatY(m_vec)); }
        inline Scalar GetZ() const { return Scalar(DirectX::XMVectorSplatZ(m_vec)); }
        inline void SetX(Scalar x) { m_vec = DirectX::XMVectorPermute<4, 1, 2, 3>(m_vec, x); }
        inline void SetY(Scalar y) { m_vec = DirectX::XMVectorPermute<0, 5, 2, 3>(m_vec, y); }
        inline void SetZ(Scalar z) { m_vec = DirectX::XMVectorPermute<0, 1, 6, 3>(m_vec, z); }

        inline Vector3 operator- () const { return Vector3(DirectX::XMVectorNegate(m_vec)); }
        inline Vector3 operator+ (Vector3 v2) const { return Vector3(DirectX::XMVectorAdd(m_vec, v2)); }
        inline Vector3 operator- (Vector3 v2) const { return Vector3(DirectX::XMVectorSubtract(m_vec, v2)); }
        inline Vector3 operator* (Vector3 v2) const { return Vector3(DirectX::XMVectorMultiply(m_vec, v2)); }
        inline Vector3 operator/ (Vector3 v2) const { return Vector3(DirectX::XMVectorDivide(m_vec, v2)); }
        inline Vector3 operator* (Scalar  v2) const { return *this * Vector3(v2); }
        inline Vector3 operator/ (Scalar  v2) const { return *this / Vector3(v2); }
        inline Vector3 operator* (float  v2) const { return *this * Scalar(v2); }
        inline Vector3 operator/ (float  v2) const { return *this / Scalar(v2); }

        inline Vector3& operator += (Vector3 v) { *this = *this + v; return *this; }
        inline Vector3& operator -= (Vector3 v) { *this = *this - v; return *this; }
        inline Vector3& operator *= (Vector3 v) { *this = *this * v; return *this; }
        inline Vector3& operator /= (Vector3 v) { *this = *this / v; return *this; }

        inline friend Vector3 operator* (Scalar  v1, Vector3 v2) { return Vector3(v1) * v2; }
        inline friend Vector3 operator/ (Scalar  v1, Vector3 v2) { return Vector3(v1) / v2; }
        inline friend Vector3 operator* (float   v1, Vector3 v2) { return Scalar(v1) * v2; }
        inline friend Vector3 operator/ (float   v1, Vector3 v2) { return Scalar(v1) / v2; }

    private:
        DirectX::XMVECTOR m_vec;
	};

    class Vector4 {
    public:
        inline Vector4() {}
        inline Vector4(float x, float y, float z, float w) { m_vec = DirectX::XMVectorSet(x, y, z, w); }
        inline Vector4(const DirectX::XMFLOAT4& v) { m_vec = DirectX::XMLoadFloat4(&v); }
        inline Vector4(Vector3 xyz, float w) { m_vec = DirectX::XMVectorSetW(xyz, w); }
        inline Vector4(const Vector4& v) { m_vec = v; }
        inline Vector4(const Scalar& s) { m_vec = s; }
        inline explicit Vector4(Vector3 xyz) { m_vec = SetWToOne(xyz); }
        inline explicit Vector4(DirectX::FXMVECTOR vec) { m_vec = vec; }
        inline explicit Vector4(EZeroTag) { m_vec = SplatZero(); }
        inline explicit Vector4(EIdentityTag) { m_vec = SplatOne(); }
        inline explicit Vector4(EXUnitVector) { m_vec = CreateXUnitVector(); }
        inline explicit Vector4(EYUnitVector) { m_vec = CreateYUnitVector(); }
        inline explicit Vector4(EZUnitVector) { m_vec = CreateZUnitVector(); }
        inline explicit Vector4(EWUnitVector) { m_vec = CreateWUnitVector(); }
        
        inline operator DirectX::XMVECTOR() const { return m_vec; }
        
        inline Scalar GetX() const { return Scalar(DirectX::XMVectorSplatX(m_vec)); }
        inline Scalar GetY() const { return Scalar(DirectX::XMVectorSplatY(m_vec)); }
        inline Scalar GetZ() const { return Scalar(DirectX::XMVectorSplatZ(m_vec)); }
        inline Scalar GetW() const { return Scalar(DirectX::XMVectorSplatW(m_vec)); }
        inline void SetX(Scalar x) { m_vec = DirectX::XMVectorPermute<4, 1, 2, 3>(m_vec, x); }
        inline void SetY(Scalar y) { m_vec = DirectX::XMVectorPermute<0, 5, 2, 3>(m_vec, y); }
        inline void SetZ(Scalar z) { m_vec = DirectX::XMVectorPermute<0, 1, 6, 3>(m_vec, z); }
        inline void SetW(Scalar w) { m_vec = DirectX::XMVectorPermute<0, 1, 2, 7>(m_vec, w); }
        inline void SetXYZ(Vector3 xyz) { m_vec = DirectX::XMVectorPermute<0, 1, 2, 7>(xyz, m_vec); }
        
        inline Vector4 operator- () const { return Vector4(DirectX::XMVectorNegate(m_vec)); }
        inline Vector4 operator+ (Vector4 v2) const { return Vector4(DirectX::XMVectorAdd(m_vec, v2)); }
        inline Vector4 operator- (Vector4 v2) const { return Vector4(DirectX::XMVectorSubtract(m_vec, v2)); }
        inline Vector4 operator* (Vector4 v2) const { return Vector4(DirectX::XMVectorMultiply(m_vec, v2)); }
        inline Vector4 operator/ (Vector4 v2) const { return Vector4(DirectX::XMVectorDivide(m_vec, v2)); }
        inline Vector4 operator* (Scalar  v2) const { return *this * Vector4(v2); }
        inline Vector4 operator/ (Scalar  v2) const { return *this / Vector4(v2); }
        inline Vector4 operator* (float   v2) const { return *this * Scalar(v2); }
        inline Vector4 operator/ (float   v2) const { return *this / Scalar(v2); }
        
        inline void operator*= (float   v2) { *this = *this * Scalar(v2); }
        inline void operator/= (float   v2) { *this = *this / Scalar(v2); }
        
        inline friend Vector4 operator* (Scalar  v1, Vector4 v2) { return Vector4(v1) * v2; }
        inline friend Vector4 operator/ (Scalar  v1, Vector4 v2) { return Vector4(v1) / v2; }
        inline friend Vector4 operator* (float   v1, Vector4 v2) { return Scalar(v1) * v2; }
        inline friend Vector4 operator/ (float   v1, Vector4 v2) { return Scalar(v1) / v2; }

    protected:
        DirectX::XMVECTOR m_vec;
    };

    // Defined after Vector4 methods are declared
    inline Vector3::Vector3(Vector4 vec) : m_vec((DirectX::XMVECTOR)vec)
    {
    }

    // For W != 1, divide XYZ by W.  If W == 0, do nothing
    inline Vector3 MakeHomogeneous(Vector4 v)
    {
        Scalar W = v.GetW();
        return Vector3(DirectX::XMVectorSelect(DirectX::XMVectorDivide(v, W), v, DirectX::XMVectorEqual(W, SplatZero())));
    }

    class BoolVector
    {
    public:
        inline BoolVector(DirectX::FXMVECTOR vec) { m_vec = vec; }
        inline operator DirectX::XMVECTOR() const { return m_vec; }
    protected:
        DirectX::XMVECTOR m_vec;
    };

    inline Vector3 Cross(Vector3 v1, Vector3 v2) { return Vector3(DirectX::XMVector3Cross(v1, v2)); }
    inline Vector3 Normalize(Vector3 v) { return Vector3(DirectX::XMVector3Normalize(v)); }
    inline Vector4 Normalize(Vector4 v) { return Vector4(DirectX::XMVector4Normalize(v)); }
}