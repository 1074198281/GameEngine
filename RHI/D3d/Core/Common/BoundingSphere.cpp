//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
// Developed by Minigraph
//
// Author:  James Stanard 
//

#include "BoundingSphere.h"
#include "Functions.inl"

using namespace Math;

//=======================================================================================================
// Inline implementations
//

BoundingSphere::BoundingSphere(float x, float y, float z, float r) : m_repr(x, y, z, r)
{

}

BoundingSphere::BoundingSphere(const XMFLOAT4* unaligned_array) : m_repr(*unaligned_array)
{

}

BoundingSphere::BoundingSphere(Vector3 center, Scalar radius)
{
    m_repr = Vector4(center);
    m_repr.SetW(radius);
}

BoundingSphere::BoundingSphere(EZeroTag) : m_repr(kZero)
{

}

BoundingSphere::BoundingSphere(const XMVECTOR& v) : m_repr(v)
{

}

BoundingSphere::BoundingSphere(const XMFLOAT4& f4) : m_repr(f4)
{

}

BoundingSphere::BoundingSphere(Vector4 sphere) : m_repr(sphere)
{

}

BoundingSphere::operator Vector4() const
{ 
    return Vector4(m_repr); 
}

Vector3 BoundingSphere::GetCenter(void) const
{ 
    return Vector3(m_repr);
}

Scalar BoundingSphere::GetRadius(void) const
{ 
    return m_repr.GetW(); 
}

BoundingSphere BoundingSphere::Union(const BoundingSphere& rhs)
{
    float radA = GetRadius();
    if (radA == 0.0f)
        return rhs;

    float radB = rhs.GetRadius();
    if (radB == 0.0f)
        return *this;

    Vector3 diff = GetCenter() - rhs.GetCenter();
    float dist = Length(diff);

    // Safe normalize vector between sphere centers
    diff = dist < 1e-6f ? Vector3(kXUnitVector) : diff * Recip(dist);

    Vector3 extremeA = GetCenter() + diff * Max(radA, radB - dist);
    Vector3 extremeB = rhs.GetCenter() - diff * Max(radB, radA - dist);

    return BoundingSphere((extremeA + extremeB) * 0.5f, Length(extremeA - extremeB) * 0.5f);
}
