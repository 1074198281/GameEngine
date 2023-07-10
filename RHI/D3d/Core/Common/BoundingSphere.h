#pragma once

#include "../Common/VectorMath.h"

namespace Math
{
    class BoundingSphere
    {
    public:
        BoundingSphere() {}
        BoundingSphere(float x, float y, float z, float r);
        BoundingSphere(const XMFLOAT4* unaligned_array);
        BoundingSphere(Vector3 center, Scalar radius);
        BoundingSphere(EZeroTag);
        explicit BoundingSphere(const XMVECTOR& v);
        explicit BoundingSphere(const XMFLOAT4& f4);
        explicit BoundingSphere(Vector4 sphere);
        explicit operator Vector4() const;

        Vector3 GetCenter(void) const;
        Scalar GetRadius(void) const;

        BoundingSphere Union(const BoundingSphere& rhs);

    private:

        Vector4 m_repr;
    };

} // namespace Math
