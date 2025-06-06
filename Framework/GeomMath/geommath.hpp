#pragma once
#include <cstdint>
#include <iostream>
#include <limits>
#include <math.h>
#include <assert.h>

//#define SSE2
#ifdef SSE2
#include <intrin.h>
#endif // SSE2
//#include "include/CrossProduct.h"
//#include "include/DotProduct.h"
//#include "include/MulByElement.h"
//#include "include/Normalize.h"
//#include "include/Transform.h"
//#include "include/Transpose.h"
//#include "include/AddByElement.h"
//#include "include/SubByElement.h"

#ifndef PI
#define PI 3.14159265358979323846f
#endif

#ifndef TWO_PI
#define TWO_PI 3.14159265358979323846f * 2.0f
#endif

#ifndef EP
#define EP  1e-7
#endif // !EP

using std::ostream;

namespace My {
    const int MATRIX4_SIZE = 4;

    template<typename T, size_t SizeOfArray>
        constexpr size_t countof(T (&)[SizeOfArray]) { return SizeOfArray; }

    template<typename T, size_t RowSize, size_t ColSize>
        constexpr size_t countof(T (&)[RowSize][ColSize]) { return RowSize * ColSize; }

    template<typename T>
        constexpr float normalize(T value) {
            return value < 0
                ? -static_cast<float>(value) / std::numeric_limits<T>::min()
                :  static_cast<float>(value) / std::numeric_limits<T>::max()
                ;
        }

    template <template<typename> class TT, typename T, int ... Indexes>
	class swizzle {
		T v[sizeof...(Indexes)];

	public:
		
		TT<T>& operator=(const TT<T>& rhs)
		{
            int indexes[] = { Indexes... };
            for (int i = 0; i < sizeof...(Indexes); i++) {
			    v[indexes[i]] = rhs[i];
            }
			return *(TT<T>*)this;
		}
	
		operator TT<T>() const
		{
			return TT<T>( v[Indexes]... );
		}
		
	};

    template <typename T>
    struct Vector2Type
    {
        union {
            T data[2];
            struct { T x, y; };
            struct { T r, g; };
            struct { T u, v; };
		    swizzle<My::Vector2Type, T, 0, 1> xy;
		    swizzle<My::Vector2Type, T, 1, 0> yx;
        };

        Vector2Type<T>() {};
        Vector2Type<T>(const T& _v) : x(_v), y(_v) {};
        Vector2Type<T>(const T& _x, const T& _y) : x(_x), y(_y) {};

        operator T*() { return data; };
        operator const T*() const { return static_cast<const T*>(data); };
    };
    
    typedef Vector2Type<float> Vector2f;

    template <typename T>
    struct Vector3Type
    {
        union {
            T data[3];
            struct { T x, y, z; };
            struct { T r, g, b; };
		    swizzle<My::Vector2Type, T, 0, 1> xy;
		    swizzle<My::Vector2Type, T, 1, 0> yx;
		    swizzle<My::Vector2Type, T, 0, 2> xz;
		    swizzle<My::Vector2Type, T, 2, 0> zx;
		    swizzle<My::Vector2Type, T, 1, 2> yz;
		    swizzle<My::Vector2Type, T, 2, 1> zy;
		    swizzle<My::Vector3Type, T, 0, 1, 2> xyz;
		    swizzle<My::Vector3Type, T, 1, 0, 2> yxz;
		    swizzle<My::Vector3Type, T, 0, 2, 1> xzy;
		    swizzle<My::Vector3Type, T, 2, 0, 1> zxy;
		    swizzle<My::Vector3Type, T, 1, 2, 0> yzx;
		    swizzle<My::Vector3Type, T, 2, 1, 0> zyx;
        };

        Vector3Type<T>() {};
        Vector3Type<T>(const T& _v) : x(_v), y(_v), z(_v) {};
        Vector3Type<T>(const T& _x, const T& _y, const T& _z) : x(_x), y(_y), z(_z) {};

        operator T*() { return data; };
        operator const T*() const { return static_cast<const T*>(data); };
    };

    typedef Vector3Type<float> Vector3f;

    template <typename T>
    struct Vector4Type
    {
        union {
            T data[4];
            struct { T x, y, z, w; };
            struct { T r, g, b, a; };
		    swizzle<My::Vector3Type, T, 0, 2, 1> xzy;
		    swizzle<My::Vector3Type, T, 1, 0, 2> yxz;
		    swizzle<My::Vector3Type, T, 1, 2, 0> yzx;
		    swizzle<My::Vector3Type, T, 2, 0, 1> zxy;
		    swizzle<My::Vector3Type, T, 2, 1, 0> zyx;
		    swizzle<My::Vector4Type, T, 2, 1, 0, 3> bgra;
        };

        Vector4Type<T>() { data[0] = 0; data[1] = 0; data[2] = 0; data[3] = 0; };
        Vector4Type<T>(const T& _v) : x(_v), y(_v), z(_v), w(_v) {};
        Vector4Type<T>(const T& _x, const T& _y, const T& _z, const T& _w) : x(_x), y(_y), z(_z), w(_w) {};
        Vector4Type<T>(const Vector3Type<T>& v3) : x(v3.x), y(v3.y), z(v3.z), w(1.0f) {};
        Vector4Type<T>(const Vector3Type<T>& v3, const T& _w) : x(v3.x), y(v3.y), z(v3.z), w(_w) {};

        operator T*() { return data; };
        operator const T*() const { return static_cast<const T*>(data); };

        Vector4Type& operator=(const T* f)
        {
            memcpy(data, f, sizeof(T) * 4);
            return *this;
        };
    };

    typedef Vector4Type<float> Vector4f;
    typedef Vector4Type<float> Quaternion;
    typedef Vector4Type<uint8_t> R8G8B8A8Unorm;
    typedef Vector4Type<uint8_t> Vector4i;

    template <template <typename> class TT, typename T>
    ostream& operator<<(ostream& out, TT<T> vector)
    {
        out << "( ";
        for (uint32_t i = 0; i < countof(vector.data); i++) {
                out << vector.data[i] << ((i == countof(vector.data) - 1)? ' ' : ',');
        }
        out << ")\n";

        return out;
    }

    template <template<typename> class TT, typename T>
    void VectorAdd(TT<T>& result, const TT<T>& vec1, const TT<T>& vec2)
    {
        //ispc::AddByElement(vec1, vec2, result, countof(result.data));
        for(int count = 0; count < countof(result.data); count++) {
            result.data[count] = vec1.data[count] + vec2.data[count];
        }
    }

    template <template<typename> class TT, typename T>
    TT<T> operator+(const TT<T>& vec1, const TT<T>& vec2)
    {
        TT<T> result;
        VectorAdd(result, vec1, vec2);

        return result;
    }

    template <template<typename> class TT, typename T>
    void VectorSub(TT<T>& result, const TT<T>& vec1, const TT<T>& vec2)
    {
        //ispc::SubByElement(vec1, vec2, result, countof(result.data));
        for(int count = 0; count < countof(result.data); count++) {
            result.data[count] = vec1.data[count] - vec2.data[count];
        }
    }

    template <template<typename> class TT, typename T>
    TT<T> operator-(const TT<T>& vec1, const TT<T>& vec2)
    {
        TT<T> result;
        VectorSub(result, vec1, vec2);

        return result;
    }

    template <template<typename> class TT, typename T>
    void VectorMultiply(TT<T>& result, const TT<T>& vec1, const T& val)
    {
        for (int count = 0; count < countof(result.data); count++) {
            result.data[count] = vec1.data[count] * val;
        }
    }

    template <template<typename> class TT, typename T>
    TT<T> operator*(const TT<T>& vec1, const T& val)
    {
        TT<T> result;
        VectorMultiply(result, vec1, val);

        return result;
    }

    template <template<typename> class TT, typename T>
    void VectorDevide(TT<T>& result, const TT<T>& vec1, T val)
    {
        assert(val != 0);
        for (int count = 0; count < countof(result.data); count++) {
            result.data[count] = vec1.data[count] / val;
        }
    }

    template <template<typename> class TT, typename T>
    TT<T> operator/(const TT<T>& vec1, const T val)
    {
        TT<T> result;
        VectorDevide(result, vec1, val);

        return result;
    }

    inline void CrossProduct(Vector3f& result, const Vector3f& vec1, const Vector3f& vec2)
    {
        result.data[0] = vec1.data[1] * vec2.data[2] - vec1.data[2] * vec2.data[1];
        result.data[1] = vec1.data[2] * vec2.data[0] - vec1.data[0] * vec2.data[2];
        result.data[2] = vec1.data[0] * vec2.data[1] - vec1.data[1] * vec2.data[0];
    }

    inline Vector3f CrossProduct(const Vector3f& vec1, const Vector3f& vec2)
    {
        Vector3f result;
        result.data[0] = vec1.data[1] * vec2.data[2] - vec1.data[2] * vec2.data[1];
        result.data[1] = vec1.data[2] * vec2.data[0] - vec1.data[0] * vec2.data[2];
        result.data[2] = vec1.data[0] * vec2.data[1] - vec1.data[1] * vec2.data[0];
        return result;
    }

    inline void CrossProduct(Vector4f& result, const Vector4f& vec1, const Vector4f& vec2)
    {
        assert(vec1.data[3] == vec2.data[3]);
        result.data[0] = vec1.data[1] * vec2.data[2] - vec1.data[2] * vec2.data[1];
        result.data[1] = vec1.data[2] * vec2.data[0] - vec1.data[0] * vec2.data[2];
        result.data[2] = vec1.data[0] * vec2.data[1] - vec1.data[1] * vec2.data[0];
        result.data[3] = vec1.data[3];
    }

    inline Vector4f CrossProduct(const Vector4f& vec1, const Vector4f& vec2)
    {
        Vector4f result;
        assert(vec1.data[3] == vec2.data[3]);
        result.data[0] = vec1.data[1] * vec2.data[2] - vec1.data[2] * vec2.data[1];
        result.data[1] = vec1.data[2] * vec2.data[0] - vec1.data[0] * vec2.data[2];
        result.data[2] = vec1.data[0] * vec2.data[1] - vec1.data[1] * vec2.data[0];
        result.data[3] = vec1.data[3];
        return result;
    }

    inline void DotProduct(float& result, const Vector3f& vec1, const Vector3f& vec2)
    {
        result = vec1.data[0] * vec2.data[0] + vec1.data[1] * vec2.data[1] + vec1.data[2] * vec2.data[2];
    }

    inline float DotProduct(const Vector3f& vec1, const Vector3f& vec2)
    {
        return vec1.data[0] * vec2.data[0] + vec1.data[1] * vec2.data[1] + vec1.data[2] * vec2.data[2];
    }

    inline void DotProduct(float& result, const Vector4f& vec1, const Vector4f& vec2)
    {
#ifdef SSE2
        __m128 _v1 = _mm_load_ps(vec1.data);
        __m128 _v2 = _mm_load_ps(vec2.data);
        __m128 _result = _mm_mul_ps(_v1, _v2);
        result = _result.m128_f32[0] + _result.m128_f32[1] + _result.m128_f32[2] + _result.m128_f32[3];
        return;
#endif // SSE2
        result = vec1.data[0] * vec2.data[0] + vec1.data[1] * vec2.data[1] + vec1.data[2] * vec2.data[2] + vec1.data[3] * vec2.data[3];
    }

    inline float DotProduct(const Vector4f& vec1, const Vector4f& vec2)
    {
        return vec1.data[0] * vec2.data[0] + vec1.data[1] * vec2.data[1] + vec1.data[2] * vec2.data[2] + vec1.data[3] * vec2.data[3];
    }

    template <template <typename> class TT, typename T>
    inline void CrossProduct(TT<T>& result, const TT<T>& vec1, const TT<T>& vec2)
    {
        //ispc::CrossProduct(vec1, vec2, result);
    }

    template <template <typename> class TT, typename T>
    inline void DotProduct(T& result, const TT<T>& vec1, const TT<T>& vec2)
    {
        //ispc::DotProduct(vec1, vec2, &result, countof(vec1.data));
        for(int count = 0; count < countof(vec1.data); count++) {
            result +=  vec1.data[count] * vec2.data[count];
        }
    }

    template <typename T>
    inline void MulByElement(T& result, const T& a, const T& b)
    {
        for(int count = 0; count < countof(result.data); count++) {
            result.data[count] = a.data[count] * b.data[count];
        }
        //ispc::MulByElement(a, b, result, countof(result.data));
    }


    // Matrix

    template <typename T, int ROWS, int COLS>
    struct Matrix
    {
        union {
            T data[ROWS][COLS];
        };

        T* operator[](int row_index) {
            return data[row_index];
        }

        const T* operator[](int row_index) const {
            return data[row_index];
        }

        operator T*() { return &data[0][0]; };
        operator const T*() const { return static_cast<const T*>(&data[0][0]); };

        Matrix& operator=(const T* _data)
        {
            memcpy(data, _data, ROWS * COLS * sizeof(T));
            return *this;
        }
    };

    typedef Matrix<float, 3, 3> Matrix3X3f;
    typedef Matrix<float, 4, 4> Matrix4X4f;
    typedef Matrix<int32_t, 8, 8> Matrix8X8i;
    typedef Matrix<float, 8, 8> Matrix8X8f;

    template <typename T, int ROWS, int COLS>
    ostream& operator<<(ostream& out, Matrix<T, ROWS, COLS> matrix)
    {
        out << std::endl;
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                out << matrix.data[i][j] << ((j == COLS - 1)? '\n' : ',');
            }
        }
        out << std::endl;

        return out;
    }


    inline void BuildIdentityMatrix(Matrix4X4f& matrix)
    {
        Matrix4X4f identity = { {{
            { 1.0f, 0.0f, 0.0f, 0.0f},
            { 0.0f, 1.0f, 0.0f, 0.0f},
            { 0.0f, 0.0f, 1.0f, 0.0f},
            { 0.0f, 0.0f, 0.0f, 1.0f}
        }} };

        matrix = identity;

        return;
    }

    template <typename T, int ROWS, int COLS>
    void MatrixAdd(Matrix<T, ROWS, COLS>& result, const Matrix<T, ROWS, COLS>& matrix1, const Matrix<T, ROWS, COLS>& matrix2)
    {
        for(int row = 0; row < ROWS; row++) {
            for(int col = 0; col < COLS; col++) {
                result.data[row][col] = matrix1.data[row][col] + matrix2.data[row][col];
            }
        }
        //ispc::AddByElement(matrix1, matrix2, result, countof(result.data));
    }

    template <typename T, int ROWS, int COLS>
    Matrix<T, ROWS, COLS> operator+(const Matrix<T, ROWS, COLS>& matrix1, const Matrix<T, ROWS, COLS>& matrix2)
    {
        Matrix<T, ROWS, COLS> result;
        MatrixAdd(result, matrix1, matrix2);

        return result;
    }

    template <typename T, int ROWS, int COLS>
    void MatrixSub(Matrix<T, ROWS, COLS>& result, const Matrix<T, ROWS, COLS>& matrix1, const Matrix<T, ROWS, COLS>& matrix2)
    {
        for(int row = 0; row < ROWS; row++) {
            for(int col = 0; col < COLS; col++) {
                result.data[row][col] = matrix1.data[row][col] - matrix2.data[row][col];
            }
        }
        //ispc::AddByElement(matrix1, matrix2, result, countof(result.data));
    }

    template <typename T, int ROWS, int COLS>
    Matrix<T, ROWS, COLS> operator-(const Matrix<T, ROWS, COLS>& matrix1, const Matrix<T, ROWS, COLS>& matrix2)
    {
        Matrix<T, ROWS, COLS> result;
        MatrixSub(result, matrix1, matrix2);

        return result;
    }

    template <typename T, int Da, int Db, int Dc>
    void MatrixMultiply(Matrix<T, Da, Dc>& result, const Matrix<T, Da, Db>& matrix1, const Matrix<T, Dc, Db>& matrix2)
    {
        // Matrix<T, Dc, Db> matrix2_transpose;
        // Transpose(matrix2_transpose, matrix2);
        // for (int i = 0; i < Da; i++) {
        //     for (int j = 0; j < Dc; j++) {
        //         ispc::DotProduct(matrix1[i], matrix2_transpose[j], &result[i][j], Db);
        //     }
        // }
        Matrix<T, Dc, Db> matrix2_transpose;
        Transpose(matrix2_transpose, matrix2);

        for (int i = 0; i < Da; i++) {
            for (int j = 0; j < Dc; j++) {
                float s = 0;
                for (int k = 0; k < Db; k++) {
                    s += matrix1.data[i][k] * matrix2.data[k][j];
                }
                result.data[i][j] = s;
            }
        }

        return;
    }

    template <typename T, int ROWS, int COLS>
    Matrix<T, ROWS, COLS> operator*(const Matrix<T, ROWS, COLS>& matrix1, const Matrix<T, ROWS, COLS>& matrix2)
    {
        Matrix<T, ROWS, COLS> result;
        MatrixMultiply(result, matrix1, matrix2);

        return result;
    }

    template <typename T, int ROWS, int COLS>
    void MatrixMulByElement(Matrix<T, ROWS, COLS>& result, const Matrix<T, ROWS, COLS>& matrix1, const Matrix<T, ROWS, COLS>& matrix2)
    {
        //ispc::MulByElement(matrix1, matrix2, result, countof(result.data));
    }

    template <int ROWS, int COLS>
    void MatrixMulByElementi32(Matrix<int32_t, ROWS, COLS>& result, const Matrix<int32_t, ROWS, COLS>& matrix1, const Matrix<int32_t, ROWS, COLS>& matrix2)
    {
        //ispc::MulByElementi32(matrix1, matrix2, result, countof(result.data));
    }

    template <template <typename, int, int> class TT, typename T, int ROWS, int COLS>
    inline void Transpose(TT<T, ROWS, COLS>& result, const TT<T, ROWS, COLS>& matrix1)
    {
        //ispc::Transpose(matrix1, result, ROWS, COLS);
        for(int i = 0; i < ROWS; i++) {
            for(int j = 0; j < COLS; j++) {
                result.data[i][j] = matrix1.data[j][i];
            }
        }
    }

    inline void MatrixMulVector(Vector4f& result, const Vector4f& vec, const Matrix4X4f& mat)
    {
#if SSE2

#endif // SSE2
        for (int c = 0; c < 4; c++) {
            for (int r = 0; r < 4; r++)
            {
                result[c] += vec[r] * mat.data[r][c];
            }
        }
    }

    template <typename T>
    inline void Normalize(T& result)
    {
        //ispc::Normalize(result, countof(result.data));
        float length = 0.0;
        for(int count = 0; count < countof(result.data); count++) {
            length += result.data[count] * result.data[count];
        }
        length = sqrt(length);
        length = 1.0f / length;
        for(int count = 0; count < countof(result.data); count++) {
            result.data[count] *= length;
        }
    }

    inline bool isNear(Vector4f vec1, Vector4f vec2, float tiny = EP)
    {
        for (int i = 0; i < 4; i++) {
            if (abs(vec1[i] - vec2[i]) > EP) {
                return false;
            }
        }
        return true;
    }

    inline void MatrixRotationYawPitchRoll(Matrix4X4f& matrix, const float yaw, const float pitch, const float roll)
    {
        float cYaw, cPitch, cRoll, sYaw, sPitch, sRoll;


        // Get the cosine and sin of the yaw, pitch, and roll.
        cYaw = cosf(yaw);
        cPitch = cosf(pitch);
        cRoll = cosf(roll);

        sYaw = sinf(yaw);
        sPitch = sinf(pitch);
        sRoll = sinf(roll);

        // Calculate the yaw, pitch, roll rotation matrix.
        Matrix4X4f tmp = {{{
            { (cRoll * cYaw) + (sRoll * sPitch * sYaw), (sRoll * cPitch), (cRoll * -sYaw) + (sRoll * sPitch * cYaw), 0.0f },
            { (-sRoll * cYaw) + (cRoll * sPitch * sYaw), (cRoll * cPitch), (sRoll * sYaw) + (cRoll * sPitch * cYaw), 0.0f },
            { (cPitch * sYaw), -sPitch, (cPitch * cYaw), 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        }}};

        matrix = tmp;

        return;
    }

    inline void Transform(Vector4f& vector, const Matrix4X4f& matrix)
    {
        //ispc::Transform(vector, matrix);
        Vector4f vec(vector);
        for (int count = 0; count < countof(vector.data); count++) {
            vector.data[count] = vec.data[0] * matrix.data[0][count]
                + vec.data[1] * matrix.data[1][count]
                + vec.data[2] * matrix.data[2][count]
                + vec.data[3] * matrix.data[3][count];
        }

        return;
    }

    inline void TransformCoord(Vector3f& vector, const Matrix4X4f& matrix)
    {
        //ispc::Transform(vector, matrix);
        Vector4f vec(vector, 1.0f);
        Transform(vec, matrix);
        vector.data[0] = vec.data[0];
        vector.data[1] = vec.data[1];
        vector.data[2] = vec.data[2];
    }

    inline bool InvertMatrix(Matrix4X4f& result, const Matrix4X4f& input)
    {
        BuildIdentityMatrix(result);

        float gaussArray[MATRIX4_SIZE][MATRIX4_SIZE * 2] = {0};
        for (int i = 0; i < MATRIX4_SIZE; i++) {
            for (int j = 0; j < MATRIX4_SIZE; j++) {
                gaussArray[i][j] = input[i][j];
            }
            for (int k = MATRIX4_SIZE; k < MATRIX4_SIZE * 2; k++) {
                gaussArray[i][k] = result[i][k - MATRIX4_SIZE];
            }
        }

        for (int i = 0; i < MATRIX4_SIZE; ++i) {
            // 如果主对角线元素为0，尝试与下面的行交换
            if (std::abs(gaussArray[i][i]) < EP) {
                int swapRow = -1;
                for (int k = i + 1; k < MATRIX4_SIZE; ++k) {
                    if (std::abs(gaussArray[k][i]) > EP) {
                        swapRow = k;
                        break;
                    }
                }

                if (swapRow == -1) {
                    return false; // 矩阵不可逆
                }

                // 交换行
                for (int k = 0; k < MATRIX4_SIZE * 2; ++k) {
                    std::swap(gaussArray[i][k], gaussArray[swapRow][k]);
                }
            }

            // 将主对角线元素化为1
            float pivot = gaussArray[i][i];
            for (int k = 0; k < MATRIX4_SIZE * 2; ++k) {
                gaussArray[i][k] /= pivot;
            }

            // 将其他行的该列元素化为0
            for (int k = 0; k < MATRIX4_SIZE; ++k) {
                if (k != i && std::abs(gaussArray[k][i]) > EP) {
                    float factor = gaussArray[k][i];
                    for (int j = 0; j < MATRIX4_SIZE * 2; ++j) {
                        gaussArray[k][j] -= factor * gaussArray[i][j];
                    }
                }
            }
        }

        // 将结果复制到输出矩阵中
        for (int i = 0; i < MATRIX4_SIZE; ++i) {
            for (int j = 0; j < MATRIX4_SIZE; ++j) {
                result[i][j] = gaussArray[i][j + MATRIX4_SIZE];
            }
        }

        return true;
    }

    inline void BuildViewMatrix(Matrix4X4f& result, const Vector3f position, const Vector3f lookAt, const Vector3f up)
    {
        Vector3f zAxis, xAxis, yAxis;
        float result1, result2, result3;

        zAxis = lookAt - position;
        Normalize(zAxis);

        CrossProduct(xAxis, up, zAxis);
        Normalize(xAxis);

        CrossProduct(yAxis, zAxis, xAxis);

        DotProduct(result1, xAxis, position);
        result1 = -result1;

        DotProduct(result2, yAxis, position);
        result2 = -result2;

        DotProduct(result3, zAxis, position);
        result3 = -result3;

        // Set the computed values in the view matrix.
        Matrix4X4f tmp = {{{
            { xAxis.x, yAxis.x, zAxis.x, 0.0f },
            { xAxis.y, yAxis.y, zAxis.y, 0.0f },
            { xAxis.z, yAxis.z, zAxis.z, 0.0f },
            { result1, result2, result3, 1.0f }
        }}};

        result = tmp;
    }

    inline void BuildViewMatrix(Matrix4X4f& result, const Vector4f position, const Vector4f lookAt, const Vector4f up)
    {
        Vector3f eyePos3(position.x, position.y, position.z);
        Vector3f lookAt3(lookAt.x, lookAt.y, lookAt.z);
        Vector3f up3(up.x, up.y, up.z);
        BuildViewMatrix(result, eyePos3, lookAt3, up3);
    }

    inline void BuildPerspectiveFovLHMatrix(Matrix4X4f& matrix, const float fieldOfView, const float screenAspect, const float screenNear, const float screenDepth)
    {
        Matrix4X4f perspective = {{{
            { 1.0f / (screenAspect * tanf(fieldOfView * 0.5f)), 0.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f / tanf(fieldOfView * 0.5f), 0.0f, 0.0f },
            { 0.0f, 0.0f, screenDepth / (screenDepth - screenNear), 1.0f },
            { 0.0f, 0.0f, (-screenNear * screenDepth) / (screenDepth - screenNear), 0.0f }
        }}};

        matrix = perspective;

        return;
    }


    inline void MatrixTranslation(Matrix4X4f& matrix, const float x, const float y, const float z)
    {
        Matrix4X4f translation = {{{
            { 1.0f, 0.0f, 0.0f, 0.0f},
            { 0.0f, 1.0f, 0.0f, 0.0f},
            { 0.0f, 0.0f, 1.0f, 0.0f},
            {    x,    y,    z, 1.0f}
        }}};

        matrix = translation;

        return;
    }

    inline void MatrixRotationX(Matrix4X4f& matrix, const float angle)
    {
        float c = cosf(angle), s = sinf(angle);

        Matrix4X4f rotation = {{{
            {  1.0f, 0.0f, 0.0f, 0.0f },
            {  0.0f,    c,    s, 0.0f },
            {  0.0f,   -s,    c, 0.0f },
            {  0.0f, 0.0f, 0.0f, 1.0f },
        }}};

        matrix = rotation;

        return;
    }

    inline void MatrixScale(Matrix4X4f& matrix, const float x, const float y, const float z)
    {
        Matrix4X4f scale = { {{
            {    x, 0.0f, 0.0f, 0.0f},
            { 0.0f,    y, 0.0f, 0.0f},
            { 0.0f, 0.0f,    z, 0.0f},
            { 0.0f, 0.0f, 0.0f, 1.0f},
        }} };

        matrix = scale;

        return;
    }

    inline void MatrixRotationY(Matrix4X4f& matrix, const float angle)
    {
        float c = cosf(angle), s = sinf(angle);

        Matrix4X4f rotation = {{{
            {    c, 0.0f,   -s, 0.0f },
            { 0.0f, 1.0f, 0.0f, 0.0f },
            {    s, 0.0f,    c, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f },
        }}};

        matrix = rotation;

        return;
    }


    inline void MatrixRotationZ(Matrix4X4f& matrix, const float angle)
    {
        float c = cosf(angle), s = sinf(angle);

        Matrix4X4f rotation = {{{
            {    c,    s, 0.0f, 0.0f },
            {   -s,    c, 0.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        }}};

        matrix = rotation;

        return;
    }

    inline void MatrixRotationAxis(Matrix4X4f& matrix, const Vector3f& axis, const float angle)
    {
        float c = cosf(angle), s = sinf(angle), one_minus_c = 1.0f - c;

        Matrix4X4f rotation = { {{
            {   c + axis.x * axis.x * one_minus_c,  axis.x * axis.y * one_minus_c + axis.z * s, axis.x * axis.z * one_minus_c - axis.y * s, 0.0f    },
            {   axis.x * axis.y * one_minus_c - axis.z * s, c + axis.y * axis.y * one_minus_c,  axis.y * axis.z * one_minus_c + axis.x * s, 0.0f    },
            {   axis.x * axis.z * one_minus_c + axis.y * s, axis.y * axis.z * one_minus_c - axis.x * s, c + axis.z * axis.z * one_minus_c, 0.0f },
            {   0.0f,  0.0f,  0.0f,  1.0f   }
        }} };

        matrix = rotation;
    }

    inline void MatrixRotationQuaternion(Matrix4X4f& matrix, Quaternion q)
    {
        Matrix4X4f rotation = { {{
            {   1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z,  2.0f * q.x * q.y + 2.0f * q.w * q.z,   2.0f * q.x * q.z - 2.0f * q.w * q.y,    0.0f    },
            {   2.0f * q.x * q.y - 2.0f * q.w * q.z,    1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z, 2.0f * q.y * q.z + 2.0f * q.w * q.x,    0.0f    },
            {   2.0f * q.x * q.z + 2.0f * q.w * q.y,    2.0f * q.y * q.z - 2.0f * q.w * q.x, 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y,    0.0f    },
            {   0.0f,   0.0f,   0.0f,   1.0f    }
        }} };

        matrix = rotation;
    }

    inline Quaternion ReserveQuaternion(Quaternion q)
    {
        Quaternion conjugate = Quaternion(-q.x, -q.y, -q.z, q.w);
        double length = std::pow(q.x, 2) + std::pow(q.y, 2) + std::pow(q.z, 2) + std::pow(q.w, 2);
        conjugate.x = conjugate.x / length;
        conjugate.y = conjugate.y / length;
        conjugate.z = conjugate.z / length;
        conjugate.w = conjugate.w / length;
        return conjugate;
    }

    inline Matrix8X8f DCT8X8(const Matrix8X8f& matrix)
    {
        Matrix8X8f result;
        
        return result;
    }

    inline Matrix8X8f IDCT8X8(const Matrix8X8f& matrix)
    {
        Matrix8X8f result;
        memset(&result, 0, sizeof(float) * 64);
        float data[64];
        for (int i = 0; i < 64; i++) {
            data[i] = .0f;
        }
        result = data;

        return result;
    }



    inline Matrix4X4f BuildScaleMatrix(float x, float y, float z, float w)
    {
        Matrix4X4f trans;
        float v[16] = {
            x, 0.0f, 0.0f, 0.0f,
            0.0f, y, 0.0f, 0.0f,
            0.0f, 0.0f, z, 0.0f,
            0.0f, 0.0f, 0.0f, w 
        };
        trans = v;
        return trans;
    }

    inline Matrix4X4f BuildRotationMatrix(float x, float y, float z, float w)
    {
        Matrix4X4f trans;
        float v[16] = {
            1 - 2 * y * y - 2 * z * z, 2 * x * y + 2 * w * z, 2 * x * z - 2 * w * y, 0.0f,
            2 * x * y - 2 * w * z, 1 - 2 * x * x - 2 * z * z, 2 * y * z + 2 * w * x, 0.0f,
            2 * x * z + 2 * w * y, 2 * y * z - 2 * w * x, 1 - 2 * x * x - 2 * y * y, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        trans = v;
        return trans;
    }

    inline Matrix4X4f BuildTranslationMatrix(float x, float y, float z, float w)
    {
        Matrix4X4f trans;
        float v[16] = {
            1.0f, 0.0f, 0.0f, x,
            0.0f, 1.0f, 0.0f, y,
            0.0f, 0.0f, 1.0f, z,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        trans = v;
        return trans;
    }


    inline Matrix4X4f BuildOrthographicMatrix(float left, float right, float bottom, float top, float nearF, float farF) {
        Matrix4X4f result = { {{
            { 1.0f, 0.0f, 0.0f, 0.0f},
            { 0.0f, 1.0f, 0.0f, 0.0f},
            { 0.0f, 0.0f, 1.0f, 0.0f},
            { 0.0f, 0.0f, 0.0f, 1.0f}
        }} };
        result[0][0] = 2.0f / (right - left);
        result[1][1] = 2.0f / (top - bottom);
        result[2][2] = 2.0f / (farF - nearF);
        result[3][0] = -(right + left) / (right - left);
        result[3][1] = -(top + bottom) / (top - bottom);
        result[3][2] = -(farF + nearF) / (farF - nearF);
        result[3][3] = 1.0f;
        return result;
    }

    inline Matrix4X4f BuildPerspectiveMatrix(float fov, float aspect, float nearF, float farF) {
        Matrix4X4f result = { {{
            { 1.0f, 0.0f, 0.0f, 0.0f},
            { 0.0f, 1.0f, 0.0f, 0.0f},
            { 0.0f, 0.0f, 1.0f, 0.0f},
            { 0.0f, 0.0f, 0.0f, 1.0f}
        }} };
        float tanHalfFov = tan(fov / 2.0f);
        result[0][0] = 1.0f / (aspect * tanHalfFov);
        result[1][1] = 1.0f / tanHalfFov;
        result[2][2] = -(farF + nearF) / (farF - nearF);
        result[3][3] = 0.0f;
        result[2][3] = -1.0f;
        result[3][2] = -(2.0f * farF * nearF) / (farF - nearF);
        return result;
    }

    inline Vector4f ConvertZPositive2YPositive(const Vector4f& vec)
    {
        Matrix4X4f trans = { {{
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, -1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        }} };
    }

    inline void ReserveVector(Vector4f& vec)
    {
        vec.x = -vec.x;
        vec.y = -vec.y;
        vec.z = -vec.z;
    }

    inline Vector4f GetReserveVector(const Vector4f& vec)
    {
        return Vector4f(-vec.x, -vec.y, -vec.z, vec.w);
    }

    static Matrix4X4f g_IdentityMatrix = {{{
            { 1.0f, 0.0f, 0.0f, 0.0f},
            { 0.0f, 1.0f, 0.0f, 0.0f},
            { 0.0f, 0.0f, 1.0f, 0.0f},
            { 0.0f, 0.0f, 0.0f, 1.0f}
        }}};

    static Vector4f g_PositionX = { 1.0f, 0.0f, 0.0f, 1.0f };
    static Vector4f g_PositionY = { 0.0f, 1.0f, 0.0f, 1.0f };
    static Vector4f g_PositionZ = { 0.0f, 0.0f, 1.0f, 1.0f };
    static Vector4f g_VectorX = { 1.0f, 0.0f, 0.0f, 0.0f };
    static Vector4f g_VectorY = { 0.0f, 1.0f, 0.0f, 0.0f };
    static Vector4f g_VectorZ = { 0.0f, 0.0f, 1.0f, 0.0f };
    static Vector4f g_VectorXReserve = { -1.0f, 0.0f, 0.0f, 0.0f };
    static Vector4f g_VectorYReserve = { 0.0f, -1.0f, 0.0f, 0.0f };
    static Vector4f g_VectorZReserve = { 0.0f, 0.0f, -1.0f, 0.0f };
    static Vector4f g_VectorZero = { 0.0f, 0.0f, 1.0f, 0.0f };
}
