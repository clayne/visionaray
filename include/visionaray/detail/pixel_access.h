// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#ifndef VSNRAY_DETAIL_PIXEL_ACCESS_H
#define VSNRAY_DETAIL_PIXEL_ACCESS_H 1

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
#endif

#include <type_traits>

#include <visionaray/math/simd/type_traits.h>
#include <visionaray/math/vector.h>
#include <visionaray/array.h>
#include <visionaray/blending.h>
#include <visionaray/packet_traits.h>
#include <visionaray/pixel_format.h>
#include <visionaray/result_record.h>

#include "color_conversion.h"
#include "macros.h"

namespace visionaray
{
namespace detail
{

//-------------------------------------------------------------------------------------------------
// Store, get and blend pixel values (color and depth)
//

namespace pixel_access
{

// Store ------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// Store an input color to an output color buffer, apply color conversion
//

template <pixel_format DF, pixel_format SF, typename InputColor, typename OutputColor>
VSNRAY_FUNC
inline void store(
        pixel_format_constant<DF>   /* dst format */,
        pixel_format_constant<SF>   /* src format */,
        int                         x,
        int                         y,
        int                         width,
        int                         height,
        InputColor const&           color,
        OutputColor*                buffer
        )
{
    VSNRAY_UNUSED(height);

    convert(
        pixel_format_constant<DF>{},
        pixel_format_constant<SF>{},
        buffer[y * width + x],
        color
        );
}

//-------------------------------------------------------------------------------------------------
// Store SIMD rgb color to RGB8 render target, apply conversion
// OutputColor must be rgb
// TODO: consolidate all RGB copies
// TODO: consolidate with rgba version
//

template <
    typename OutputColor,
    typename FloatT,
    typename = typename std::enable_if<simd::is_simd_vector<FloatT>::value>::type
    >
VSNRAY_FUNC
inline void store(
        pixel_format_constant<PF_RGB8>      /* dst format */,
        pixel_format_constant<PF_RGB32F>    /* src format */,
        int                                 x,
        int                                 y,
        int                                 width,
        int                                 height,
        vector<3, FloatT> const&            color,
        OutputColor*                        buffer
        )
{
    using float_array = simd::aligned_array_t<FloatT>;

    float_array r;
    float_array g;
    float_array b;

    store(r, color.x);
    store(g, color.y);
    store(b, color.z);

    const int w = packet_size<FloatT>::w;
    const int h = packet_size<FloatT>::h;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                int idx = row * w + col;
                convert(
                    pixel_format_constant<PF_RGB8>{},
                    pixel_format_constant<PF_RGB32F>{},
                    buffer[(y + row) * width + (x + col)],
                    vec3(r[idx], g[idx], b[idx])
                    );
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
// Store SIMD rgb color to RGB32F render target, apply conversion
// OutputColor must be rgb
//

template <
    typename OutputColor,
    typename FloatT,
    typename = typename std::enable_if<simd::is_simd_vector<FloatT>::value>::type
    >
VSNRAY_FUNC
inline void store(
        pixel_format_constant<PF_RGB32F>    /* dst format */,
        pixel_format_constant<PF_RGB32F>    /* src format */,
        int                                 x,
        int                                 y,
        int                                 width,
        int                                 height,
        vector<3, FloatT> const&            color,
        OutputColor*                        buffer
        )
{
    using float_array = simd::aligned_array_t<FloatT>;

    float_array r;
    float_array g;
    float_array b;

    store(r, color.x);
    store(g, color.y);
    store(b, color.z);

    const int w = packet_size<FloatT>::w;
    const int h = packet_size<FloatT>::h;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                int idx = row * w + col;
                convert(
                    pixel_format_constant<PF_RGB32F>{},
                    pixel_format_constant<PF_RGB32F>{},
                    buffer[(y + row) * width + (x + col)],
                    vec3(r[idx], g[idx], b[idx])
                    );
            }
        }
    }
}


//-------------------------------------------------------------------------------------------------
// Store SIMD rgba color to RGB32F render target, apply conversion
// OutputColor must be rgb
//

template <
    typename OutputColor,
    typename FloatT,
    typename = typename std::enable_if<simd::is_simd_vector<FloatT>::value>::type
    >
VSNRAY_FUNC
inline void store(
        pixel_format_constant<PF_RGB32F>    /* dst format */,
        pixel_format_constant<PF_RGBA32F>   /* src format */,
        int                                 x,
        int                                 y,
        int                                 width,
        int                                 height,
        vector<4, FloatT> const&            color,
        OutputColor*                        buffer
        )
{
    using float_array = simd::aligned_array_t<FloatT>;

    float_array r;
    float_array g;
    float_array b;
    float_array a;

    store(r, color.x);
    store(g, color.y);
    store(b, color.z);
    store(a, color.w);

    const int w = packet_size<FloatT>::w;
    const int h = packet_size<FloatT>::h;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                int idx = row * w + col;
                convert(
                    pixel_format_constant<PF_RGB32F>{},
                    pixel_format_constant<PF_RGBA32F>{},
                    buffer[(y + row) * width + (x + col)],
                    vec4(r[idx], g[idx], b[idx], a[idx])
                    );
            }
        }
    }
}


//-------------------------------------------------------------------------------------------------
// Store SIMD rgba color to RGBA8 render target, apply conversion
// OutputColor must be rgba
// TODO: consolidate all rgba copies
// TODO: consolidate with rgb version
//

template <
    typename OutputColor,
    typename FloatT,
    typename = typename std::enable_if<simd::is_simd_vector<FloatT>::value>::type
    >
VSNRAY_FUNC
inline void store(
        pixel_format_constant<PF_RGBA8>     /* dst format */,
        pixel_format_constant<PF_RGBA32F>   /* src format */,
        int                                 x,
        int                                 y,
        int                                 width,
        int                                 height,
        vector<4, FloatT> const&            color,
        OutputColor*                        buffer
        )
{
    using float_array = simd::aligned_array_t<FloatT>;

    float_array r;
    float_array g;
    float_array b;
    float_array a;

    store(r, color.x);
    store(g, color.y);
    store(b, color.z);
    store(a, color.w);

    const int w = packet_size<FloatT>::w;
    const int h = packet_size<FloatT>::h;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                int idx = row * w + col;
                convert(
                    pixel_format_constant<PF_RGBA8>{},
                    pixel_format_constant<PF_RGBA32F>{},
                    buffer[(y + row) * width + (x + col)],
                    vec4(r[idx], g[idx], b[idx], a[idx])
                    );
            }
        }
    }
}


//-------------------------------------------------------------------------------------------------
// Store SIMD rgba color to RGB8 render target, apply conversion
// OutputColor must be rgba
// TODO: consolidate all rgba copies
// TODO: consolidate with rgb version
//

template <
    typename OutputColor,
    typename FloatT,
    typename = typename std::enable_if<simd::is_simd_vector<FloatT>::value>::type
    >
VSNRAY_FUNC
inline void store(
        pixel_format_constant<PF_RGB8>      /* dst format */,
        pixel_format_constant<PF_RGBA32F>   /* src format */,
        int                                 x,
        int                                 y,
        int                                 width,
        int                                 height,
        vector<4, FloatT> const&            color,
        OutputColor*                        buffer
        )
{
    using float_array = simd::aligned_array_t<FloatT>;

    float_array r;
    float_array g;
    float_array b;
    float_array a;

    store(r, color.x);
    store(g, color.y);
    store(b, color.z);
    store(a, color.w);

    const int w = packet_size<FloatT>::w;
    const int h = packet_size<FloatT>::h;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                int idx = row * w + col;
                convert(
                    pixel_format_constant<PF_RGB8>{},
                    pixel_format_constant<PF_RGBA32F>{},
                    buffer[(y + row) * width + (x + col)],
                    vec4(r[idx], g[idx], b[idx], a[idx])
                    );
            }
        }
    }
}


//-------------------------------------------------------------------------------------------------
// Store SIMD rgba color to RGBA32F render target, apply conversion
// OutputColor must be rgba
//

template <
    typename OutputColor,
    typename FloatT,
    typename = typename std::enable_if<simd::is_simd_vector<FloatT>::value>::type
    >
VSNRAY_FUNC
inline void store(
        pixel_format_constant<PF_RGBA32F>   /* dst format */,
        pixel_format_constant<PF_RGBA32F>   /* src format */,
        int                                 x,
        int                                 y,
        int                                 width,
        int                                 height,
        vector<4, FloatT> const&            color,
        OutputColor*                        buffer
        )
{
    using float_array = simd::aligned_array_t<FloatT>;

    float_array r;
    float_array g;
    float_array b;
    float_array a;

    store(r, color.x);
    store(g, color.y);
    store(b, color.z);
    store(a, color.w);

    const int w = packet_size<FloatT>::w;
    const int h = packet_size<FloatT>::h;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                int idx = row * w + col;
                convert(
                    pixel_format_constant<PF_RGBA32F>{},
                    pixel_format_constant<PF_RGBA32F>{},
                    buffer[(y + row) * width + (x + col)],
                    vec4(r[idx], g[idx], b[idx], a[idx])
                    );
            }
        }
    }
}


//-------------------------------------------------------------------------------------------------
// Store SoA rgba color to RGBA32F color buffer, no conversion necessary
// Special treatment, can convert from SoA to AoS using transpose
//

VSNRAY_FUNC
inline void store(
        pixel_format_constant<PF_RGBA32F>   /* dst format */,
        pixel_format_constant<PF_RGBA32F>   /* src format */,
        int                                 x,
        int                                 y,
        int                                 width,
        int                                 height,
        vector<4, simd::float4> const&      color,
        vector<4, float>*                   buffer
        )
{
    auto c = transpose(color);

    if ( x      < width &&  y      < height) store( buffer[ y      * width +  x     ].data(), c.x);
    if ((x + 1) < width &&  y      < height) store( buffer[ y      * width + (x + 1)].data(), c.y);
    if ( x      < width && (y + 1) < height) store( buffer[(y + 1) * width +  x     ].data(), c.z);
    if ((x + 1) < width && (y + 1) < height) store( buffer[(y + 1) * width + (x + 1)].data(), c.w);
}

//-------------------------------------------------------------------------------------------------
// Store single SIMD channel to 32-bit FP buffer, no conversion
// TODO: consolidate various overloads
//

template <
    typename FloatT,
    typename = typename std::enable_if<simd::is_simd_vector<FloatT>::value>::type
    >
VSNRAY_FUNC
inline void store(
        pixel_format_constant<PF_DEPTH32F>  /* dst format */,
        pixel_format_constant<PF_DEPTH32F>  /* src format */,
        int                                 x,
        int                                 y,
        int                                 width,
        int                                 height,
        FloatT const&                       value,
        float*                              buffer
        )
{
    using float_array = simd::aligned_array_t<FloatT>;

    float_array v;

    store(v, value);

    const int w = packet_size<FloatT>::w;
    const int h = packet_size<FloatT>::h;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                convert(
                    pixel_format_constant<PF_DEPTH32F>{},
                    pixel_format_constant<PF_DEPTH32F>{},
                    buffer[(y + row) * width + (x + col)],
                    v[row * w + col]
                    );
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
// Store single SIMD channel to 32-bit FP buffer, no conversion
// TODO: consolidate various overloads
//

template <
    typename T,
    typename = typename std::enable_if<simd::is_simd_vector<T>::value>::type
    >
VSNRAY_FUNC
inline void store(
        pixel_format_constant<PF_DEPTH24_STENCIL8>  /* dst format */,
        pixel_format_constant<PF_DEPTH32F>          /* src format */,
        int                                         x,
        int                                         y,
        int                                         width,
        int                                         height,
        T const&                                    value,
        unsigned*                                   buffer
        )
{
    using float_array = simd::aligned_array_t<T>;

    float_array v;

    store(v, value);

    const int w = packet_size<T>::w;
    const int h = packet_size<T>::h;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                convert(
                    pixel_format_constant<PF_DEPTH24_STENCIL8>{},
                    pixel_format_constant<PF_DEPTH32F>{},
                    buffer[(y + row) * width + (x + col)],
                    v[row * w + col]
                    );
            }
        }
    }
}

template <
    typename FloatT,
    typename = typename std::enable_if<simd::is_simd_vector<FloatT>::value>::type
    >
VSNRAY_FUNC
inline void store(
        pixel_format_constant<PF_R32F>  /* dst format */,
        pixel_format_constant<PF_R32F>  /* src format */,
        int                             x,
        int                             y,
        int                             width,
        int                             height,
        FloatT const&                   value,
        float*                          buffer
        )
{
    using float_array = simd::aligned_array_t<FloatT>;

    float_array v;

    store(v, value);

    const int w = packet_size<FloatT>::w;
    const int h = packet_size<FloatT>::h;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                convert(
                    pixel_format_constant<PF_R32F>{},
                    pixel_format_constant<PF_R32F>{},
                    buffer[(y + row) * width + (x + col)],
                    v[row * w + col]
                    );
            }
        }
    }
}


//-------------------------------------------------------------------------------------------------
// Store color from result record to output color buffer
//

template <pixel_format DF, pixel_format SF, typename T, typename OutputColor>
VSNRAY_FUNC
inline void store(
        pixel_format_constant<DF>   /* dst format */,
        pixel_format_constant<SF>   /* src format */,
        int                         x,
        int                         y,
        int                         width,
        int                         height,
        result_record<T> const&     rr,
        OutputColor*                buffer
        )
{
    store(
        pixel_format_constant<DF>{},
        pixel_format_constant<SF>{},
        x,
        y,
        width,
        height,
        rr.color,
        buffer
        );
}


// Get -------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// Get a color from an output color buffer, apply conversion
//

template <pixel_format DF, pixel_format SF, typename InputColor, typename OutputColor>
VSNRAY_FUNC
inline void get(
        pixel_format_constant<DF>   /* dst format */,
        pixel_format_constant<SF>   /* src format */,
        int                         x,
        int                         y,
        int                         width,
        int                         height,
        InputColor&                 color,
        OutputColor const*          buffer
        )
{
    VSNRAY_UNUSED(height);

    convert(
        pixel_format_constant<DF>{},
        pixel_format_constant<SF>{},
        color,
        buffer[y * width + x]
        );
}

//-------------------------------------------------------------------------------------------------
// Get SoA rgba color from RGB32F color buffer, let alpha = 1.0
//

template <typename T>
VSNRAY_FUNC
inline void get(
        pixel_format_constant<PF_RGBA32F>   /* dst format */,
        pixel_format_constant<PF_RGB32F>    /* src format */,
        int                                 x,
        int                                 y,
        int                                 width,
        int                                 height,
        vector<4, simd::float4>&            color,
        vector<3, T> const*                 buffer
        )
{
    using OutputColor = vector<3, T>;

    array<OutputColor, 4> out;

    out[0] = ( x      < width &&  y      < height) ? buffer[ y      * width +  x     ] : OutputColor();
    out[1] = ((x + 1) < width &&  y      < height) ? buffer[ y      * width + (x + 1)] : OutputColor();
    out[2] = ( x      < width && (y + 1) < height) ? buffer[(y + 1) * width +  x     ] : OutputColor();
    out[3] = ((x + 1) < width && (y + 1) < height) ? buffer[(y + 1) * width + (x + 1)] : OutputColor();

    color = vector<4, simd::float4>(simd::pack(out), simd::float4(1.0f));
}

//-------------------------------------------------------------------------------------------------
// Get SoA rgba color from RGBA32F color buffer, no conversion necessary
//

template <typename OutputColor>
VSNRAY_FUNC
inline void get(
        pixel_format_constant<PF_RGBA32F>   /* dst format */,
        pixel_format_constant<PF_RGBA32F>   /* src format */,
        int                                 x,
        int                                 y,
        int                                 width,
        int                                 height,
        vector<4, simd::float4>&            color,
        OutputColor const*                  buffer
        )
{
    array<OutputColor, 4> out;

    out[0] = ( x      < width &&  y      < height) ? buffer[ y      * width +  x     ] : OutputColor();
    out[1] = ((x + 1) < width &&  y      < height) ? buffer[ y      * width + (x + 1)] : OutputColor();
    out[2] = ( x      < width && (y + 1) < height) ? buffer[(y + 1) * width +  x     ] : OutputColor();
    out[3] = ((x + 1) < width && (y + 1) < height) ? buffer[(y + 1) * width + (x + 1)] : OutputColor();

    color = simd::pack(out);
}

//-------------------------------------------------------------------------------------------------
// Get SoA simd vector from scalar buffer
//

template <typename T>
VSNRAY_FUNC
inline void get(
        pixel_format_constant<PF_R32F>  /* dst format */,
        pixel_format_constant<PF_R32F>  /* src format */,
        int                             x,
        int                             y,
        int                             width,
        int                             height,
        simd::float4&                   result,
        T const*                        buffer
        )
{
    simd::aligned_array_t<simd::float4> out;

    out[0] = ( x      < width &&  y      < height) ? buffer[ y      * width +  x     ] : T();
    out[1] = ((x + 1) < width &&  y      < height) ? buffer[ y      * width + (x + 1)] : T();
    out[2] = ( x      < width && (y + 1) < height) ? buffer[(y + 1) * width +  x     ] : T();
    out[3] = ((x + 1) < width && (y + 1) < height) ? buffer[(y + 1) * width + (x + 1)] : T();

    result = simd::float4(out);
}

// TODO: merge w/ overload(s) from above

template <typename T>
VSNRAY_FUNC
inline void get(
        pixel_format_constant<PF_DEPTH32F>  /* dst format */,
        pixel_format_constant<PF_DEPTH32F>  /* src format */,
        int                                 x,
        int                                 y,
        int                                 width,
        int                                 height,
        simd::float4&                       result,
        T const*                            buffer
        )
{
    simd::aligned_array_t<simd::float4> out;

    out[0] = ( x      < width &&  y      < height) ? buffer[ y      * width +  x     ] : T();
    out[1] = ((x + 1) < width &&  y      < height) ? buffer[ y      * width + (x + 1)] : T();
    out[2] = ( x      < width && (y + 1) < height) ? buffer[(y + 1) * width +  x     ] : T();
    out[3] = ((x + 1) < width && (y + 1) < height) ? buffer[(y + 1) * width + (x + 1)] : T();

    result = simd::float4(out);
}

// TODO: merge w/ overload(s) from above

template <typename T>
VSNRAY_FUNC
inline void get(
        pixel_format_constant<PF_DEPTH24_STENCIL8>  /* dst format */,
        pixel_format_constant<PF_DEPTH24_STENCIL8>  /* src format */,
        int                                         x,
        int                                         y,
        int                                         width,
        int                                         height,
        simd::int4&                                 result,
        T const*                                    buffer
        )
{
    simd::aligned_array_t<simd::int4> out;

    out[0] = ( x      < width &&  y      < height) ? buffer[ y      * width +  x     ] : T();
    out[1] = ((x + 1) < width &&  y      < height) ? buffer[ y      * width + (x + 1)] : T();
    out[2] = ( x      < width && (y + 1) < height) ? buffer[(y + 1) * width +  x     ] : T();
    out[3] = ((x + 1) < width && (y + 1) < height) ? buffer[(y + 1) * width + (x + 1)] : T();

    result = simd::int4(out);
}


//-------------------------------------------------------------------------------------------------
// Get AVX rgba color from output color buffer, apply conversion
// OutputColor must be rgba (TODO: ensure through source pixel format!)
//

template <pixel_format DF, pixel_format SF, typename OutputColor>
VSNRAY_FUNC
inline void get(
        pixel_format_constant<DF>   /* dst format */,
        pixel_format_constant<SF>   /* src format */,
        int                         x,
        int                         y,
        int                         width,
        int                         height,
        vector<4, simd::float8>&    color,
        OutputColor const*          buffer
        )
{
    const int w = packet_size<simd::float8>::w;
    const int h = packet_size<simd::float8>::h;

    simd::aligned_array_t<simd::float8> xs;
    simd::aligned_array_t<simd::float8> ys;
    simd::aligned_array_t<simd::float8> zs;
    simd::aligned_array_t<simd::float8> ws;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                int idx = row * w + col;
                xs[idx] = buffer[(y + row) * width + (x + col)].x;
                ys[idx] = buffer[(y + row) * width + (x + col)].y;
                zs[idx] = buffer[(y + row) * width + (x + col)].z;
                ws[idx] = buffer[(y + row) * width + (x + col)].w;
            }
        }
    }

    color.x = simd::float8(xs);
    color.y = simd::float8(ys);
    color.z = simd::float8(zs);
    color.w = simd::float8(ws);
}

//-------------------------------------------------------------------------------------------------
// Get AVX simd vector from scalar buffer
// TODO: consolidate w/ float4 version
//

template <typename T>
VSNRAY_FUNC
inline void get(
        pixel_format_constant<PF_R32F>  /* dst format */,
        pixel_format_constant<PF_R32F>  /* src format */,
        int                             x,
        int                             y,
        int                             width,
        int                             height,
        simd::float8&                   result,
        T const*                        buffer
        )
{
    const int w = packet_size<simd::float8>::w;
    const int h = packet_size<simd::float8>::h;

    simd::aligned_array_t<simd::float8> out;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                int idx = row * w + col;
                out[idx] = buffer[(y + row) * width + (x + col)];
            }
        }
    }

    result = simd::float8(out);
}

template <typename T>
VSNRAY_FUNC
inline void get(
        pixel_format_constant<PF_DEPTH32F>  /* dst format */,
        pixel_format_constant<PF_DEPTH32F>  /* src format */,
        int                                 x,
        int                                 y,
        int                                 width,
        int                                 height,
        simd::float8&                       result,
        T const*                            buffer
        )
{
    const int w = packet_size<simd::float8>::w;
    const int h = packet_size<simd::float8>::h;

    simd::aligned_array_t<simd::float8> out;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                int idx = row * w + col;
                out[idx] = buffer[(y + row) * width + (x + col)];
            }
        }
    }

    result = simd::float8(out);
}

// TODO: merge w/ overload(s) from above
template <typename T>
VSNRAY_FUNC
inline void get(
        pixel_format_constant<PF_DEPTH24_STENCIL8>  /* dst format */,
        pixel_format_constant<PF_DEPTH24_STENCIL8>  /* src format */,
        int                                         x,
        int                                         y,
        int                                         width,
        int                                         height,
        simd::int8&                                 result,
        T const*                                    buffer
        )
{
    const int w = packet_size<simd::float8>::w;
    const int h = packet_size<simd::float8>::h;

    simd::aligned_array_t<simd::int8> out;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                int idx = row * w + col;
                out[idx] = buffer[(y + row) * width + (x + col)];
            }
        }
    }

    result = simd::int8(out);
}


//-------------------------------------------------------------------------------------------------
// Get AVX-512 rgba color from output color buffer, apply conversion
// OutputColor must be rgba (TODO: ensure through source pixel format!)
//

template <pixel_format DF, pixel_format SF, typename OutputColor>
VSNRAY_FUNC
inline void get(
        pixel_format_constant<DF>   /* dst format */,
        pixel_format_constant<SF>   /* src format */,
        int                         x,
        int                         y,
        int                         width,
        int                         height,
        vector<4, simd::float16>&   color,
        OutputColor const*          buffer
        )
{
    const int w = packet_size<simd::float16>::w;
    const int h = packet_size<simd::float16>::h;

    simd::aligned_array_t<simd::float16> xs;
    simd::aligned_array_t<simd::float16> ys;
    simd::aligned_array_t<simd::float16> zs;
    simd::aligned_array_t<simd::float16> ws;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                int idx = row * w + col;
                xs[idx] = buffer[(y + row) * width + (x + col)].x;
                ys[idx] = buffer[(y + row) * width + (x + col)].y;
                zs[idx] = buffer[(y + row) * width + (x + col)].z;
                ws[idx] = buffer[(y + row) * width + (x + col)].w;
            }
        }
    }

    color.x = simd::float16(xs);
    color.y = simd::float16(ys);
    color.z = simd::float16(zs);
    color.w = simd::float16(ws);
}

//-------------------------------------------------------------------------------------------------
// Get AVX-512 simd vector from scalar buffer
// TODO: consolidate w/ float4 version
//

template <typename T>
VSNRAY_FUNC
inline void get(
        pixel_format_constant<PF_R32F>  /* dst format */,
        pixel_format_constant<PF_R32F>  /* src format */,
        int                             x,
        int                             y,
        int                             width,
        int                             height,
        simd::float16&                  result,
        T const*                        buffer
        )
{
    const int w = packet_size<simd::float16>::w;
    const int h = packet_size<simd::float16>::h;

    simd::aligned_array_t<simd::float16> out;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                int idx = row * w + col;
                out[idx] = buffer[(y + row) * width + (x + col)];
            }
        }
    }

    result = simd::float16(out);
}

template <typename T>
VSNRAY_FUNC
inline void get(
        pixel_format_constant<PF_DEPTH32F>  /* dst format */,
        pixel_format_constant<PF_DEPTH32F>  /* src format */,
        int                                 x,
        int                                 y,
        int                                 width,
        int                                 height,
        simd::float16&                      result,
        T const*                            buffer
        )
{
    const int w = packet_size<simd::float16>::w;
    const int h = packet_size<simd::float16>::h;

    simd::aligned_array_t<simd::float16> out;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                int idx = row * w + col;
                out[idx] = buffer[(y + row) * width + (x + col)];
            }
        }
    }

    result = simd::float16(out);
}

// TODO: merge w/ overload(s) from above
template <typename T>
VSNRAY_FUNC
inline void get(
        pixel_format_constant<PF_DEPTH24_STENCIL8>  /* dst format */,
        pixel_format_constant<PF_DEPTH24_STENCIL8>  /* src format */,
        int                                         x,
        int                                         y,
        int                                         width,
        int                                         height,
        simd::int16&                                result,
        T const*                                    buffer
        )
{
    const int w = packet_size<simd::float16>::w;
    const int h = packet_size<simd::float16>::h;

    simd::aligned_array_t<simd::int16> out;

    for (int row = 0; row < h; ++row)
    {
        for (int col = 0; col < w; ++col)
        {
            if (x + col < width && y + row < height)
            {
                int idx = row * w + col;
                out[idx] = buffer[(y + row) * width + (x + col)];
            }
        }
    }

    result = simd::int16(out);
}

// Blend ------------------------------------------------------------------

template <typename T, typename U>
VSNRAY_FUNC
inline T blend(T const& src, U const& sfactor, T const& dst, U const& dfactor)
{
    return src * T(sfactor) + dst * T(dfactor);
}

template <typename T>
VSNRAY_FUNC
inline T blend(
        T const& src,
        blending::scale_factor const& sfactor,
        T const& dst,
        blending::scale_factor const& dfactor
        )
{
    auto make_factor = [](T const& src, T const& dst, blending::scale_factor const& factor)
    {
        if (factor == blending::Zero)
        {
            return T(0.0);
        }
        else if (factor == blending::One)
        {
            return T(1.0);
        }
        else if (factor == blending::SrcColor)
        {
            return src;
        }
        else if (factor == blending::OneMinusSrcColor)
        {
            return T(1.0) - src;
        }
        else if (factor == blending::DstColor)
        {
            return dst;
        }
        else if (factor == blending::OneMinusDstColor)
        {
            return T(1.0) - dst;
        }
        else if (factor == blending::SrcAlpha)
        {
            return T(src.w);
        }
        else if (factor == blending::OneMinusSrcAlpha)
        {
            return T(1.0) - T(src.w);
        }
        else if (factor == blending::DstAlpha)
        {
            return T(dst.w);
        }
        else if (factor == blending::OneMinusDstAlpha)
        {
            return T(1.0) - T(src.w);
        }

        return T(0.0);
    };

    T s = make_factor(src, dst, sfactor);
    T d = make_factor(src, dst, dfactor);

    return src * s + dst * d;
}

//-------------------------------------------------------------------------------------------------
// Blend input and output colors, store in output buffer
//

template <pixel_format DF, pixel_format SF, typename InputColor, typename OutputColor, typename T>
VSNRAY_FUNC
inline void blend(
        pixel_format_constant<DF>   /* dst format */,
        pixel_format_constant<SF>   /* src format */,
        int                         x,
        int                         y,
        int                         width,
        int                         height,
        InputColor const&           color,
        OutputColor*                buffer,
        T const&                    sfactor,
        T const&                    dfactor
        )
{
    InputColor dst;

    get(pixel_format_constant<SF>{}, pixel_format_constant<DF>{}, x, y, width, height, dst, buffer);

    dst = blend(color, sfactor, dst, dfactor);

    store(pixel_format_constant<DF>{}, pixel_format_constant<SF>{}, x, y, width, height, dst, buffer);
}


//-------------------------------------------------------------------------------------------------
// Blend color from result record on top of output color buffer
//

template <pixel_format DF, pixel_format SF, typename S, typename OutputColor, typename T>
VSNRAY_FUNC
inline void blend(
        pixel_format_constant<DF>   /* dst format */,
        pixel_format_constant<SF>   /* src format */,
        int                         x,
        int                         y,
        int                         width,
        int                         height,
        result_record<S> const&     rr,
        OutputColor*                color_buffer,
        T const&                    sfactor,
        T const&                    dfactor
        )
{
    blend(
        pixel_format_constant<DF>{},
        pixel_format_constant<SF>{},
        x,
        y,
        width,
        height,
        rr.color,
        color_buffer,
        sfactor,
        dfactor
        );
}

} // pixel_access

} // detail
} // visionaray

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif // VSNRAY_DETAIL_PIXEL_ACCESS_H
