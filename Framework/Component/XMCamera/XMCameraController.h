#pragma once

#include "XM_Matrix3.h"
#include "XM_Matrix4.h"
#include "XM_Vector.h"

namespace XM_Camera {
    using XM_Math::Vector3;
    using XM_Math::Vector4;
    using XM_Math::Matrix3;
    using XM_Math::Matrix4;
    using XM_Math::kIdentity;
    using XM_Math::Quaternion;
    using XM_Math::AffineTransform;
    using XM_Math::OrthogonalTransform;

    class Camera;

    class CameraController
    {
    public:
        // Assumes worldUp is not the X basis vector
        CameraController(Camera& camera) : m_TargetCamera(camera) {}
        virtual ~CameraController() {}
        virtual void Update(float dt) = 0;

        // Helper function
        static void ApplyMomentum(float& oldValue, float& newValue, float deltaTime);

    protected:
        Camera& m_TargetCamera;

    private:
        CameraController& operator=(const CameraController&) { return *this; }
    };

    class FlyingFPSCamera : public CameraController
    {
    public:
        FlyingFPSCamera(Camera& camera, Vector3 worldUp);

        virtual void Update(float dt) override;

        void SlowMovement(bool enable) { m_FineMovement = enable; }
        void SlowRotation(bool enable) { m_FineRotation = enable; }

        void EnableMomentum(bool enable) { m_Momentum = enable; }

        void SetHeadingPitchAndPosition(float heading, float pitch, const Vector3& position);

    private:

        Vector3 m_WorldUp;
        Vector3 m_WorldNorth;
        Vector3 m_WorldEast;
        float m_HorizontalLookSensitivity;
        float m_VerticalLookSensitivity;
        float m_MoveSpeed;
        float m_StrafeSpeed;
        float m_MouseSensitivityX;
        float m_MouseSensitivityY;

        float m_CurrentHeading;
        float m_CurrentPitch;

        bool m_FineMovement;
        bool m_FineRotation;
        bool m_Momentum;

        float m_LastYaw;
        float m_LastPitch;
        float m_LastForward;
        float m_LastStrafe;
        float m_LastAscent;
    };

    class OrbitCamera : public CameraController
    {
    public:
        OrbitCamera(XM_Camera::Camera& camera,
            XM_Math::Vector3 upVec = XM_Math::Vector3(XM_Math::kYUnitVector));

        virtual void Update(float dt) override;

        void EnableMomentum(bool enable) { m_Momentum = enable; }

    private:
        OrbitCamera& operator=(const OrbitCamera&) { return *this; }

        XM_Math::Vector3 m_WorldUp;

        float m_JoystickSensitivityX;
        float m_JoystickSensitivityY;

        float m_MouseSensitivityX;
        float m_MouseSensitivityY;

        float m_CurrentHeading;
        float m_CurrentPitch;
        float m_CurrentCloseness;

        bool m_Momentum;

        float m_LastYaw;
        float m_LastPitch;
        float m_LastForward;
    };

}