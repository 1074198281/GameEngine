#pragma once

#include "XM_Header.h"
#include "XM_Scalar.h"
#include "XM_Vector.h"
#include "XM_Quaternion.h"
#include "XM_Functions.h"

namespace XM_Math {
	__declspec(align(16)) class Matrix3
	{
    public:
        inline Matrix3() {}
        inline Matrix3(Vector3 x, Vector3 y, Vector3 z) { m_mat[0] = x; m_mat[1] = y; m_mat[2] = z; }
        inline Matrix3(const Matrix3& m) { m_mat[0] = m.m_mat[0]; m_mat[1] = m.m_mat[1]; m_mat[2] = m.m_mat[2]; }
        inline Matrix3(Quaternion q) { *this = Matrix3(DirectX::XMMatrixRotationQuaternion(q)); }
        inline explicit Matrix3(const DirectX::XMMATRIX& m) { m_mat[0] = Vector3(m.r[0]); m_mat[1] = Vector3(m.r[1]); m_mat[2] = Vector3(m.r[2]); }
        inline explicit Matrix3(EIdentityTag) { m_mat[0] = Vector3(kXUnitVector); m_mat[1] = Vector3(kYUnitVector); m_mat[2] = Vector3(kZUnitVector); }
        inline explicit Matrix3(EZeroTag) { m_mat[0] = m_mat[1] = m_mat[2] = Vector3(kZero); }
        
        inline void SetX(Vector3 x) { m_mat[0] = x; }
        inline void SetY(Vector3 y) { m_mat[1] = y; }
        inline void SetZ(Vector3 z) { m_mat[2] = z; }
        
        inline Vector3 GetX() const { return m_mat[0]; }
        inline Vector3 GetY() const { return m_mat[1]; }
        inline Vector3 GetZ() const { return m_mat[2]; }

        static inline Matrix3 MakeXRotation(float angle) { return Matrix3(DirectX::XMMatrixRotationX(angle)); }
        static inline Matrix3 MakeYRotation(float angle) { return Matrix3(DirectX::XMMatrixRotationY(angle)); }
        static inline Matrix3 MakeZRotation(float angle) { return Matrix3(DirectX::XMMatrixRotationZ(angle)); }
        static inline Matrix3 MakeScale(float scale) { return Matrix3(DirectX::XMMatrixScaling(scale, scale, scale)); }
        static inline Matrix3 MakeScale(float sx, float sy, float sz) { return Matrix3(DirectX::XMMatrixScaling(sx, sy, sz)); }
        static inline Matrix3 MakeScale(const DirectX::XMFLOAT3& scale) { return Matrix3(DirectX::XMMatrixScaling(scale.x, scale.y, scale.z)); }
        static inline Matrix3 MakeScale(Vector3 scale) { return Matrix3(DirectX::XMMatrixScalingFromVector(scale)); }

        // Useful for DirectXMath interaction.  WARNING:  Only the 3x3 elements are defined.
        inline operator DirectX::XMMATRIX() const { return DirectX::XMMATRIX(m_mat[0], m_mat[1], m_mat[2], DirectX::XMVectorZero()); }

        inline Matrix3 operator* (Scalar scl) const { return Matrix3(scl * GetX(), scl * GetY(), scl * GetZ()); }
        inline Vector3 operator* (Vector3 vec) const { return Vector3(DirectX::XMVector3TransformNormal(vec, *this)); }
        inline Matrix3 operator* (const Matrix3& mat) const { return Matrix3(*this * mat.GetX(), *this * mat.GetY(), *this * mat.GetZ()); }

    private:
        Vector3 m_mat[3];
	};

    // inline Matrix3 Inverse( const Matrix3& mat ) { TBD }
    // inline Transform Inverse( const Transform& mat ) { TBD }
    inline Matrix3 Transpose(const Matrix3& mat) { return Matrix3(DirectX::XMMatrixTranspose(mat)); }
    inline Matrix3 InverseTranspose(const Matrix3& mat)
    {
        const Vector3 x = mat.GetX();
        const Vector3 y = mat.GetY();
        const Vector3 z = mat.GetZ();

        const Vector3 inv0 = Cross(y, z);
        const Vector3 inv1 = Cross(z, x);
        const Vector3 inv2 = Cross(x, y);
        const Scalar  rDet = Recip(Dot(z, inv2));

        // Return the adjoint / determinant
        return Matrix3(inv0, inv1, inv2) * rDet;
    }
}