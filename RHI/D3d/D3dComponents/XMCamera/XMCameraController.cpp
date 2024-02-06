#include "XMCameraController.h"
#include "XM_Functions.h"
#include "XM_Scalar.h"
#include "XMCamera.h"
#include "../XMInput/XMInput.h"

using XM_Math::Vector3;
using XM_Math::Vector4;
using XM_Math::Matrix3;
using XM_Math::Matrix4;
using XM_Math::Scalar;
using XM_Math::EIdentityTag;
using XM_Math::kXUnitVector;
using XM_Math::kYUnitVector;
using XM_Math::kZUnitVector;
using XM_Math::Quaternion;
using XM_Math::AffineTransform;
using XM_Math::OrthogonalTransform;

XM_Camera::FlyingFPSCamera::FlyingFPSCamera(Camera& camera, Vector3 worldUp) : CameraController(camera)
{
    m_WorldUp = XM_Math::Normalize(worldUp);
    m_WorldNorth = XM_Math::Normalize(Cross(m_WorldUp, Vector3(kXUnitVector)));
    m_WorldEast = XM_Math::Cross(m_WorldNorth, m_WorldUp);

    m_HorizontalLookSensitivity = 2.0f;
    m_VerticalLookSensitivity = 2.0f;
    m_MoveSpeed = 100.0f;
    m_StrafeSpeed = 100.0f;
    m_MouseSensitivityX = 1.0f;
    m_MouseSensitivityY = 1.0f;

    m_CurrentPitch = XM_Math::Sin(XM_Math::Dot(camera.GetForwardVec(), m_WorldUp));

    Vector3 forward = XM_Math::Normalize(XM_Math::Cross(m_WorldUp, camera.GetRightVec()));
    m_CurrentHeading = XM_Math::ATan2(-XM_Math::Dot(forward, m_WorldEast), XM_Math::Dot(forward, m_WorldNorth));

    m_FineMovement = false;
    m_FineRotation = false;
    m_Momentum = true;

    m_LastYaw = 0.0f;
    m_LastPitch = 0.0f;
    m_LastForward = 0.0f;
    m_LastStrafe = 0.0f;
    m_LastAscent = 0.0f;
}

void XM_Camera::FlyingFPSCamera::Update(float deltaTime)
{
    (deltaTime);

    //float timeScale = Graphics::DebugZoom == 0 ? 1.0f : Graphics::DebugZoom == 1 ? 0.5f : 0.25f;
    float timeScale = 1.0f;

    if (XM_Input::IsFirstPressed(XM_Input::kLThumbClick) || XM_Input::IsFirstPressed(XM_Input::kKey_lshift))
        m_FineMovement = !m_FineMovement;

    if (XM_Input::IsFirstPressed(XM_Input::kRThumbClick))
        m_FineRotation = !m_FineRotation;

    float speedScale = (m_FineMovement ? 0.1f : 1.0f) * timeScale;
    float panScale = (m_FineRotation ? 0.5f : 1.0f) * timeScale;

    float yaw, pitch, forward, strafe, ascent;
    yaw = pitch = forward = strafe = ascent = 0.0f;
    yaw = XM_Input::GetTimeCorrectedAnalogInput(XM_Input::kAnalogRightStickX) * m_HorizontalLookSensitivity * panScale;
    pitch = XM_Input::GetTimeCorrectedAnalogInput(XM_Input::kAnalogRightStickY) * m_VerticalLookSensitivity * panScale;
    forward = m_MoveSpeed * speedScale * (
        XM_Input::GetTimeCorrectedAnalogInput(XM_Input::kAnalogLeftStickY) +
        (XM_Input::IsPressed(XM_Input::kKey_w) ? deltaTime : 0.0f) +
        (XM_Input::IsPressed(XM_Input::kKey_s) ? -deltaTime : 0.0f)
        );
    strafe = m_StrafeSpeed * speedScale * (
        XM_Input::GetTimeCorrectedAnalogInput(XM_Input::kAnalogLeftStickX) +
        (XM_Input::IsPressed(XM_Input::kKey_d) ? deltaTime : 0.0f) +
        (XM_Input::IsPressed(XM_Input::kKey_a) ? -deltaTime : 0.0f)
        );
    ascent = m_StrafeSpeed * speedScale * (
        XM_Input::GetTimeCorrectedAnalogInput(XM_Input::kAnalogRightTrigger) -
        XM_Input::GetTimeCorrectedAnalogInput(XM_Input::kAnalogLeftTrigger) +
        (XM_Input::IsPressed(XM_Input::kKey_e) ? deltaTime : 0.0f) +
        (XM_Input::IsPressed(XM_Input::kKey_q) ? -deltaTime : 0.0f)
        );

    if (m_Momentum)
    {
        ApplyMomentum(m_LastYaw, yaw, deltaTime);
        ApplyMomentum(m_LastPitch, pitch, deltaTime);
        ApplyMomentum(m_LastForward, forward, deltaTime);
        ApplyMomentum(m_LastStrafe, strafe, deltaTime);
        ApplyMomentum(m_LastAscent, ascent, deltaTime);
    }

    // don't apply momentum to mouse inputs
    if (XM_Input::IsPressed(XM_Input::kMouse0)) {
        yaw += XM_Input::GetAnalogInput(XM_Input::kAnalogMouseX) * m_MouseSensitivityX;
        pitch += XM_Input::GetAnalogInput(XM_Input::kAnalogMouseY) * m_MouseSensitivityY;
    }


    m_CurrentPitch += pitch;
    m_CurrentPitch = DirectX::XMMin(DirectX::XM_PIDIV2, m_CurrentPitch);
    m_CurrentPitch = DirectX::XMMax(-DirectX::XM_PIDIV2, m_CurrentPitch);

    m_CurrentHeading -= yaw;
    if (m_CurrentHeading > DirectX::XM_PI)
        m_CurrentHeading -= DirectX::XM_2PI;
    else if (m_CurrentHeading <= -DirectX::XM_PI)
        m_CurrentHeading += DirectX::XM_2PI;

    Matrix3 orientation = Matrix3(m_WorldEast, m_WorldUp, -m_WorldNorth) * Matrix3::MakeYRotation(m_CurrentHeading) * Matrix3::MakeXRotation(m_CurrentPitch);
    Vector3 position = orientation * Vector3(strafe, ascent, -forward) + m_TargetCamera.GetPosition();
    Vector3 CamPos = m_TargetCamera.GetPosition();
    if (fabs(position.GetX() - CamPos.GetX()) < FLT_EPSILON && 
        fabs(position.GetY() - CamPos.GetY()) < FLT_EPSILON &&
        fabs(position.GetZ() - CamPos.GetZ()) < FLT_EPSILON) {
        m_TargetCamera.m_bIsCameraMove = false;
    }
    else {
        m_TargetCamera.m_bIsCameraMove = true;
    }
    m_TargetCamera.SetTransform(AffineTransform(orientation, position));
    m_TargetCamera.Update();
}

void XM_Camera::FlyingFPSCamera::SetHeadingPitchAndPosition(float heading, float pitch, const Vector3& position)
{
    m_CurrentHeading = heading;
    if (m_CurrentHeading > DirectX::XM_PI)
        m_CurrentHeading -= DirectX::XM_2PI;
    else if (m_CurrentHeading <= -DirectX::XM_PI)
        m_CurrentHeading += DirectX::XM_2PI;

    m_CurrentPitch = pitch;
    m_CurrentPitch = DirectX::XMMin(DirectX::XM_PIDIV2, m_CurrentPitch);
    m_CurrentPitch = DirectX::XMMax(-DirectX::XM_PIDIV2, m_CurrentPitch);

    Matrix3 orientation =
        Matrix3(m_WorldEast, m_WorldUp, -m_WorldNorth) *
        Matrix3::MakeYRotation(m_CurrentHeading) *
        Matrix3::MakeXRotation(m_CurrentPitch);

    m_TargetCamera.SetTransform(AffineTransform(orientation, position));
    m_TargetCamera.Update();
}


void XM_Camera::CameraController::ApplyMomentum(float& oldValue, float& newValue, float deltaTime)
{
    float blendedValue;
    if (XM_Math::Abs(newValue) > XM_Math::Abs(oldValue))
        blendedValue = XM_Math::Lerp(newValue, oldValue, XM_Math::Pow(0.6f, deltaTime * 60.0f));
    else
        blendedValue = XM_Math::Lerp(newValue, oldValue, XM_Math::Pow(0.8f, deltaTime * 60.0f));
    oldValue = blendedValue;
    newValue = blendedValue;
}

XM_Camera::OrbitCamera::OrbitCamera(Camera& camera, Vector3 worldUp) : CameraController(camera)
{
    m_WorldUp = XM_Math::Normalize(worldUp);

    m_JoystickSensitivityX = 2.0f;
    m_JoystickSensitivityY = 2.0f;

    m_MouseSensitivityX = 1.0f;
    m_MouseSensitivityY = 1.0f;

    m_CurrentPitch = 0.0f;
    m_CurrentHeading = 0.0f;
    m_CurrentCloseness = 0.5f;

    m_Momentum = true;

    m_LastYaw = 0.0f;
    m_LastPitch = 0.0f;
    m_LastForward = 0.0f;
}

void XM_Camera::OrbitCamera::Update(float deltaTime)
{
    (deltaTime);

    //float timeScale = Graphics::DebugZoom == 0 ? 1.0f : Graphics::DebugZoom == 1 ? 0.5f : 0.25f;
    float timeScale = 1.0f;

    float yaw, pitch, closeness;
    yaw = pitch = closeness = 0.0f;
    //摇杆输入
    yaw = XM_Input::GetTimeCorrectedAnalogInput(XM_Input::kAnalogLeftStickX) * timeScale * m_JoystickSensitivityX;
    pitch = XM_Input::GetTimeCorrectedAnalogInput(XM_Input::kAnalogLeftStickY) * timeScale * m_JoystickSensitivityY;
    closeness = XM_Input::GetTimeCorrectedAnalogInput(XM_Input::kAnalogRightStickY) * timeScale;

    // 应用上一帧动量来平滑当前相机的运动
    if (m_Momentum)
    {
        ApplyMomentum(m_LastYaw, yaw, deltaTime);
        ApplyMomentum(m_LastPitch, pitch, deltaTime);
    }

    // don't apply momentum to mouse inputs
    yaw += XM_Input::GetAnalogInput(XM_Input::kAnalogMouseX) * m_MouseSensitivityX;
    pitch += XM_Input::GetAnalogInput(XM_Input::kAnalogMouseY) * m_MouseSensitivityY;
    closeness += XM_Input::GetAnalogInput(XM_Input::kAnalogMouseScroll) * 0.1f;

    m_CurrentPitch += pitch;
    m_CurrentPitch = DirectX::XMMin(DirectX::XM_PIDIV2, m_CurrentPitch);
    m_CurrentPitch = DirectX::XMMax(-DirectX::XM_PIDIV2, m_CurrentPitch);

    m_CurrentHeading -= yaw;
    if (m_CurrentHeading > DirectX::XM_PI)
        m_CurrentHeading -= DirectX::XM_2PI;
    else if (m_CurrentHeading <= -DirectX::XM_PI)
        m_CurrentHeading += DirectX::XM_2PI;

    m_CurrentCloseness += closeness;
    m_CurrentCloseness = XM_Math::Clamp(m_CurrentCloseness, 0.0f, 1.0f);

    Matrix3 orientation = Matrix3::MakeYRotation(m_CurrentHeading) * Matrix3::MakeXRotation(m_CurrentPitch);
    //Vector3 position = orientation.GetZ() * (m_ModelBounds.GetRadius() * Lerp(3.0f, 1.0f, m_CurrentCloseness) + m_TargetCamera.GetNearClip());
    //m_TargetCamera.SetTransform(AffineTransform(orientation, position + m_ModelBounds.GetCenter()));
    Vector3 position = orientation.GetZ() * ((Scalar(1.0f) * XM_Math::Lerp(3.0f, 1.0f, m_CurrentCloseness)) + m_TargetCamera.GetNearClip());
    m_TargetCamera.SetTransform(AffineTransform(orientation, position));
    m_TargetCamera.Update();
}
