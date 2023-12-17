#include <cmath>
#include <corecrt_math.h>
#include "XMCamera.h"

using XM_Math::Vector3;
using XM_Math::Vector4;
using XM_Math::Matrix3;
using XM_Math::Matrix4;
using XM_Math::Scalar;
using XM_Math::EIdentityTag;
using XM_Math::kZUnitVector;
using XM_Math::kYUnitVector;
using XM_Math::Quaternion;
using XM_Math::AffineTransform;
using XM_Math::OrthogonalTransform;

void XM_Camera::BaseCamera::SetLookDirection(Vector3 forward, Vector3 up)
{
    // Given, but ensure normalization
    Scalar forwardLenSq = LengthSquare(forward);
    forward = Select(forward * RecipSqrt(forwardLenSq), -Vector3(kZUnitVector), forwardLenSq < Scalar(0.000001f));

    // Deduce a valid, orthogonal right vector
    Vector3 right = Cross(forward, up);
    Scalar rightLenSq = LengthSquare(right);
    right = Select(right * RecipSqrt(rightLenSq), Quaternion(Vector3(kYUnitVector), -DirectX::XM_PIDIV2) * forward, rightLenSq < Scalar(0.000001f));

    // Compute actual up vector
    up = Cross(right, forward);

    // Finish constructing basis
    m_Basis = Matrix3(right, up, -forward);
    m_CameraToWorld.SetRotation(Quaternion(m_Basis));
}

void XM_Camera::BaseCamera::Update()
{
    m_PreviousViewProjMatrix = m_ViewProjMatrix;

    m_ViewMatrix = Matrix4(~m_CameraToWorld);
    m_ViewProjMatrix = m_ProjMatrix * m_ViewMatrix;
    m_ReprojectMatrix = m_PreviousViewProjMatrix * Invert(GetViewProjMatrix());

    //m_FrustumVS = Frustum(m_ProjMatrix);
    //m_FrustumWS = m_CameraToWorld * m_FrustumVS;
}

void XM_Camera::Camera::UpdateProjMatrix(void)
{
    float Y = 1.0f / std::tanf(m_VerticalFOV * 0.5f);
    float X = Y * m_AspectRatio;

    float Q1, Q2;

    // ReverseZ puts far plane at Z=0 and near plane at Z=1.  This is never a bad idea, and it's
    // actually a great idea with F32 depth buffers to redistribute precision more evenly across
    // the entire range.  It requires clearing Z to 0.0f and using a GREATER variant depth test.
    // Some care must also be done to properly reconstruct linear W in a pixel shader from hyperbolic Z.
    if (m_ReverseZ)
    {
        if (m_InfiniteZ)
        {
            Q1 = 0.0f;
            Q2 = m_NearClip;
        }
        else
        {
            Q1 = m_NearClip / (m_FarClip - m_NearClip);
            Q2 = Q1 * m_FarClip;
        }
    }
    else
    {
        if (m_InfiniteZ)
        {
            Q1 = -1.0f;
            Q2 = -m_NearClip;
        }
        else
        {
            Q1 = m_FarClip / (m_NearClip - m_FarClip);
            Q2 = Q1 * m_NearClip;
        }
    }

    SetProjMatrix(Matrix4(
        Vector4(X, 0.0f, 0.0f, 0.0f),
        Vector4(0.0f, Y, 0.0f, 0.0f),
        Vector4(0.0f, 0.0f, Q1, -1.0f),
        Vector4(0.0f, 0.0f, Q2, 0.0f)
    ));
}