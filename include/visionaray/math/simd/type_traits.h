// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#ifndef VSNRAY_SIMD_TYPE_TRAITS_H
#define VSNRAY_SIMD_TYPE_TRAITS_H 1

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
#endif

#include "forward.h"
#include "intrinsics.h"

namespace MATH_NAMESPACE
{
namespace simd
{

//-------------------------------------------------------------------------------------------------
// Traits for SIMD vector types
//
//
// Traits have default implementations that are compatible with elementary C++ types
//
//
//  - alignment_of:
//      get the required alignment for the SIMD vector type
//      default: value := alignof(T)
//
//  - aligned_array:
//      get an array that adheres to the alignment requirements and that can store
//      the contents of a SIMD vector type
//      default: n/a
//      helper type: aligned_array_t
//
//  - float_type:
//      get a compatible float type for a SIMD vector type
//      default: type := float
//      helper type: float_type_t
//
//  - int_type
//      get a compatible int type for a SIMD vector type
//      default: type := int
//      helper type: int_type_t
//
//  - mask_type:
//      get a compatible mask type for a SIMD vector type
//      default: type := bool
//      helper type: mask_type_t
//
//  - native_type:
//      get the native type for a SIMD vectory type
//      mask types that are based on unions may map to int_type<T>
//      default: n/a
//      helper type: native_type_t
//
//  - float_from_simd_width:
//      get the best matching floating point type for a given SIMD width
//      the returned type depends on the ISA compiled for
//      default: n/a
//      helper type: float_from_simd_with_t
//
//  - int_from_simd_width:
//      get the best matching signed integer type for a given SIMD width
//      the returned type depends on the ISA compiled for
//      default: n/a
//      helper type: int_from_simd_width_t
//
//  - mask_from_simd_width:
//      get the best matching mask type for a given SIMD width
//      the returned type depends on the ISA compiled for
//      default: n/a
//      helper type: mask_from_simd_width_t
//
//  - is_simd_vector
//      check if T is a SIMD vector type
//      default: value := false
//
//  - element_type:
//      get the elementary type of a SIMD vector component
//      default: T <= T
//      helper type: element_type_t
//
//  - num_elements:
//      get the number of vector components for a SIMD vector type
//      default: value := 1
//
//  - wider_simd_type
//      given two FP types, returns the one with the greater SIMD width
//
//
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// Required alignment, use when allocating data used with SIMD vector types
//

// general ------------------------------------------------

template <typename T>
struct alignment_of
{
    enum { value = alignof(T) };
};

// SSE ----------------------------------------------------

template <>
struct alignment_of<simd::float4>
{
    enum { value = 16 };
};

template <>
struct alignment_of<simd::int4>
{
    enum { value = 16 };
};

template <>
struct alignment_of<simd::mask4>
{
    enum { value = 16 };
};

// AVX ----------------------------------------------------

template <>
struct alignment_of<simd::float8>
{
    enum { value = 32 };
};

template <>
struct alignment_of<simd::int8>
{
    enum { value = 32 };
};

template <>
struct alignment_of<simd::mask8>
{
    enum { value = 32 };
};

// AVX-512 ------------------------------------------------

template <>
struct alignment_of<simd::float16>
{
    enum { value = 64 };
};

template <>
struct alignment_of<simd::int16>
{
    enum { value = 64 };
};

template <>
struct alignment_of<simd::mask16>
{
    enum { value = 64 };
};


//-------------------------------------------------------------------------------------------------
// Get an array that adheres to the alignment requirements and that can store
// the contents of a SIMD vector type
//

// general (n/a) ------------------------------------------

template <typename T>
struct aligned_array;

// SSE ----------------------------------------------------

template <>
struct aligned_array<simd::float4>
{
    typedef VSNRAY_ALIGN(16) float type[4];
};

template <>
struct aligned_array<simd::int4>
{
    typedef VSNRAY_ALIGN(16) int type[4];
};

template <>
struct aligned_array<simd::mask4>
{
    typedef VSNRAY_ALIGN(16) bool type[4];
};

// AVX ----------------------------------------------------

template <>
struct aligned_array<simd::float8>
{
    typedef VSNRAY_ALIGN(32) float type[8];
};

template <>
struct aligned_array<simd::int8>
{
    typedef VSNRAY_ALIGN(32) int type[8];
};

template <>
struct aligned_array<simd::mask8>
{
    typedef VSNRAY_ALIGN(32) bool type[8];
};

// AVX-512 ------------------------------------------------

template <>
struct aligned_array<simd::float16>
{
    typedef VSNRAY_ALIGN(64) float type[16];
};

template <>
struct aligned_array<simd::int16>
{
    typedef VSNRAY_ALIGN(64) int type[16];
};

template <>
struct aligned_array<simd::mask16>
{
    typedef VSNRAY_ALIGN(64) bool type[16];
};

// helper type --------------------------------------------

template <typename T>
using aligned_array_t = typename aligned_array<T>::type;


//-------------------------------------------------------------------------------------------------
// Deduce float type from SIMD vector types
//

// general ------------------------------------------------

template <typename T>
struct float_type
{
    using type = float;
};

// SSE ----------------------------------------------------

template <>
struct float_type<simd::float4>
{
    using type = simd::float4;
};

template <>
struct float_type<simd::int4>
{
    using type = simd::float4;
};

template <>
struct float_type<simd::mask4>
{
    using type = simd::float4;
};

// AVX ----------------------------------------------------

template <>
struct float_type<simd::float8>
{
    using type = simd::float8;
};

template <>
struct float_type<simd::int8>
{
    using type = simd::float8;
};

template <>
struct float_type<simd::mask8>
{
    using type = simd::float8;
};

// AVX-512 ------------------------------------------------

template <>
struct float_type<simd::float16>
{
    using type = simd::float16;
};

template <>
struct float_type<simd::int16>
{
    using type = simd::float16;
};

template <>
struct float_type<simd::mask16>
{
    using type = simd::float16;
};

// helper type --------------------------------------------

template <typename T>
using float_type_t = typename float_type<T>::type;


//-------------------------------------------------------------------------------------------------
// Deduce int type from SIMD vector types
//

// general ------------------------------------------------

template <typename T>
struct int_type
{
    using type = int;
};

// SSE ----------------------------------------------------

template <>
struct int_type<simd::float4>
{
    using type = simd::int4;
};

template <>
struct int_type<simd::int4>
{
    using type = simd::int4;
};

template <>
struct int_type<simd::mask4>
{
    using type = simd::int4;
};

// AVX ----------------------------------------------------

template <>
struct int_type<simd::float8>
{
    using type = simd::int8;
};

template <>
struct int_type<simd::int8>
{
    using type = simd::int8;
};

template <>
struct int_type<simd::mask8>
{
    using type = simd::int8;
};

// AVX-512 ------------------------------------------------

template <>
struct int_type<simd::float16>
{
    using type = simd::int16;
};

template <>
struct int_type<simd::int16>
{
    using type = simd::int16;
};

template <>
struct int_type<simd::mask16>
{
    using type = simd::int16;
};

// helper type --------------------------------------------

template <typename T>
using int_type_t = typename int_type<T>::type;


//-------------------------------------------------------------------------------------------------
// Deduce mask type from SIMD vector types
//

// general ------------------------------------------------

template <typename T>
struct mask_type
{
    using type = bool;
};

// SSE ----------------------------------------------------

template <>
struct mask_type<simd::float4>
{
    using type = simd::mask4;
};

template <>
struct mask_type<simd::int4>
{
    using type = simd::mask4;
};

template <>
struct mask_type<simd::mask4>
{
    using type = simd::mask4;
};

// AVX ----------------------------------------------------

template <>
struct mask_type<simd::float8>
{
    using type = simd::mask8;
};

template <>
struct mask_type<simd::int8>
{
    using type = simd::mask8;
};

template <>
struct mask_type<simd::mask8>
{
    using type = simd::mask8;
};

// AVX-512 ------------------------------------------------

template <>
struct mask_type<simd::float16>
{
    using type = simd::mask16;
};

template <>
struct mask_type<simd::int16>
{
    using type = simd::mask16;
};

template <>
struct mask_type<simd::mask16>
{
    using type = simd::mask16;
};

// helper type --------------------------------------------

template <typename T>
using mask_type_t = typename mask_type<T>::type;


//-------------------------------------------------------------------------------------------------
// Deduce native type from SIMD vector types
//

// general (n/a) ------------------------------------------

template <typename T>
struct native_type;

#if VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_SSE2)

// SSE ----------------------------------------------------

template <>
struct native_type<simd::float4>
{
    using type = __m128;
};

template <>
struct native_type<simd::int4>
{
    using type = __m128i;
};

template <>
struct native_type<simd::mask4>
{
    using type = __m128i;
};

#elif VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_NEON_FP)

// NEON ---------------------------------------------------

template <>
struct native_type<simd::float4>
{
    using type = float32x4_t;
};

template <>
struct native_type<simd::int4>
{
    using type = int32x4_t;
};

template <>
struct native_type<simd::mask4>
{
    using type = uint32x4_t;
};

#else

// builtin ------------------------------------------------

template <>
struct native_type<simd::float4>
{
    using type = float[4];
};

template <>
struct native_type<simd::int4>
{
    using type = int[4];
};

template <>
struct native_type<simd::mask4>
{
    using type = bool[4];
};

#endif

#if VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_AVX)

// AVX ----------------------------------------------------

template <>
struct native_type<simd::float8>
{
    using type = __m256;
};

template <>
struct native_type<simd::int8>
{
    using type = __m256i;
};

template <>
struct native_type<simd::mask8>
{
    using type = __m256i;
};

#else

// builtin ------------------------------------------------

template <>
struct native_type<simd::float8>
{
    using type = float[8];
};

template <>
struct native_type<simd::int8>
{
    using type = int[8];
};

template <>
struct native_type<simd::mask8>
{
    using type = bool[8];
};

#endif

#if VSNRAY_SIMD_ISA_GE(VSNRAY_SIMD_ISA_AVX512F)

// AVX-512 ------------------------------------------------

template <>
struct native_type<simd::float16>
{
    using type = __m512;
};

template <>
struct native_type<simd::int16>
{
    using type = __m512i;
};

template <>
struct native_type<simd::mask16>
{
    using type = __mmask16;
};

#else

// builtin ------------------------------------------------

template <>
struct native_type<simd::float16>
{
    using type = float[16];
};

template <>
struct native_type<simd::int16>
{
    using type = int[16];
};

template <>
struct native_type<simd::mask16>
{
    using type = bool[16];
};

#endif

// helper type --------------------------------------------

template <typename T>
using native_type_t = typename native_type<T>::type;


//-------------------------------------------------------------------------------------------------
// Deduce SIMD floating point type from a given SIMD width
//

// general (n/a) ------------------------------------------

template <unsigned Width>
struct float_from_simd_width;

// SSE ----------------------------------------------------

template <>
struct float_from_simd_width<4>
{
    using type = simd::float4;
};

// AVX ----------------------------------------------------

template <>
struct float_from_simd_width<8>
{
    using type = simd::float8;
};

// AVX-512 ------------------------------------------------

template <>
struct float_from_simd_width<16>
{
    using type = simd::float16;
};

// helper type --------------------------------------------

template <unsigned Width>
using float_from_simd_width_t = typename float_from_simd_width<Width>::type;


//-------------------------------------------------------------------------------------------------
// Deduce SIMD signed integer type from a given SIMD width
//

// general (n/a) ------------------------------------------

template <unsigned Width>
struct int_from_simd_width;

// SSE ----------------------------------------------------

template <>
struct int_from_simd_width<4>
{
    using type = simd::int4;
};

// AVX ----------------------------------------------------

template <>
struct int_from_simd_width<8>
{
    using type = simd::int8;
};

// AVX-512 ------------------------------------------------

template <>
struct int_from_simd_width<16>
{
    using type = simd::int16;
};

// helper type --------------------------------------------

template <unsigned Width>
using int_from_simd_width_t = typename int_from_simd_width<Width>::type;


//-------------------------------------------------------------------------------------------------
// Deduce SIMD mask type from a given SIMD width
//

// general (n/a) ------------------------------------------

template <unsigned Width>
struct mask_from_simd_width;

// SSE ----------------------------------------------------

template <>
struct mask_from_simd_width<4>
{
    using type = simd::mask4;
};

// AVX ----------------------------------------------------

template <>
struct mask_from_simd_width<8>
{
    using type = simd::mask8;
};

// AVX-512 ------------------------------------------------

template <>
struct mask_from_simd_width<16>
{
    using type = simd::mask16;
};

// helper type --------------------------------------------

template <unsigned Width>
using mask_from_simd_width_t = typename mask_from_simd_width<Width>::type;


//-------------------------------------------------------------------------------------------------
// Check if a given type T is a SIMD vector type
//

// general ------------------------------------------------

template <typename T>
struct is_simd_vector
{
    enum { value = false };
};

// SSE ----------------------------------------------------

template <>
struct is_simd_vector<simd::float4>
{
    enum { value = true };
};

template <>
struct is_simd_vector<simd::int4>
{
    enum { value = true };
};

template <>
struct is_simd_vector<simd::mask4>
{
    enum { value = true };
};

// AVX ----------------------------------------------------

template <>
struct is_simd_vector<simd::float8>
{
    enum { value = true };
};

template <>
struct is_simd_vector<simd::int8>
{
    enum { value = true };
};

template <>
struct is_simd_vector<simd::mask8>
{
    enum { value = true };
};

// AVX-512 ------------------------------------------------

template <>
struct is_simd_vector<simd::float16>
{
    enum { value = true };
};

template <>
struct is_simd_vector<simd::int16>
{
    enum { value = true };
};

template <>
struct is_simd_vector<simd::mask16>
{
    enum { value = true };
};


//-------------------------------------------------------------------------------------------------
// Get the elementary type belonging to a SIMD vector type
//

// general ------------------------------------------------

template <typename T>
struct element_type
{
    using type = T;
};

// SSE ----------------------------------------------------

template <>
struct element_type<simd::float4>
{
    using type = float;
};

template <>
struct element_type<simd::int4>
{
    using type = int;
};

template <>
struct element_type<simd::mask4>
{
    using type = bool;
};

// AVX ----------------------------------------------------

template <>
struct element_type<simd::float8>
{
    using type = float;
};

template <>
struct element_type<simd::int8>
{
    using type = int;
};

template <>
struct element_type<simd::mask8>
{
    using type = bool;
};

// AVX-512 ------------------------------------------------

template <>
struct element_type<simd::float16>
{
    using type = float;
};

template <>
struct element_type<simd::int16>
{
    using type = int;
};

template <>
struct element_type<simd::mask16>
{
    using type = bool;
};

// helper type --------------------------------------------

template <typename T>
using element_type_t = typename element_type<T>::type;


//-------------------------------------------------------------------------------------------------
// Deduce number of vector components from SIMD vector types
//

// general ------------------------------------------------

template <typename T>
struct num_elements
{
    enum { value = 1 };
};

// SSE ----------------------------------------------------

template <>
struct num_elements<simd::float4>
{
    enum { value = 4 };
};

template <>
struct num_elements<simd::int4>
{
    enum { value = 4 };
};

template <>
struct num_elements<simd::mask4>
{
    enum { value = 4 };
};

// AVX ----------------------------------------------------

template <>
struct num_elements<simd::float8>
{
    enum { value = 8 };
};

template <>
struct num_elements<simd::int8>
{
    enum { value = 8 };
};

template <>
struct num_elements<simd::mask8>
{
    enum { value = 8 };
};

// AVX-512 ------------------------------------------------

template <>
struct num_elements<simd::float16>
{
    enum { value = 16 };
};

template <>
struct num_elements<simd::int16>
{
    enum { value = 16 };
};

template <>
struct num_elements<simd::mask16>
{
    enum { value = 16 };
};

//-------------------------------------------------------------------------------------------------
// Given two floating point types, return the one with the wider simd width
//

// general ------------------------------------------------

template <typename T, typename U>
struct wider_simd_type
{
};

template <>
struct wider_simd_type<float, float>
{
    using type = float;
};

// SSE ----------------------------------------------------

template <>
struct wider_simd_type<float, simd::float4>
{
    using type = simd::float4;
};

template <>
struct wider_simd_type<simd::float4, float>
{
    using type = simd::float4;
};

// AVX ----------------------------------------------------

template <>
struct wider_simd_type<float, simd::float8>
{
    using type = simd::float8;
};

template <>
struct wider_simd_type<simd::float8, float>
{
    using type = simd::float8;
};

template <>
struct wider_simd_type<simd::float4, simd::float8>
{
    using type = simd::float8;
};

template <>
struct wider_simd_type<simd::float8, simd::float4>
{
    using type = simd::float8;
};

// AVX-512 ------------------------------------------------

template <>
struct wider_simd_type<float, simd::float16>
{
    using type = simd::float16;
};

template <>
struct wider_simd_type<simd::float16, float>
{
    using type = simd::float16;
};

template <>
struct wider_simd_type<simd::float4, simd::float16>
{
    using type = simd::float16;
};

template <>
struct wider_simd_type<simd::float16, simd::float4>
{
    using type = simd::float16;
};

template <>
struct wider_simd_type<simd::float8, simd::float16>
{
    using type = simd::float16;
};

template <>
struct wider_simd_type<simd::float16, simd::float8>
{
    using type = simd::float16;
};

} // simd
} // MATH_NAMESPACE

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif // VSNRAY_SIMD_TYPE_TRAITS_H
