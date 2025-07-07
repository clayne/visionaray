// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#ifndef VSNRAY_MATH_SIMD_FORWARD_H
#define VSNRAY_MATH_SIMD_FORWARD_H 1

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
#endif

#include "intrinsics.h"
#include "../config.h"
#include "../forward.h"


namespace MATH_NAMESPACE
{
namespace simd
{

template <typename T>
class basic_float;

template <typename T>
class basic_int;

template <typename T>
class basic_uint;

template <typename... Args>
union basic_mask;


//-------------------------------------------------------------------------------------------------
// typedefs
//

#if VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_SSE2)
typedef basic_int<__m128i>                      int4;
typedef basic_float<__m128>                     float4;
typedef basic_mask<__m128, __m128i>             mask4;
#elif VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_NEON_FP)
typedef basic_int<int32x4_t>                    int4;
typedef basic_float<float32x4_t>                float4;
typedef basic_mask<uint32x4_t>                  mask4;
#else
typedef basic_int<int[4]>                       int4;
typedef basic_float<float[4]>                   float4;
typedef basic_mask<bool[4]>                     mask4;
#endif

#if VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_AVX)
typedef basic_int<__m256i>                      int8;
typedef basic_float<__m256>                     float8;
typedef basic_mask<__m256, __m256i>             mask8;
#elif VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_NEON_FP)
typedef basic_int<int32x4_t[2]>                 int8;
typedef basic_uint<uint32x4_t[2]>               uint8;
typedef basic_float<float32x4_t[2]>             float8;
typedef basic_mask<uint32x4_t[2]>               mask8;
#else
typedef basic_int<int[8]>                       int8;
typedef basic_float<float[8]>                   float8;
typedef basic_mask<bool[8]>                     mask8;
#endif

#if VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_AVX512F)
typedef basic_int<__m512i>                      int16;
typedef basic_float<__m512>                     float16;
typedef basic_mask<__mmask16>                   mask16;
#else
typedef basic_int<int[16]>                      int16;
typedef basic_float<float[16]>                  float16;
typedef basic_mask<bool[16]>                    mask16;
#endif

typedef basic_ray<float4>                       ray4;
typedef basic_ray<float8>                       ray8;
typedef basic_ray<float16>                      ray16;


} // simd
} // MATH_NAMESPACE

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif // VSNRAY_MATH_SIMD_FORWARD_H
