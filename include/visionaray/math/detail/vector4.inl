// This file is distributed under the MIT license.
// See the LICENSE file for details.

#include <type_traits>

#include "../simd/builtin.h"
#include "../simd/neon.h"
#include "../simd/sse.h"
#include "../simd/type_traits.h"
#include "../config.h"
#include "math.h"

namespace MATH_NAMESPACE
{

//--------------------------------------------------------------------------------------------------
// vector4 members
//

template <typename T>
MATH_FUNC
inline vector<4, T>::vector(T const& x, T const& y, T const& z, T const& w)
    : x(x)
    , y(y)
    , z(z)
    , w(w)
{
}

template <typename T>
MATH_FUNC
inline vector<4, T>::vector(T const& s)
    : x(s)
    , y(s)
    , z(s)
    , w(s)
{
}

template <typename T>
MATH_FUNC
inline vector<4, T>::vector(T const data[4])
    : x(data[0])
    , y(data[1])
    , z(data[2])
    , w(data[3])
{
}

template <typename T>
template <typename U>
MATH_FUNC
inline vector<4, T>::vector(vector<2, U> const& rhs, U const& z, U const& w)
    : x(rhs.x)
    , y(rhs.y)
    , z(z)
    , w(w)
{
}

template <typename T>
template <typename U>
MATH_FUNC
inline vector<4, T>::vector(vector<2, U> const& first, vector<2, U> const& second)
    : x(first.x)
    , y(first.y)
    , z(second.x)
    , w(second.y)
{
}

template <typename T>
template <typename U>
MATH_FUNC
inline vector<4, T>::vector(vector<3, U> const& rhs, U const& w)
    : x(rhs.x)
    , y(rhs.y)
    , z(rhs.z)
    , w(w)
{
}

template <typename T>
template <typename U>
MATH_FUNC
inline vector<4, T>::vector(vector<4, U> const& rhs)
    : x(rhs.x)
    , y(rhs.y)
    , z(rhs.z)
    , w(rhs.w)
{
}

template <typename T>
template <typename U>
MATH_FUNC
inline vector<4, T>& vector<4, T>::operator=(vector<4, U> const& rhs)
{

    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    w = rhs.w;
    return *this;

}

template <typename T>
MATH_FUNC
inline T* vector<4, T>::data()
{
    return reinterpret_cast<T*>(this);
}

template <typename T>
MATH_FUNC
inline T const* vector<4, T>::data() const
{
    return reinterpret_cast<T const*>(this);
}

template <typename T>
MATH_FUNC
inline T& vector<4, T>::operator[](unsigned i)
{
    return data()[i];
}

template <typename T>
MATH_FUNC
inline T const& vector<4, T>::operator[](unsigned i) const
{
    return data()[i];
}

template <typename T>
MATH_FUNC
inline vector<2, T>& vector<4, T>::xy()
{
    return *reinterpret_cast<vector<2, T>*>( data() );
}

template <typename T>
MATH_FUNC
inline vector<2, T> const& vector<4, T>::xy() const
{
    return *reinterpret_cast<vector<2, T> const*>( data() );
}

template <typename T>
MATH_FUNC
inline vector<3, T>& vector<4, T>::xyz()
{
    return *reinterpret_cast<vector<3, T>*>( data() );
}

template <typename T>
MATH_FUNC
inline vector<3, T> const& vector<4, T>::xyz() const
{
    return *reinterpret_cast<vector<3, T> const*>( data() );
}


//--------------------------------------------------------------------------------------------------
// Basic arithmetic
//

template <typename T>
MATH_FUNC
inline vector<4, T> operator+(vector<4, T> const& v)
{
    return vector<4, T>(+v.x, +v.y, +v.z, +v.w);
}

template <typename T>
MATH_FUNC
inline vector<4, T> operator-(vector<4, T> const& v)
{
    return vector<4, T>(-v.x, -v.y, -v.z, -v.w);
}

template <typename T>
MATH_FUNC
inline vector<4, T> operator+(vector<4, T> const& u, vector<4, T> const& v)
{
    return vector<4, T>(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);
}

template <typename T>
MATH_FUNC
inline vector<4, T> operator-(vector<4, T> const& u, vector<4, T> const& v)
{
    return vector<4, T>(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);
}

template <typename T>
MATH_FUNC
inline vector<4, T> operator*(vector<4, T> const& u, vector<4, T> const& v)
{
    return vector<4, T>(u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w);
}

template <typename T>
MATH_FUNC
inline vector<4, T> operator/(vector<4, T> const& u, vector<4, T> const& v)
{
    return vector<4, T>(u.x / v.x, u.y / v.y, u.z / v.z, u.w / v.w);
}

template <typename T>
MATH_FUNC
inline vector<4, T> operator+(vector<4, T> const& v, T const& s)
{
    return vector<4, T>(v.x + s, v.y + s, v.z + s, v.w + s);
}

template <typename T>
MATH_FUNC
inline vector<4, T> operator-(vector<4, T> const& v, T const& s)
{
    return vector<4, T>(v.x - s, v.y - s, v.z - s, v.w - s);
}

template <typename T>
MATH_FUNC
inline vector<4, T> operator*(vector<4, T> const& v, T const& s)
{
    return vector<4, T>(v.x * s, v.y * s, v.z * s, v.w * s);
}

template <typename T>
MATH_FUNC
inline vector<4, T> operator/(vector<4, T> const& v, T const& s)
{
    return vector<4, T>(v.x / s, v.y / s, v.z / s, v.w / s);
}

template <typename T>
MATH_FUNC
inline vector<4, T> operator+(T const& s, vector<4, T> const& v)
{
    return vector<4, T>(s + v.x, s + v.y, s + v.z, s + v.w);
}

template <typename T>
MATH_FUNC
inline vector<4, T> operator-(T const& s, vector<4, T> const& v)
{
    return vector<4, T>(s - v.x, s - v.y, s - v.z, s - v.w);
}

template <typename T>
MATH_FUNC
inline vector<4, T> operator*(T const& s, vector<4, T> const& v)
{
    return vector<4, T>(s * v.x, s * v.y, s * v.z, s * v.w);
}

template <typename T>
MATH_FUNC
inline vector<4, T> operator/(T const& s, vector<4, T> const& v)
{
    return vector<4, T>(s / v.x, s / v.y, s / v.z, s / v.w);
}


//--------------------------------------------------------------------------------------------------
// Comparisons
//

template <typename T>
MATH_FUNC
inline auto operator==(vector<4, T> const& u, vector<4, T> const& v)
    -> decltype(u.x == v.x)
{
    return u.x == v.x && u.y == v.y && u.z == v.z && u.w == v.w;
}

template <typename T>
MATH_FUNC
inline auto operator<(vector<4, T> const& u, vector<4, T> const& v)
    -> decltype(u.x < v.x)
{
    return u.x < v.x || ( (u.x == v.x && u.y < v.y) || ( (u.y == v.y && u.z < v.z) || (u.z == v.z && u.w < v.w) ) );
}

template <typename T>
MATH_FUNC
inline auto operator!=(vector<4, T> const& u, vector<4, T> const& v)
    -> decltype(u == v)
{
    return !(u == v);
}

template <typename T>
MATH_FUNC
inline auto operator<=(vector<4, T> const& u, vector<4, T> const& v)
    -> decltype(v < u)
{
    return !(v < u);
}

template <typename T>
MATH_FUNC
inline auto operator>(vector<4, T> const& u, vector<4, T> const& v)
    -> decltype(v < u)
{
    return v < u;
}

template <typename T>
MATH_FUNC
inline auto operator>=(vector<4, T> const& u, vector<4, T> const& v)
    -> decltype(u < v)
{
    return !(u < v);
}


//--------------------------------------------------------------------------------------------------
// Geometric functions
//

template <typename T>
MATH_FUNC
inline T dot(vector<4, T> const& u, vector<4, T> const& v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w;
}

template <typename T>
MATH_FUNC
inline T norm(vector<4, T> const& v)
{
    return sqrt( dot(v, v) );
}

template <typename T>
MATH_FUNC
inline T norm2(vector<4, T> const& v)
{
    return dot(v, v);
}

template <typename T>
MATH_FUNC
inline T length(vector<4, T> const& v)
{
    return norm(v);
}

template <typename T>
MATH_FUNC
inline vector<4, T> normalize(vector<4, T> const& v)
{
    return v * rsqrt( dot(v, v) );
}


//-------------------------------------------------------------------------------------------------
// Misc.
//

template <typename M, typename T>
MATH_FUNC
vector<4, T> select(M const& m, vector<4, T> const& u, vector<4, T> const& v)
{
    return vector<4, T>(
            select(m, u.x, v.x),
            select(m, u.y, v.y),
            select(m, u.z, v.z),
            select(m, u.w, v.w)
            );
}

template <typename M, typename T1, typename T2>
MATH_FUNC
auto select(M const& m, vector<4, T1> const& u, vector<4, T2> const& v)
    -> vector<4, decltype(select(m, u.x, v.x))>
{
    using T3 = decltype(select(m, u.x, v.x));

    return vector<4, T3>(
            select(m, u.x, v.x),
            select(m, u.y, v.y),
            select(m, u.z, v.z),
            select(m, u.w, v.w)
            );
}

template <typename T>
MATH_FUNC
inline vector<4, T> min(vector<4, T> const& u, vector<4, T> const& v)
{
    return vector<4, T>( min(u.x, v.x), min(u.y, v.y), min(u.z, v.z), min(u.w, v.w) );
}

template <typename T>
MATH_FUNC
inline vector<4, T> max(vector<4, T> const& u, vector<4, T> const& v)
{
    return vector<4, T>( max(u.x, v.x), max(u.y, v.y), max(u.z, v.z), max(u.w, v.w) );
}

template <typename T>
MATH_FUNC
inline T hadd(vector<4, T> const& u)
{
    return u.x + u.y + u.z + u.w;
}

template <typename T>
MATH_FUNC
inline T hmul(vector<4, T> const& u)
{
    return u.x * u.y * u.z * u.w;
}


namespace simd
{

//-------------------------------------------------------------------------------------------------
// SIMD conversions
//

// pack ---------------------------------------------------

template <typename T, size_t N>
MATH_FUNC
inline vector<4, float_from_simd_width_t<N>> pack(array<vector<4, T>, N> const& vecs)
{
    using U = float_from_simd_width_t<N>; // TODO: generalize, not just float!

    vector<4, U> result;

    T* x = reinterpret_cast<T*>(&result.x);
    T* y = reinterpret_cast<T*>(&result.y);
    T* z = reinterpret_cast<T*>(&result.z);
    T* w = reinterpret_cast<T*>(&result.w);

    for (size_t i = 0; i < N; ++i)
    {
        x[i] = vecs[i].x;
        y[i] = vecs[i].y;
        z[i] = vecs[i].z;
        w[i] = vecs[i].w;
    }

    return result;
}

// unpack -------------------------------------------------

template <
    typename T,
    typename = typename std::enable_if<is_simd_vector<T>::value>::type
    >
MATH_FUNC
inline array<vector<4, element_type_t<T>>, num_elements<T>::value> unpack(vector<4, T> const& v)
{
    using U = element_type_t<T>;

    U const* x = reinterpret_cast<U const*>(&v.x);
    U const* y = reinterpret_cast<U const*>(&v.y);
    U const* z = reinterpret_cast<U const*>(&v.z);
    U const* w = reinterpret_cast<U const*>(&v.w);

    array<vector<4, U>, num_elements<T>::value> result;

    for (int i = 0; i < num_elements<T>::value; ++i)
    {
        result[i].x = x[i];
        result[i].y = y[i];
        result[i].z = z[i];
        result[i].w = w[i];
    }

    return result;
}

// Transpose to get from SoA to AoS (and vice versa)
// Similar to mat4 transpose
MATH_FUNC
inline vector<4, float4> transpose(vector<4, float4> const& v)
{
    float4 tmp0 = interleave_lo(v.x, v.y);
    float4 tmp1 = interleave_lo(v.z, v.w);
    float4 tmp2 = interleave_hi(v.x, v.y);
    float4 tmp3 = interleave_hi(v.z, v.w);

    return vector<4, float4>(
            move_lo(tmp0, tmp1),
            move_hi(tmp1, tmp0),
            move_lo(tmp2, tmp3),
            move_hi(tmp3, tmp2)
            );
}

// TODO: transpose for AVX?

} // simd
} // MATH_NAMESPACE
