#pragma once

#include "XM_Header.h"
#include "XM_Transform.h"

namespace XM_Math {
    __declspec(align(16)) class Matrix4
    {
    public:
        inline Matrix4() {}
        inline Matrix4(Vector3 x, Vector3 y, Vector3 z, Vector3 w)
        {
            m_mat.r[0] = SetWToZero(x); m_mat.r[1] = SetWToZero(y);
            m_mat.r[2] = SetWToZero(z); m_mat.r[3] = SetWToOne(w);
        }

        inline Matrix4(const float* data)
        {
            m_mat = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)data);
        }

        inline Matrix4(Vector4 x, Vector4 y, Vector4 z, Vector4 w) { m_mat.r[0] = x; m_mat.r[1] = y; m_mat.r[2] = z; m_mat.r[3] = w; }
        inline Matrix4(const Matrix4& mat) { m_mat = mat.m_mat; }
        inline Matrix4(const Matrix3& mat)
        {
            m_mat.r[0] = SetWToZero(mat.GetX());
            m_mat.r[1] = SetWToZero(mat.GetY());
            m_mat.r[2] = SetWToZero(mat.GetZ());
            m_mat.r[3] = CreateWUnitVector();
        }
        inline Matrix4(const Matrix3& xyz, Vector3 w)
        {
            m_mat.r[0] = SetWToZero(xyz.GetX());
            m_mat.r[1] = SetWToZero(xyz.GetY());
            m_mat.r[2] = SetWToZero(xyz.GetZ());
            m_mat.r[3] = SetWToOne(w);
        }
        inline Matrix4(const AffineTransform& xform) { *this = Matrix4(xform.GetBasis(), xform.GetTranslation()); }
        inline Matrix4(const OrthogonalTransform& xform) { *this = Matrix4(Matrix3(xform.GetRotation()), xform.GetTranslation()); }
        inline explicit Matrix4(const DirectX::XMMATRIX& mat) { m_mat = mat; }
        inline explicit Matrix4(EIdentityTag) { m_mat = DirectX::XMMatrixIdentity(); }
        inline explicit Matrix4(EZeroTag) { m_mat.r[0] = m_mat.r[1] = m_mat.r[2] = m_mat.r[3] = SplatZero(); }

        inline const Matrix3& Get3x3() const { return (const Matrix3&)*this; }
        inline void Set3x3(const Matrix3& xyz)
        {
            m_mat.r[0] = SetWToZero(xyz.GetX());
            m_mat.r[1] = SetWToZero(xyz.GetY());
            m_mat.r[2] = SetWToZero(xyz.GetZ());
        }

        inline Vector4 GetX() const { return Vector4(m_mat.r[0]); }
        inline Vector4 GetY() const { return Vector4(m_mat.r[1]); }
        inline Vector4 GetZ() const { return Vector4(m_mat.r[2]); }
        inline Vector4 GetW() const { return Vector4(m_mat.r[3]); }

        inline void SetX(Vector4 x) { m_mat.r[0] = x; }
        inline void SetY(Vector4 y) { m_mat.r[1] = y; }
        inline void SetZ(Vector4 z) { m_mat.r[2] = z; }
        inline void SetW(Vector4 w) { m_mat.r[3] = w; }

        inline operator DirectX::XMMATRIX() const { return m_mat; }
        inline DirectX::XMMATRIX GetMatrix() const { return m_mat; }

        inline Vector4 operator* (Vector3 vec) const { return Vector4(DirectX::XMVector3Transform(vec, m_mat)); }
        inline Vector4 operator* (Vector4 vec) const { return Vector4(DirectX::XMVector4Transform(vec, m_mat)); }
        inline Matrix4 operator* (const Matrix4& mat) const { return Matrix4(DirectX::XMMatrixMultiply(mat, m_mat)); }

        static inline Matrix4 MakeScale(float scale) { return Matrix4(DirectX::XMMatrixScaling(scale, scale, scale)); }
        static inline Matrix4 MakeScale(Vector3 scale) { return Matrix4(DirectX::XMMatrixScalingFromVector(scale)); }

    private:
        DirectX::XMMATRIX m_mat;
    };

    inline Matrix4 Transpose(const Matrix4& mat) { return Matrix4(DirectX::XMMatrixTranspose(mat)); }
    inline Matrix4 Invert(const Matrix4& mat) { return Matrix4(DirectX::XMMatrixInverse(nullptr, mat)); }

    inline Matrix4 OrthoInvert(const Matrix4& xform)
    {
        Matrix3 basis = Transpose(xform.Get3x3());
        Vector3 translate = basis * -Vector3(xform.GetW());
        return Matrix4(basis, translate);
    }
}

