#pragma once

#include "XM_Header.h"
#include "XM_Vector.h"

namespace XM_Math {
    class Quaternion
    {
    public:
        inline Quaternion() { m_vec = DirectX::XMQuaternionIdentity(); }
        inline Quaternion(const Vector3& axis, const Scalar& angle) { m_vec = DirectX::XMQuaternionRotationAxis(axis, angle); }
        inline Quaternion(float pitch, float yaw, float roll) { m_vec = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll); }
        inline explicit Quaternion(const DirectX::XMMATRIX& matrix) { m_vec = DirectX::XMQuaternionRotationMatrix(matrix); }
        inline explicit Quaternion(DirectX::FXMVECTOR vec) { m_vec = vec; }
        inline explicit Quaternion(EIdentityTag) { m_vec = DirectX::XMQuaternionIdentity(); }

        inline operator DirectX::XMVECTOR() const { return m_vec; }

        inline Quaternion operator~ (void) const { return Quaternion(DirectX::XMQuaternionConjugate(m_vec)); }
        inline Quaternion operator- (void) const { return Quaternion(DirectX::XMVectorNegate(m_vec)); }

        inline Quaternion operator* (Quaternion rhs) const { return Quaternion(DirectX::XMQuaternionMultiply(rhs, m_vec)); }
        inline Vector3 operator* (Vector3 rhs) const { return Vector3(DirectX::XMVector3Rotate(rhs, m_vec)); }

        inline Quaternion& operator= (Quaternion rhs) { m_vec = rhs; return *this; }
        inline Quaternion& operator*= (Quaternion rhs) { *this = *this * rhs; return *this; }

    protected:
        DirectX::XMVECTOR m_vec;
    };

    inline Quaternion Normalize(Quaternion q) { return Quaternion(DirectX::XMQuaternionNormalize(q)); }
    inline Quaternion Slerp(Quaternion a, Quaternion b, float t) { return Normalize(Quaternion(DirectX::XMQuaternionSlerp(a, b, t))); }
    inline Quaternion Lerp(Quaternion a, Quaternion b, float t) { return Normalize(Quaternion(DirectX::XMVectorLerp(a, b, t))); }
}