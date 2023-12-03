#pragma once

#include "XM_Header.h"
#include "XM_Scalar.h"
#include "XM_Matrix3.h"
#include "XM_Quaternion.h"


namespace XM_Math {
    // Orthonormal basis (just rotation via quaternion) and translation
    class OrthogonalTransform;

    // A 3x4 matrix that allows for asymmetric skew and scale
    class AffineTransform;

    // Uniform scale and translation that can be compactly represented in a vec4
    class ScaleAndTranslation;

    // Uniform scale, rotation (quaternion), and translation that fits in two vec4s
    class UniformTransform;

    // This transform strictly prohibits non-uniform scale.  Scale itself is barely tolerated.
    class OrthogonalTransform
    {
    public:
        inline OrthogonalTransform() : m_rotation(kIdentity), m_translation(kZero) {}
        inline OrthogonalTransform(Quaternion rotate) : m_rotation(rotate), m_translation(kZero) {}
        inline OrthogonalTransform(Vector3 translate) : m_rotation(kIdentity), m_translation(translate) {}
        inline OrthogonalTransform(Quaternion rotate, Vector3 translate) : m_rotation(rotate), m_translation(translate) {}
        inline OrthogonalTransform(const Matrix3& mat) : m_rotation(mat), m_translation(kZero) {}
        inline OrthogonalTransform(const Matrix3& mat, Vector3 translate) : m_rotation(mat), m_translation(translate) {}
        inline OrthogonalTransform(EIdentityTag) : m_rotation(kIdentity), m_translation(kZero) {}
        inline explicit OrthogonalTransform(const DirectX::XMMATRIX& mat) { *this = OrthogonalTransform(Matrix3(mat), Vector3(mat.r[3])); }

        inline void SetRotation(Quaternion q) { m_rotation = q; }
        inline void SetTranslation(Vector3 v) { m_translation = v; }

        inline Quaternion GetRotation() const { return m_rotation; }
        inline Vector3 GetTranslation() const { return m_translation; }

        static inline OrthogonalTransform MakeXRotation(float angle) { return OrthogonalTransform(Quaternion(Vector3(kXUnitVector), angle)); }
        static inline OrthogonalTransform MakeYRotation(float angle) { return OrthogonalTransform(Quaternion(Vector3(kYUnitVector), angle)); }
        static inline OrthogonalTransform MakeZRotation(float angle) { return OrthogonalTransform(Quaternion(Vector3(kZUnitVector), angle)); }
        static inline OrthogonalTransform MakeTranslation(Vector3 translate) { return OrthogonalTransform(translate); }

        inline Vector3 operator* (Vector3 vec) const { return m_rotation * vec + m_translation; }
        inline Vector4 operator* (Vector4 vec) const {
            return Vector4(SetWToZero(m_rotation * Vector3((DirectX::XMVECTOR)vec))) + Vector4(SetWToOne(m_translation)) * vec.GetW();
        }
        //inline BoundingSphere operator* (const BoundingSphere& sphere) const {
        //    return BoundingSphere(*this * sphere.GetCenter(), sphere.GetRadius());
        //}

        inline OrthogonalTransform operator* (const OrthogonalTransform& xform) const {
            return OrthogonalTransform(m_rotation * xform.m_rotation, m_rotation * xform.m_translation + m_translation);
        }

        inline OrthogonalTransform operator~ () const {
            Quaternion invertedRotation = ~m_rotation;
            return OrthogonalTransform(invertedRotation, invertedRotation * -m_translation);
        }

    private:

        Quaternion m_rotation;
        Vector3 m_translation;
    };

    //
    // A transform that lacks rotation and has only uniform scale.
    //
    class ScaleAndTranslation
    {
    public:
        inline ScaleAndTranslation()
        {}
        inline ScaleAndTranslation(EIdentityTag)
            : m_repr(kWUnitVector) {}
        inline ScaleAndTranslation(float tx, float ty, float tz, float scale)
            : m_repr(tx, ty, tz, scale) {}
        inline ScaleAndTranslation(Vector3 translate, Scalar scale)
        {
            m_repr = Vector4(translate);
            m_repr.SetW(scale);
        }
        inline explicit ScaleAndTranslation(const DirectX::XMVECTOR& v)
            : m_repr(v) {}

        inline void SetScale(Scalar s) { m_repr.SetW(s); }
        inline void SetTranslation(Vector3 t) { m_repr.SetXYZ(t); }

        inline Scalar GetScale() const { return m_repr.GetW(); }
        inline Vector3 GetTranslation() const { return (Vector3)m_repr; }

        //inline BoundingSphere operator*(const BoundingSphere& sphere) const
        //{
        //    Vector4 scaledSphere = (Vector4)sphere * GetScale();
        //    Vector4 translation = Vector4(SetWToZero(m_repr));
        //    return BoundingSphere(scaledSphere + translation);
        //}

    private:
        Vector4 m_repr;
    };

    //
    // This transform allows for rotation, translation, and uniform scale
    // 
    class UniformTransform
    {
    public:
        inline UniformTransform()
        {}
        inline UniformTransform(EIdentityTag)
            : m_rotation(kIdentity), m_translationScale(kIdentity) {}
        inline UniformTransform(Quaternion rotation, ScaleAndTranslation transScale)
            : m_rotation(rotation), m_translationScale(transScale)
        {}
        inline UniformTransform(Quaternion rotation, Scalar scale, Vector3 translation)
            : m_rotation(rotation), m_translationScale(translation, scale)
        {}

        inline void SetRotation(Quaternion r) { m_rotation = r; }
        inline void SetScale(Scalar s) { m_translationScale.SetScale(s); }
        inline void SetTranslation(Vector3 t) { m_translationScale.SetTranslation(t); }


        inline Quaternion GetRotation() const { return m_rotation; }
        inline Scalar GetScale() const { return m_translationScale.GetScale(); }
        inline Vector3 GetTranslation() const { return m_translationScale.GetTranslation(); }


        inline Vector3 operator*(Vector3 vec) const
        {
            return m_rotation * (vec * m_translationScale.GetScale()) + m_translationScale.GetTranslation();
        }

        //inline BoundingSphere operator*(const BoundingSphere& sphere) const
        //{
        //    return BoundingSphere(*this * sphere.GetCenter(), GetScale() * sphere.GetRadius());
        //}

    private:
        Quaternion m_rotation;
        ScaleAndTranslation m_translationScale;
    };

    // A AffineTransform is a 3x4 matrix with an implicit 4th row = [0,0,0,1].  This is used to perform a change of
    // basis on 3D points.  An affine transformation does not have to have orthonormal basis vectors.
    class AffineTransform
    {
    public:
        inline AffineTransform()
        {}
        inline AffineTransform(Vector3 x, Vector3 y, Vector3 z, Vector3 w)
            : m_basis(x, y, z), m_translation(w) {}
        inline AffineTransform(Vector3 translate)
            : m_basis(kIdentity), m_translation(translate) {}
        inline AffineTransform(const Matrix3& mat, Vector3 translate = Vector3(kZero))
            : m_basis(mat), m_translation(translate) {}
        inline AffineTransform(Quaternion rot, Vector3 translate = Vector3(kZero))
            : m_basis(rot), m_translation(translate) {}
        inline AffineTransform(const OrthogonalTransform& xform)
            : m_basis(xform.GetRotation()), m_translation(xform.GetTranslation()) {}
        inline AffineTransform(const UniformTransform& xform)
        {
            m_basis = Matrix3(xform.GetRotation()) * xform.GetScale();
            m_translation = xform.GetTranslation();
        }
        inline AffineTransform(EIdentityTag)
            : m_basis(kIdentity), m_translation(kZero) {}
        inline explicit AffineTransform(const DirectX::XMMATRIX& mat)
            : m_basis(mat), m_translation(mat.r[3]) {}

        inline operator DirectX::XMMATRIX() const { return (DirectX::XMMATRIX&)*this; }

        inline void SetX(Vector3 x) { m_basis.SetX(x); }
        inline void SetY(Vector3 y) { m_basis.SetY(y); }
        inline void SetZ(Vector3 z) { m_basis.SetZ(z); }
        inline void SetTranslation(Vector3 w) { m_translation = w; }
        inline void SetBasis(const Matrix3& basis) { m_basis = basis; }

        inline Vector3 GetX() const { return m_basis.GetX(); }
        inline Vector3 GetY() const { return m_basis.GetY(); }
        inline Vector3 GetZ() const { return m_basis.GetZ(); }
        inline Vector3 GetTranslation() const { return m_translation; }
        inline const Matrix3& GetBasis() const { return (const Matrix3&)*this; }

        static inline AffineTransform MakeXRotation(float angle) { return AffineTransform(Matrix3::MakeXRotation(angle)); }
        static inline AffineTransform MakeYRotation(float angle) { return AffineTransform(Matrix3::MakeYRotation(angle)); }
        static inline AffineTransform MakeZRotation(float angle) { return AffineTransform(Matrix3::MakeZRotation(angle)); }
        static inline AffineTransform MakeScale(float scale) { return AffineTransform(Matrix3::MakeScale(scale)); }
        static inline AffineTransform MakeScale(Vector3 scale) { return AffineTransform(Matrix3::MakeScale(scale)); }
        static inline AffineTransform MakeTranslation(Vector3 translate) { return AffineTransform(translate); }

        inline Vector3 operator* (Vector3 vec) const { return m_basis * vec + m_translation; }
        inline AffineTransform operator* (const AffineTransform& mat) const {
            return AffineTransform(m_basis * mat.m_basis, *this * mat.GetTranslation());
        }

    private:
        Matrix3 m_basis;
        Vector3 m_translation;
    };

    // This specialized matrix invert assumes that the 3x3 matrix is orthogonal (and normalized).
    inline AffineTransform OrthoInvert(const AffineTransform& xform)
    {
        Matrix3 basis = Transpose(xform.GetBasis());
        return AffineTransform(basis, basis * -xform.GetTranslation());
    }

    inline OrthogonalTransform Invert(const OrthogonalTransform& xform) { return ~xform; }
}