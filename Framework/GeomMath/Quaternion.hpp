#pragma once

#include <math.h>
#include <algorithm>
#include "geommath.hpp"

namespace My {
	inline float QuaternionLengthSquare(const Quaternion& q)
	{
		return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	}

	inline float QuaternionLength(const Quaternion& q)
	{
		return sqrt(QuaternionLengthSquare(q));
	}

	inline Quaternion QuaternionConjugate(const Quaternion& q)
	{
		Quaternion result;
		result.x = -q.x;
		result.y = -q.y;
		result.z = -q.z;
		result.w = q.w;
		return result;
	}

	inline Quaternion QuaternionInverse(const Quaternion& q)
	{
		return QuaternionConjugate(q) / QuaternionLengthSquare(q);
	}

	inline Quaternion QuaternionMultiply(const Quaternion& q, const Quaternion& p)
	{
		Quaternion result;
		Vector3f v1{ q.x, q.y, q.z };
		Vector3f v2{ p.x, p.y, p.z };
		float s1{ q.w };
		float s2{ p.w };
		result.w = s1 * s2 - DotProduct(v1, v2);
		Vector3f v = v2 * s1 + v1 * s2 + CrossProduct(v1, v2);
		result.x = v.x;
		result.y = v.y;
		result.z = v.z;
		return result;
	}

	inline Vector4f QuaternionRotate(const Vector4f& position, const Vector3f& rotate_axis, const float& angle)
	{
		Quaternion q{ rotate_axis.x, rotate_axis.y, rotate_axis.z, angle / 2 };
		Quaternion pos{ position };
		return QuaternionMultiply(QuaternionMultiply(q, pos), QuaternionInverse(q));
	}


}