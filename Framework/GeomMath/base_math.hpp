#pragma once

#include <assert.h>
#include <math.h>

namespace MyMath {
    template<typename _T, int row, int col>
    class Matrix
    {
    private:
        enum {
            channel = row * col,
            shortdim = row > col ? col : row
        };

    public:
        _T _data[row * col];

        //! default constructor
        Matrix();

        explicit Matrix(_T v0); //!< 1x1 matrix
        Matrix(_T v0, _T v1); //!< 1x2 or 2x1 matrix
        Matrix(_T v0, _T v1, _T v2); //!< 1x3 or 3x1 matrix
        Matrix(_T v0, _T v1, _T v2, _T v3); //!< 1x4, 2x2 or 4x1 matrix
        Matrix(_T v0, _T v1, _T v2, _T v3, _T v4); //!< 1x5 or 5x1 matrix
        Matrix(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5); //!< 1x6, 2x3, 3x2 or 6x1 matrix
        Matrix(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5, _T v6); //!< 1x7 or 7x1 matrix
        Matrix(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5, _T v6, _T v7); //!< 1x8, 2x4, 4x2 or 8x1 matrix
        Matrix(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5, _T v6, _T v7, _T v8); //!< 1x9, 3x3 or 9x1 matrix
        Matrix(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5, _T v6, _T v7, _T v8, _T v9); //!< 1x10, 2x5 or 5x2 or 10x1 matrix
        Matrix(_T v0, _T v1, _T v2, _T v3,
            _T v4, _T v5, _T v6, _T v7,
            _T v8, _T v9, _T v10, _T v11); //!< 1x12, 2x6, 3x4, 4x3, 6x2 or 12x1 matrix
        Matrix(_T v0, _T v1, _T v2, _T v3,
            _T v4, _T v5, _T v6, _T v7,
            _T v8, _T v9, _T v10, _T v11,
            _T v12, _T v13); //!< 1x14, 2x7, 7x2 or 14x1 matrix
        Matrix(_T v0, _T v1, _T v2, _T v3,
            _T v4, _T v5, _T v6, _T v7,
            _T v8, _T v9, _T v10, _T v11,
            _T v12, _T v13, _T v14, _T v15); //!< 1x16, 4x4 or 16x1 matrix
        explicit Matrix(const _T* vals); //!< initialize from a plain array

        Matrix all(_T val);
        Matrix eye();
        Matrix zeros();
        Matrix ones();
        Matrix transpose();

        _T dot(Matrix<_T, row, col>& mat) const;
        double ddot(Matrix<_T, row, col>& mat) const;

        _T& operator[](int i);
        const _T& operator[](int i) const;

        _T& operator()(int m, int n);
        const _T& operator()(int m, int n) const;

        Matrix operator+(const Matrix<_T, row, col>& mat) const;
        Matrix operator-(const Matrix<_T, row, col>& mat) const;

        template<int p>
        Matrix<_T, row, p> operator*(const Matrix<_T, col, p>& mat) const;
    };

    typedef Matrix<float, 1, 1> Matrix1X1f;
    typedef Matrix<double, 1, 1> Matrix1X1d;
    typedef Matrix<float, 2, 2> Matrix2X2f;
    typedef Matrix<double, 2, 2> Matrix2X2d;
    typedef Matrix<float, 3, 3> Matrix3X3f;
    typedef Matrix<double, 3, 3> Matrix3X3d;
    typedef Matrix<float, 4, 4> Matrix4X4f;
    typedef Matrix<double, 4, 4> Matrix4X4d;
    typedef Matrix<float, 6, 6> Matrix6X6f;
    typedef Matrix<double, 6, 6> Matrix6X6d;
    typedef Matrix<float, 8, 8> Matrix8X8f;
    typedef Matrix<double, 8, 8> Matrix8X8d;

    template<typename _T, int row>
    class Vector : public Matrix<_T, row, 1> {
    private:
        enum {
            channel = row
        };
    public:
        _T _data[row];

        explicit Vector();

        Vector(_T v0);
        Vector(_T v0, _T v1);
        Vector(_T v0, _T v1, _T v2);
        Vector(_T v0, _T v1, _T v2, _T v3);
        Vector(_T v0, _T v1, _T v2, _T v3, _T v4);
        Vector(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5);
        Vector(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5, _T v6);
        Vector(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5, _T v6, _T v7);
        Vector(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5, _T v6, _T v7, _T v8);
        Vector(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5, _T v6, _T v7, _T v8, _T v9);
        explicit Vector(const _T* vals);

        Vector all(_T val);
        void zeros();
        void ones();

        Vector cross(const Vector& vec);
        _T dot(const Vector& vec);
        Vector normalize();

        _T& operator[](int val);
        const _T& operator[](int val) const;

        Vector operator+(const Vector& vec);
        Vector operator-(const Vector& vec);

        Vector operator+(const _T& val);
        Vector operator-(const _T& val);
        Vector operator*(const _T& val);
        Vector operator/(const _T& val);
    };


    
    typedef Vector<float, 2> Vector2f;
    typedef Vector<double, 2> Vector2d;
    typedef Vector<float, 3> Vector3f;
    typedef Vector<double, 3> Vector3d;
    typedef Vector<uint8_t, 4> R8G8B8A8Unorm;
    typedef Vector<uint8_t, 4> Vector4i;
    typedef Vector<float, 4> Vector4f;
    typedef Vector<double, 4> Vector4d;
    typedef Vector<float, 6> Vector6f;
    typedef Vector<double, 6> Vector6d;
    typedef Vector<float, 8> Vector8f;
    typedef Vector<double, 8> Vector8d;


    template<typename _T, int row, int col> inline
        Matrix<_T, row, col>::Matrix()
    {
        for (int i = 0; i < channel; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col>::Matrix(_T v0)
    {
        _data[0] = v0;
        for (int i = 1; i < channel; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col>::Matrix(_T v0, _T v1)
    {
        _data[0] = v0;
        _data[1] = v1;
        for (int i = 2; i < channel; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col>::Matrix(_T v0, _T v1, _T v2)
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        for (int i = 3; i < channel; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col>::Matrix(_T v0, _T v1, _T v2, _T v3)
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        for (int i = 4; i < channel; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col>::Matrix(_T v0, _T v1, _T v2, _T v3, _T v4)
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        _data[4] = v4;
        for (int i = 5; i < channel; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col>::Matrix(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5)
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        _data[4] = v4;
        _data[5] = v5;
        for (int i = 6; i < channel; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col>::Matrix(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5, _T v6)
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        _data[4] = v4;
        _data[5] = v5;
        _data[6] = v6;
        for (int i = 7; i < channel; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col>::Matrix(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5, _T v6, _T v7)
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        _data[4] = v4;
        _data[5] = v5;
        _data[6] = v6;
        _data[7] = v7;
        for (int i = 8; i < channel; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col>::Matrix(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5, _T v6, _T v7, _T v8)
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        _data[4] = v4;
        _data[5] = v5;
        _data[6] = v6;
        _data[7] = v7;
        _data[8] = v8;
        for (int i = 9; i < channel; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col>::Matrix(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5, _T v6, _T v7, _T v8, _T v9)
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        _data[4] = v4;
        _data[5] = v5;
        _data[6] = v6;
        _data[7] = v7;
        _data[8] = v8;
        _data[9] = v9;
        for (int i = 10; i < channel; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col>::Matrix(
            _T v0, _T v1, _T v2, _T v3,
            _T v4, _T v5, _T v6, _T v7,
            _T v8, _T v9, _T v10, _T v11
        )
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        _data[4] = v4;
        _data[5] = v5;
        _data[6] = v6;
        _data[7] = v7;
        _data[8] = v8;
        _data[9] = v9;
        _data[10] = v10;
        _data[11] = v11;
        for (int i = 12; i < channel; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col>::Matrix(
            _T v0, _T v1, _T v2, _T v3,
            _T v4, _T v5, _T v6, _T v7,
            _T v8, _T v9, _T v10, _T v11,
            _T v12, _T v13
        )
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        _data[4] = v4;
        _data[5] = v5;
        _data[6] = v6;
        _data[7] = v7;
        _data[8] = v8;
        _data[9] = v9;
        _data[10] = v10;
        _data[11] = v11;
        _data[12] = v12;
        _data[13] = v13;
        for (int i = 14; i < channel; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col>::Matrix(
            _T v0, _T v1, _T v2, _T v3,
            _T v4, _T v5, _T v6, _T v7,
            _T v8, _T v9, _T v10, _T v11,
            _T v12, _T v13, _T v14, _T v15
        )
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        _data[4] = v4;
        _data[5] = v5;
        _data[6] = v6;
        _data[7] = v7;
        _data[8] = v8;
        _data[9] = v9;
        _data[10] = v10;
        _data[11] = v11;
        _data[12] = v12;
        _data[13] = v13;
        _data[14] = v14;
        _data[15] = v15;
        for (int i = 16; i < channel; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col>::Matrix(const _T* vals)
    {
        for (int i = 0; i < channel; i++) {
            _data[i] = vals[i];
        }
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col> Matrix<_T, row, col>::all(_T val)
    {
        Matrix<_T, row, col> mat;
        for (int i = 0; i < channel; i++) {
            mat._data[i] = val;
        }
        return mat;
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col> Matrix<_T, row, col>::eye()
    {
        Matrix<_T, row, col> mat;
        for (int i = 0; i < shortdim; i++) {
            mat._data[i * col + i] = _T(1);
        }
        return mat;
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col> Matrix<_T, row, col>::zeros()
    {
        Matrix<_T, row, col> mat;
        for (int i = 0; i < channel; i++) {
            mat._data[i] = _T(0);
        }
        return mat;
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col> Matrix<_T, row, col>::ones()
    {
        Matrix<_T, row, col> mat;
        for (int i = 0; i < channel; i++) {
            mat._data[i] = _T(1);
        }
        return mat;
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col> Matrix<_T, row, col>::transpose()
    {
        Matrix<_T, col, row> mat;
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                mat(i, j) = _data[j * row + i];
            }
        }
        return mat;
    }

    template<typename _T, int row, int col> inline
        _T Matrix<_T, row, col>::dot(Matrix<_T, row, col>& mat) const
    {
        _T v(0);
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                v += _data[i * col + j] * mat(i, j);
            }
        }
        return v;
    }

    template<typename _T, int row, int col> inline
        double Matrix<_T, row, col>::ddot(Matrix<_T, row, col>& mat) const
    {
        double v(0);
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                v += _data[i * col + j] * mat(i, j);
            }
        }
        return v;
    }

    template<typename _T, int row, int col> inline
        _T& Matrix<_T, row, col>::operator[](int i)
    {
        return _data[i];
    }

    template<typename _T, int row, int col> inline
        const _T& Matrix<_T, row, col>::operator[](int i) const
    {
        return _data[i];
    }

    template<typename _T, int row, int col> inline
        _T& Matrix<_T, row, col>::operator()(int m, int n)
    {
        assert(m < row);
        assert(n < col);
        return _data[m * col + n];
    }

    template<typename _T, int row, int col> inline
        const _T& Matrix<_T, row, col>::operator()(int m, int n) const
    {
        assert(m < row);
        assert(n < col);
        return _data[m * col + n];
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col> Matrix<_T, row, col>::operator+(const Matrix<_T, row, col>& mat) const
    {
        Matrix<_T, row, col> result;
        for (int i = 0; i < channel; i++) {
            result[i] = _data[i] + mat._data[i];
        }
        return result;
    }

    template<typename _T, int row, int col> inline
        Matrix<_T, row, col> Matrix<_T, row, col>::operator-(const Matrix<_T, row, col>& mat) const
    {
        Matrix<_T, row, col> result;
        for (int i = 0; i < channel; i++) {
            result[i] = _data[i] - mat._data[i];
        }
        return result;
    }

    template<typename _T, int row, int col>
    template<int p> inline
        Matrix<_T, row, p> Matrix<_T, row, col>::operator*(const Matrix<_T, col, p>& mat) const
    {
        Matrix<_T, row, col> result;
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < p; j++) {
                _T s = 0;
                for (int k = 0; k < col; k++) {
                    s += _data[i * col + k] * mat._data[k * p + j];
                }
                result._data[i * p + j] = s;
            }
        }
        return result;
    }

    template<typename _T, int row>
    Vector<_T, row>::Vector()
    {
        for (int i = 0; i < row; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row>
    Vector<_T, row>::Vector(_T v0)
    {
        _data[0] = v0;
        for (int i = 1; i < row; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row>
    Vector<_T, row>::Vector(_T v0, _T v1)
    {
        _data[0] = v0;
        _data[1] = v1;
        for (int i = 2; i < row; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row>
    Vector<_T, row>::Vector(_T v0, _T v1, _T v2)
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        for (int i = 3; i < row; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row>
    Vector<_T, row>::Vector(_T v0, _T v1, _T v2, _T v3)
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        for (int i = 4; i < row; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row>
    Vector<_T, row>::Vector(_T v0, _T v1, _T v2, _T v3, _T v4)
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        _data[4] = v4;
        for (int i = 5; i < row; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row>
    Vector<_T, row>::Vector(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5)
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        _data[4] = v4;
        _data[5] = v5;
        for (int i = 6; i < row; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row>
    Vector<_T, row>::Vector(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5, _T v6)
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        _data[4] = v4;
        _data[5] = v5;
        _data[6] = v6;
        for (int i = 7; i < row; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row>
    Vector<_T, row>::Vector(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5, _T v6, _T v7)
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        _data[4] = v4;
        _data[5] = v5;
        _data[6] = v6;
        _data[7] = v7;
        for (int i = 8; i < row; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row>
    Vector<_T, row>::Vector(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5, _T v6, _T v7, _T v8)
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        _data[4] = v4;
        _data[5] = v5;
        _data[6] = v6;
        _data[7] = v7;
        _data[8] = v8;
        for (int i = 9; i < row; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row>
    Vector<_T, row>::Vector(_T v0, _T v1, _T v2, _T v3, _T v4, _T v5, _T v6, _T v7, _T v8, _T v9)
    {
        _data[0] = v0;
        _data[1] = v1;
        _data[2] = v2;
        _data[3] = v3;
        _data[4] = v4;
        _data[5] = v5;
        _data[6] = v6;
        _data[7] = v7;
        _data[8] = v8;
        _data[9] = v9;
        for (int i = 10; i < row; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row>
    Vector<_T, row>::Vector(const _T* vals)
    {
        for (int i = 0; i < row; i++) {
            _data[i] = vals[i];
        }
    }

    template<typename _T, int row> inline
    Vector<_T, row> Vector<_T, row>::all(_T val)
    {
        for (int i = 0; i < row; i++) {
            _data[i] = val;
        }
    }

    template<typename _T, int row> inline
    void Vector<_T, row>::zeros()
    {
        for (int i = 0; i < row; i++) {
            _data[i] = _T(0);
        }
    }

    template<typename _T, int row> inline
    void Vector<_T, row>::ones()
    {
        for (int i = 0; i < row; i++) {
            _data[i] = _T(1);
        }
    }

    template<typename _T, int row> inline
    Vector<_T, row> Vector<_T, row>::cross(const Vector<_T, row>& vec)
    {
        assert(false);
    }

    template<> inline
    Vector<float, 3> Vector<float, 3>::cross(const Vector<float, 3>& vec)
    {
        Vector<float, 3> result;
        result._data[0] = _data[1] * vec._data[2] - _data[2] * vec._data[1];
        result._data[1] = _data[2] * vec._data[0] - _data[0] * vec._data[2];
        result._data[2] = _data[0] * vec._data[1] - _data[1] * vec._data[0];
        return result;
    }

    template<> inline
    Vector<double, 3> Vector<double, 3>::cross(const Vector<double, 3>& vec)
    {
        Vector<double, 3> result;
        result._data[0] = _data[1] * vec._data[2] - _data[2] * vec._data[1];
        result._data[1] = _data[2] * vec._data[0] - _data[0] * vec._data[2];
        result._data[2] = _data[0] * vec._data[1] - _data[1] * vec._data[0];
        return result;
    }

    template<> inline
    Vector<float, 4> Vector<float, 4>::cross(const Vector<float, 4>& vec)
    {
        Vector<float, 4> result;
        result._data[0] = _data[1] * vec._data[2] - _data[2] * vec._data[1];
        result._data[1] = _data[2] * vec._data[0] - _data[0] * vec._data[2];
        result._data[2] = _data[0] * vec._data[1] - _data[1] * vec._data[0];
        result._data[3] = 1.0f;
        return result;
    }

    template<> inline
    Vector<double, 4> Vector<double, 4>::cross(const Vector<double, 4>& vec)
    {
        Vector<double, 4> result;
        result._data[0] = _data[1] * vec._data[2] - _data[2] * vec._data[1];
        result._data[1] = _data[2] * vec._data[0] - _data[0] * vec._data[2];
        result._data[2] = _data[0] * vec._data[1] - _data[1] * vec._data[0];
        result._data[3] = 1.0;
        return result;
    }

    template<typename _T, int row> inline
    _T Vector<_T, row>::dot(const Vector<_T, row>& vec)
    {
        _T result = _T(0);
        for (int i = 0; i < row; i++) {
            result += _data[i] * vec._data[i];
        }
        return result;
    }

    template<typename _T, int row> inline
    Vector<_T, row> Vector<_T, row>::normalize()
    {
        _T m = _T(0);
        Vector<_T, row> result;
        for (int i = 0; i < row; i++) {
            m += _data[i] * _data[i];
        }
        m = 1.0 / sqrt(m);
        for (int i = 0; i < row; i++) {
            result = _data[i] * m;
        }
        return result;
    }
    template<typename _T, int row> inline
    _T& Vector<_T, row>::operator[](int val)
    {
        assert(val < row);
        return _data[val];
    }

    template<typename _T, int row> inline
    const _T& Vector<_T, row>::operator[](int val) const
    {
        assert(val < row);
        return _data[val];
    }

    template<typename _T, int row> inline
    Vector<_T, row> Vector<_T, row>::operator+(const Vector<_T, row>& vec)
    {
        Vector<_T, row> result;
        for (int i = 0; i < row; i++) {
            result._data[i] = _data[i] + vec._data[i];
        }
        return result;
    }

    template<typename _T, int row> inline
    Vector<_T, row> Vector<_T, row>::operator-(const Vector<_T, row>& vec)
    {
        Vector<_T, row> result;
        for (int i = 0; i < row; i++) {
            result._data[i] = _data[i] - vec._data[i];
        }
        return result;
    }

    template<typename _T, int row> inline
    Vector<_T, row> Vector<_T, row>::operator+(const _T& val)
    {
        Vector<_T, row> result;
        for (int i = 0; i < row; i++) {
            result._data[i] = _data[i] + val;
        }
        return result;
    }

    template<typename _T, int row> inline
    Vector<_T, row> Vector<_T, row>::operator-(const _T& val)
    {
        Vector<_T, row> result;
        for (int i = 0; i < row; i++) {
            result._data[i] = _data[i] - val;
        }
        return result;
    }

    template<typename _T, int row> inline
    Vector<_T, row> Vector<_T, row>::operator*(const _T& val)
    {
        Vector<_T, row> result;
        for (int i = 0; i < row; i++) {
            result._data[i] = _data[i] * val;
        }
        return result;
    }

    template<typename _T, int row> inline
    Vector<_T, row> Vector<_T, row>::operator/(const _T& val)
    {
        Vector<_T, row> result;
        for (int i = 0; i < row; i++) {
            result._data[i] = _data[i] / val;
        }
        return result;
    }

    template<typename _T> inline
    Vector<_T, 4> Multiple(Matrix<_T, 4, 4> mat, Vector<_T, 4> vec)
    {
        Vector<_T, 4> result;
        for (int i = 0; i < 4; i++) {
            result._data[i] = mat(i, 0) * vec._data[0]
                + mat(i, 1) * vec._data[1]
                + mat(i, 2) * vec._data[2]
                + mat(i, 3) * vec._data[3];
        }
        return result;
    }

    template<typename _T> inline
    static Vector<_T, 4> Multiple(Matrix<_T, 4, 4> mat, Vector<_T, 3> vec)
    {
        Vector<_T, 4> result;
        for (int i = 0; i < 4; i++) {
            result._data[i] = mat(i, 0) * vec._data[0]
                + mat(i, 1) * vec._data[1]
                + mat(i, 2) * vec._data[2]
                + mat(i, 3) * _T(1);
        }
        return result;
    }
}
