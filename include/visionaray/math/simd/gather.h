// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#ifndef VSNRAY_MATH_SIMD_GATHER_H
#define VSNRAY_MATH_SIMD_GATHER_H 1

#include <cstdint>
#include <type_traits>

#include <visionaray/array.h>

#include "avx.h"
#include "avx512.h"
#include "builtin.h"
#include "neon.h"
#include "sse.h"

// Insert math headers after platform headers to inhibit ADL!
#include "../norm.h"
#include "../vector.h"

namespace MATH_NAMESPACE
{
namespace simd
{

//-------------------------------------------------------------------------------------------------
// Gather loads from memory to SIMD vectors
//
//
// The gather functions use dedicated hardware instructions when applicable
// and resort to software implementations if no instruction is available.
//
//
// Implemented for the following cases:
//  - base address: unorm<N>,            index type: int4
//  - base address: unorm<N>,            index type: int8
//  - base address: unorm<N>,            index type: int16
//  - base address: float,               index type: int4
//  - base address: float,               index type: int8
//  - base address: float,               index type: int16
//  - base address: Int,                 index type: int4
//  - base address: Int,                 index type: int8
//  - base address: Int,                 index type: int16
//
//  - base address: vector<4, float>,    index type: int4
//  - base address: vector<4, float>,    index type: int8
//  - base address: vector<4, float>,    index type: int16
//  - base address: vector<N, float>,    index type: int4
//  - base address: vector<N, float>,    index type: int8
//
//  - base address: vector<N, unorm<M>>, index type: int4
//  - base address: vector<N, unorm<M>>, index type: int8
//  - base address: vector<N, unorm<M>>, index type: int16
//  - base address: vector<N, Int>>,     index type: int4
//  - base address: vector<N, Int>>,     index type: int8
//  - base address: vector<N, Int>>,     index type: int16
//
//  , where I in (u)int{8|16|32|64}_t
//
//
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// Gather float4 from N-bit unorm array, N <= 32
// No dedicated AVX2 instruction!
//

template <unsigned Bits>
VSNRAY_FORCE_INLINE float4 gather(unorm<Bits> const* base_addr, int4 const& index)
{
    static_assert(Bits <= 32, "Incompatible unorm type");

    VSNRAY_ALIGN(16) int indices[4];
    store(&indices[0], index);

    return float4(
        static_cast<float>(base_addr[indices[0]]),
        static_cast<float>(base_addr[indices[1]]),
        static_cast<float>(base_addr[indices[2]]),
        static_cast<float>(base_addr[indices[3]])
        );
}


//-------------------------------------------------------------------------------------------------
// Gather float8 from N-bit unorm array, N <= 32
// No dedicated AVX2 instruction!
//

template <unsigned Bits>
VSNRAY_FORCE_INLINE float8 gather(unorm<Bits> const* base_addr, int8 const& index)
{
    static_assert(Bits <= 32, "Incompatible unorm type");

    VSNRAY_ALIGN(32) int indices[8];
    store(&indices[0], index);

    return float8(
        static_cast<float>(base_addr[indices[0]]),
        static_cast<float>(base_addr[indices[1]]),
        static_cast<float>(base_addr[indices[2]]),
        static_cast<float>(base_addr[indices[3]]),
        static_cast<float>(base_addr[indices[4]]),
        static_cast<float>(base_addr[indices[5]]),
        static_cast<float>(base_addr[indices[6]]),
        static_cast<float>(base_addr[indices[7]])
        );
}


//-------------------------------------------------------------------------------------------------
// Gather float16 from N-bit unorm array, N <= 32
// No dedicated AVX-512 instruction!
//

template <unsigned Bits>
VSNRAY_FORCE_INLINE float16 gather(unorm<Bits> const* base_addr, int16 const& index)
{
    static_assert(Bits <= 32, "Incompatible unorm type");

    VSNRAY_ALIGN(64) int indices[16];
    store(&indices[0], index);

    return float16(
        static_cast<float>(base_addr[indices[ 0]]),
        static_cast<float>(base_addr[indices[ 1]]),
        static_cast<float>(base_addr[indices[ 2]]),
        static_cast<float>(base_addr[indices[ 3]]),
        static_cast<float>(base_addr[indices[ 4]]),
        static_cast<float>(base_addr[indices[ 5]]),
        static_cast<float>(base_addr[indices[ 6]]),
        static_cast<float>(base_addr[indices[ 7]]),
        static_cast<float>(base_addr[indices[ 8]]),
        static_cast<float>(base_addr[indices[ 9]]),
        static_cast<float>(base_addr[indices[10]]),
        static_cast<float>(base_addr[indices[11]]),
        static_cast<float>(base_addr[indices[12]]),
        static_cast<float>(base_addr[indices[13]]),
        static_cast<float>(base_addr[indices[14]]),
        static_cast<float>(base_addr[indices[15]])
        );
}


//-------------------------------------------------------------------------------------------------
// Gather float4 from 32-bit float array
//

VSNRAY_FORCE_INLINE float4 gather(float const* base_addr, int4 const& index)
{
#if VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_AVX2)
    return _mm_i32gather_ps(base_addr, index, 4);
#else

    VSNRAY_ALIGN(16) int indices[4];
    store(&indices[0], index);

    return float4(
        base_addr[indices[0]],
        base_addr[indices[1]],
        base_addr[indices[2]],
        base_addr[indices[3]]
        );

#endif
}


//-------------------------------------------------------------------------------------------------
// Gather float8 from 32-bit float array
//

VSNRAY_FORCE_INLINE float8 gather(float const* base_addr, int8 const& index)
{
#if VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_AVX2)
    return _mm256_i32gather_ps(base_addr, index, 4);
#else

    VSNRAY_ALIGN(32) int indices[8];
    store(&indices[0], index);

    return float8(
        base_addr[indices[0]],
        base_addr[indices[1]],
        base_addr[indices[2]],
        base_addr[indices[3]],
        base_addr[indices[4]],
        base_addr[indices[5]],
        base_addr[indices[6]],
        base_addr[indices[7]]
        );

#endif
}


//-------------------------------------------------------------------------------------------------
// Gather float8 from 32-bit float array
//

VSNRAY_FORCE_INLINE float16 gather(float const* base_addr, int16 const& index)
{
#if VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_AVX512F)
    return _mm512_i32gather_ps(index, base_addr, 4);
#else

    VSNRAY_ALIGN(64) int indices[16];
    store(&indices[0], index);

    return float16(
        base_addr[indices[ 0]],
        base_addr[indices[ 1]],
        base_addr[indices[ 2]],
        base_addr[indices[ 3]],
        base_addr[indices[ 4]],
        base_addr[indices[ 5]],
        base_addr[indices[ 6]],
        base_addr[indices[ 7]],
        base_addr[indices[ 8]],
        base_addr[indices[ 9]],
        base_addr[indices[10]],
        base_addr[indices[11]],
        base_addr[indices[12]],
        base_addr[indices[13]],
        base_addr[indices[14]],
        base_addr[indices[15]]
        );

#endif
}



//-------------------------------------------------------------------------------------------------
// Gather int4 from any integer array
// No dedicated AVX2 instruction (see special case for 32-bit integer arrays)!
//

template <
    typename I,
    typename = typename std::enable_if<std::is_integral<I>::value>::type
    >
VSNRAY_FORCE_INLINE int4 gather(I const* base_addr, int4 const& index)
{
    VSNRAY_ALIGN(16) int indices[4];
    store(&indices[0], index);

    return int4(
        static_cast<int32_t>(base_addr[indices[0]]),
        static_cast<int32_t>(base_addr[indices[1]]),
        static_cast<int32_t>(base_addr[indices[2]]),
        static_cast<int32_t>(base_addr[indices[3]])
        );
}


//-------------------------------------------------------------------------------------------------
// Gather int8 from any integer array
// No dedicated AVX2 instruction (see special case for 32-bit integer arrays)!
//

template <
    typename I,
    typename = typename std::enable_if<std::is_integral<I>::value>::type
    >
VSNRAY_FORCE_INLINE int8 gather(I const* base_addr, int8 const& index)
{
    VSNRAY_ALIGN(32) int indices[8];
    store(&indices[0], index);

    return int8(
        static_cast<int32_t>(base_addr[indices[0]]),
        static_cast<int32_t>(base_addr[indices[1]]),
        static_cast<int32_t>(base_addr[indices[2]]),
        static_cast<int32_t>(base_addr[indices[3]]),
        static_cast<int32_t>(base_addr[indices[4]]),
        static_cast<int32_t>(base_addr[indices[5]]),
        static_cast<int32_t>(base_addr[indices[6]]),
        static_cast<int32_t>(base_addr[indices[7]])
        );
}


//-------------------------------------------------------------------------------------------------
// Gather int16 from any integer array
// No dedicated AVX-512 instruction (see special case for 32-bit integer arrays)!
//

template <
    typename I,
    typename = typename std::enable_if<std::is_integral<I>::value>::type
    >
VSNRAY_FORCE_INLINE int16 gather(I const* base_addr, int16 const& index)
{
    VSNRAY_ALIGN(64) int indices[16];
    store(&indices[0], index);

    return int16(
        static_cast<int32_t>(base_addr[indices[ 0]]),
        static_cast<int32_t>(base_addr[indices[ 1]]),
        static_cast<int32_t>(base_addr[indices[ 2]]),
        static_cast<int32_t>(base_addr[indices[ 3]]),
        static_cast<int32_t>(base_addr[indices[ 4]]),
        static_cast<int32_t>(base_addr[indices[ 5]]),
        static_cast<int32_t>(base_addr[indices[ 6]]),
        static_cast<int32_t>(base_addr[indices[ 7]]),
        static_cast<int32_t>(base_addr[indices[ 8]]),
        static_cast<int32_t>(base_addr[indices[ 9]]),
        static_cast<int32_t>(base_addr[indices[10]]),
        static_cast<int32_t>(base_addr[indices[11]]),
        static_cast<int32_t>(base_addr[indices[12]]),
        static_cast<int32_t>(base_addr[indices[13]]),
        static_cast<int32_t>(base_addr[indices[14]]),
        static_cast<int32_t>(base_addr[indices[15]])
        );
}


//-------------------------------------------------------------------------------------------------
// Gather int4 from 32-bit integer array
//

VSNRAY_FORCE_INLINE int4 gather(int const* base_addr, int4 const& index)
{
#if VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_AVX2)
    return _mm_i32gather_epi32(base_addr, index, 4);
#else

    VSNRAY_ALIGN(16) int indices[4];
    store(&indices[0], index);

    return int4(
        base_addr[indices[0]],
        base_addr[indices[1]],
        base_addr[indices[2]],
        base_addr[indices[3]]
        );

#endif
}


//-------------------------------------------------------------------------------------------------
// Gather int8 from 32-bit integer array
//

VSNRAY_FORCE_INLINE int8 gather(int const* base_addr, int8 const& index)
{
#if VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_AVX2)
    return _mm256_i32gather_epi32(base_addr, index, 4);
#else

    VSNRAY_ALIGN(32) int indices[8];
    store(&indices[0], index);

    return int8(
        base_addr[indices[0]],
        base_addr[indices[1]],
        base_addr[indices[2]],
        base_addr[indices[3]],
        base_addr[indices[4]],
        base_addr[indices[5]],
        base_addr[indices[6]],
        base_addr[indices[7]]
        );

#endif
}


//-------------------------------------------------------------------------------------------------
// Gather int16 from 32-bit integer array
//

VSNRAY_FORCE_INLINE int16 gather(int const* base_addr, int16 const& index)
{
#if VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_AVX512F)
    return _mm512_i32gather_epi32(index, base_addr, 4);
#else


    VSNRAY_ALIGN(64) int indices[16];
    store(&indices[0], index);

    return int16(
        base_addr[indices[ 0]],
        base_addr[indices[ 1]],
        base_addr[indices[ 2]],
        base_addr[indices[ 3]],
        base_addr[indices[ 4]],
        base_addr[indices[ 5]],
        base_addr[indices[ 6]],
        base_addr[indices[ 7]],
        base_addr[indices[ 8]],
        base_addr[indices[ 9]],
        base_addr[indices[10]],
        base_addr[indices[11]],
        base_addr[indices[12]],
        base_addr[indices[13]],
        base_addr[indices[14]],
        base_addr[indices[15]]
        );

#endif
}


//-------------------------------------------------------------------------------------------------
// Gather vector<N, float4> from vector<N, float> array
//

template <size_t Dim>
VSNRAY_FORCE_INLINE vector<Dim, float4> gather(vector<Dim, float> const* base_addr, int4 const& index)
{
    VSNRAY_ALIGN(16) int indices[4];
    store(&indices[0], index);

    array<vector<Dim, float>, 4> arr {{
            base_addr[indices[0]],
            base_addr[indices[1]],
            base_addr[indices[2]],
            base_addr[indices[3]]
            }};

    return simd::pack(arr);
}


template <size_t Dim>
VSNRAY_FORCE_INLINE vector<Dim, float8> gather(vector<Dim, float> const* base_addr, int8 const& index)
{
#if 0 // AVX2

    // TODO: check if gather intrinsic can be used here!

#else

    VSNRAY_ALIGN(32) int indices[8];
    store(&indices[0], index);

    array<vector<Dim, float>, 8> arr {{
            base_addr[indices[0]],
            base_addr[indices[1]],
            base_addr[indices[2]],
            base_addr[indices[3]],
            base_addr[indices[4]],
            base_addr[indices[5]],
            base_addr[indices[6]],
            base_addr[indices[7]]
            }};

    return simd::pack(arr);

#endif
}


template <size_t Dim>
VSNRAY_FORCE_INLINE vector<Dim, float16> gather(vector<Dim, float> const* base_addr, int16 const& index)
{
//#if VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_AVX512F)
//  TODO: check if gather intrinsic can be used here!
//#endif

    VSNRAY_ALIGN(64) int indices[16];
    store(&indices[0], index);

    array<vector<Dim, float>, 16> arr {{
            base_addr[indices[ 0]],
            base_addr[indices[ 1]],
            base_addr[indices[ 2]],
            base_addr[indices[ 3]],
            base_addr[indices[ 4]],
            base_addr[indices[ 5]],
            base_addr[indices[ 6]],
            base_addr[indices[ 7]],
            base_addr[indices[ 8]],
            base_addr[indices[ 9]],
            base_addr[indices[10]],
            base_addr[indices[11]],
            base_addr[indices[12]],
            base_addr[indices[13]],
            base_addr[indices[14]],
            base_addr[indices[15]]
            }};

    return simd::pack(arr);
}


//-------------------------------------------------------------------------------------------------
// Gather vector<4, float4> from vector<4, float> array
//

VSNRAY_FORCE_INLINE vector<4, float4> gather(vector<4, float> const* base_addr, int4 const& index)
{
    //-----------------------------------------------------
    // Optimization for AoS data.
    //
    // Data is gathered w/o a context switch to GP
    // registers by transposing to SoA after memory
    // lookup.
    //

    VSNRAY_ALIGN(16) int indices[4];
    store(&indices[0], index * 4);

    float const* tmp = reinterpret_cast<float const*>(base_addr);

    vector<4, simd::float4> result(
            &tmp[0] + indices[0],
            &tmp[0] + indices[1],
            &tmp[0] + indices[2],
            &tmp[0] + indices[3]
            );

    result = transpose(result);
    return result;
}


//-------------------------------------------------------------------------------------------------
// Gather vector<4, float8> from vector<4, float> array
//

VSNRAY_FORCE_INLINE vector<4, float8> gather(vector<4, float> const* base_addr, int8 const& index)
{
#if VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_AVX2)

    float const* tmp = reinterpret_cast<float const*>(base_addr);

    return vector<4, simd::float8>(
            _mm256_i32gather_ps(tmp, index * 4    , 4),
            _mm256_i32gather_ps(tmp, index * 4 + 1, 4),
            _mm256_i32gather_ps(tmp, index * 4 + 2, 4),
            _mm256_i32gather_ps(tmp, index * 4 + 3, 4)
            );

#else

    VSNRAY_ALIGN(32) int indices[8];
    store(&indices[0], index);

    array<vector<4, float>, 8> arr{{
            base_addr[indices[0]],
            base_addr[indices[1]],
            base_addr[indices[2]],
            base_addr[indices[3]],
            base_addr[indices[4]],
            base_addr[indices[5]],
            base_addr[indices[6]],
            base_addr[indices[7]]
            }};


    return simd::pack(arr);

#endif
}


//-------------------------------------------------------------------------------------------------
// Gather vector<4, float16> from vector<4, float> array
//

VSNRAY_FORCE_INLINE vector<4, float16> gather(vector<4, float> const* base_addr, int16 const& index)
{
#if VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_AVX512F)
    float const* tmp = reinterpret_cast<float const*>(base_addr);

    return vector<4, simd::float16>(
            _mm512_i32gather_ps(index * 4    , tmp, 4),
            _mm512_i32gather_ps(index * 4 + 1, tmp, 4),
            _mm512_i32gather_ps(index * 4 + 2, tmp, 4),
            _mm512_i32gather_ps(index * 4 + 3, tmp, 4)
            );
#else

    VSNRAY_ALIGN(64) int indices[16];
    store(&indices[0], index);

    array<vector<4, float>, 16> arr{{
            base_addr[indices[ 0]],
            base_addr[indices[ 1]],
            base_addr[indices[ 2]],
            base_addr[indices[ 3]],
            base_addr[indices[ 4]],
            base_addr[indices[ 5]],
            base_addr[indices[ 6]],
            base_addr[indices[ 7]],
            base_addr[indices[ 8]],
            base_addr[indices[ 9]],
            base_addr[indices[10]],
            base_addr[indices[11]],
            base_addr[indices[12]],
            base_addr[indices[13]],
            base_addr[indices[14]],
            base_addr[indices[15]]
            }};

    return simd::pack(arr);

#endif
}


//-------------------------------------------------------------------------------------------------
// Gather vector<Dim, int4> from vector<Dim, Int> array
// No dedicated AVX2 instruction!
//

template <
    size_t Dim,
    typename I,
    typename = typename std::enable_if<std::is_integral<I>::value>::type
    >
VSNRAY_FORCE_INLINE vector<Dim, int4> gather(vector<Dim, I> const* base_addr, int4 const& index)
{
    using V = vector<Dim, int32_t>;

    VSNRAY_ALIGN(16) int indices[4];
    store(&indices[0], index);

    array<V, 4> arr{{
            V(base_addr[indices[0]]),
            V(base_addr[indices[1]]),
            V(base_addr[indices[2]]),
            V(base_addr[indices[3]])
            }};

    return simd::pack(arr);
}


//-------------------------------------------------------------------------------------------------
// Gather vector<Dim, int8> from vector<Dim, Int> array
// No dedicated AVX2 instruction!
//

template <
    size_t Dim,
    typename I,
    typename = typename std::enable_if<std::is_integral<I>::value>::type
    >
VSNRAY_FORCE_INLINE vector<Dim, int8> gather(vector<Dim, I> const* base_addr, int8 const& index)
{
    using V = vector<Dim, int32_t>;

    VSNRAY_ALIGN(32) int indices[8];
    store(&indices[0], index);

    array<V, 8> arr{{
            V(base_addr[indices[0]]),
            V(base_addr[indices[1]]),
            V(base_addr[indices[2]]),
            V(base_addr[indices[3]]),
            V(base_addr[indices[4]]),
            V(base_addr[indices[5]]),
            V(base_addr[indices[6]]),
            V(base_addr[indices[7]])
            }};

    return simd::pack(arr);
}


//-------------------------------------------------------------------------------------------------
// Gather vector<Dim, int16> from vector<Dim, Int> array
// No dedicated AVX-512 instruction!
//

template <
    size_t Dim,
    typename I,
    typename = typename std::enable_if<std::is_integral<I>::value>::type
    >
VSNRAY_FORCE_INLINE vector<Dim, int16> gather(vector<Dim, I> const* base_addr, int16 const& index)
{
    using V = vector<Dim, int32_t>;

    VSNRAY_ALIGN(64) int indices[16];
    store(&indices[0], index);

    array<V, 16> arr{{
            V(base_addr[indices[ 0]]),
            V(base_addr[indices[ 1]]),
            V(base_addr[indices[ 2]]),
            V(base_addr[indices[ 3]]),
            V(base_addr[indices[ 4]]),
            V(base_addr[indices[ 5]]),
            V(base_addr[indices[ 6]]),
            V(base_addr[indices[ 7]]),
            V(base_addr[indices[ 8]]),
            V(base_addr[indices[ 9]]),
            V(base_addr[indices[10]]),
            V(base_addr[indices[11]]),
            V(base_addr[indices[12]]),
            V(base_addr[indices[13]]),
            V(base_addr[indices[14]]),
            V(base_addr[indices[15]])
            }};

    return simd::pack(arr);
}


//-------------------------------------------------------------------------------------------------
// Gather vector<Dim, float8> from vector<Dim, unorm<Bits>> array, Bits <= 32
// No dedicated AVX2 instruction!
//

template <size_t Dim, unsigned Bits>
VSNRAY_FORCE_INLINE vector<Dim, float4> gather(vector<Dim, unorm<Bits>> const* base_addr, int4 const& index)
{
    static_assert(Bits <= 32, "Incompatible unorm type");

    using V = vector<Dim, float>;

    VSNRAY_ALIGN(16) int indices[4];
    store(&indices[0], index);

    array<V, 4> arr{{
            V(base_addr[indices[0]]),
            V(base_addr[indices[1]]),
            V(base_addr[indices[2]]),
            V(base_addr[indices[3]]),
            }};

    return simd::pack(arr);
}


//-------------------------------------------------------------------------------------------------
// Gather vector<Dim, float8> from vector<Dim, unorm<Bits>> array, Bits <= 32
// No dedicated AVX2 instruction!
//

template <size_t Dim, unsigned Bits>
VSNRAY_FORCE_INLINE vector<Dim, float8> gather(vector<Dim, unorm<Bits>> const* base_addr, int8 const& index)
{
    static_assert(Bits <= 32, "Incompatible unorm type");

    using V = vector<Dim, float>;

    VSNRAY_ALIGN(32) int indices[8];
    store(&indices[0], index);

    array<V, 8> arr{{
            V(base_addr[indices[0]]),
            V(base_addr[indices[1]]),
            V(base_addr[indices[2]]),
            V(base_addr[indices[3]]),
            V(base_addr[indices[4]]),
            V(base_addr[indices[5]]),
            V(base_addr[indices[6]]),
            V(base_addr[indices[7]])
            }};

    return simd::pack(arr);
}


//-------------------------------------------------------------------------------------------------
// Gather vector<Dim, float16> from vector<Dim, unorm<Bits>> array, Bits <= 32
// No dedicated AVX-512F instruction!
//

template <size_t Dim, unsigned Bits>
VSNRAY_FORCE_INLINE vector<Dim, float16> gather(vector<Dim, unorm<Bits>> const* base_addr, int16 const& index)
{
    static_assert(Bits <= 32, "Incompatible unorm type");

    using V = vector<Dim, float>;

    VSNRAY_ALIGN(64) int indices[16];
    store(&indices[0], index);

    array<V, 16> arr{{
            V(base_addr[indices[ 0]]),
            V(base_addr[indices[ 1]]),
            V(base_addr[indices[ 2]]),
            V(base_addr[indices[ 3]]),
            V(base_addr[indices[ 4]]),
            V(base_addr[indices[ 5]]),
            V(base_addr[indices[ 6]]),
            V(base_addr[indices[ 7]]),
            V(base_addr[indices[ 8]]),
            V(base_addr[indices[ 9]]),
            V(base_addr[indices[10]]),
            V(base_addr[indices[11]]),
            V(base_addr[indices[12]]),
            V(base_addr[indices[13]]),
            V(base_addr[indices[14]]),
            V(base_addr[indices[15]])
            }};

    return simd::pack(arr);
}

} // simd
} // MATH_NAMESPACE

#endif // VSNRAY_MATH_SIMD_GATHER_H
