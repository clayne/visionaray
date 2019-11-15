// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#ifndef VSNRAY_TEXTURE_DETAIL_FILTER_LINEAR_H
#define VSNRAY_TEXTURE_DETAIL_FILTER_LINEAR_H 1

#include <array>

#include <visionaray/math/detail/math.h>
#include <visionaray/math/vector.h>

#include "common.h"

namespace visionaray
{
namespace detail
{

//-------------------------------------------------------------------------------------------------
// 1D
//

template <
    typename ReturnT,
    typename InternalT,
    typename TexelT,
    typename FloatT,
    typename SizeT
    >
inline ReturnT linear(
        ReturnT                                 /* */,
        InternalT                               /* */,
        TexelT const*                           tex,
        FloatT const&                           coord,
        SizeT const&                            texsize,
        std::array<tex_address_mode, 1> const&  address_mode
        )
{
    auto coord1 = map_tex_coord(
            coord - FloatT(0.5) / FloatT(texsize),
            texsize,
            address_mode
            );

    auto coord2 = map_tex_coord(
            coord + FloatT(0.5) / FloatT(texsize),
            texsize,
            address_mode
            );

    auto lo = min(convert_to_int(coord1 * FloatT(texsize)), texsize - SizeT(1));
    auto hi = min(convert_to_int(coord2 * FloatT(texsize)), texsize - SizeT(1));

    InternalT samples[2] =
    {
        InternalT( point(tex, lo, ReturnT{}) ),
        InternalT( point(tex, hi, ReturnT{}) )
    };

    auto u = coord1 * FloatT(texsize) - FloatT(lo);

    return ReturnT(lerp(samples[0], samples[1], u));
}


//-------------------------------------------------------------------------------------------------
// 2D
//

template <
    typename ReturnT,
    typename InternalT,
    typename TexelT,
    typename FloatT,
    typename SizeT
    >
inline ReturnT linear(
        ReturnT                                 /* */,
        InternalT                               /* */,
        TexelT const*                           tex,
        vector<2, FloatT> const&                coord,
        vector<2, SizeT>  const&                texsize,
        std::array<tex_address_mode, 2> const&  address_mode
        )
{
    auto coord1 = map_tex_coord(
            coord - FloatT(0.5) / vector<2, FloatT>(texsize),
            texsize,
            address_mode
            );

    auto coord2 = map_tex_coord(
            coord + FloatT(0.5) / vector<2, FloatT>(texsize),
            texsize,
            address_mode
            );

    auto lo = min(convert_to_int(coord1 * vector<2, FloatT>(texsize)), texsize - SizeT(1));
    auto hi = min(convert_to_int(coord2 * vector<2, FloatT>(texsize)), texsize - SizeT(1));

    InternalT samples[4] =
    {
        InternalT( point(tex, index( lo.x, lo.y, texsize ), ReturnT{}) ),
        InternalT( point(tex, index( hi.x, lo.y, texsize ), ReturnT{}) ),
        InternalT( point(tex, index( lo.x, hi.y, texsize ), ReturnT{}) ),
        InternalT( point(tex, index( hi.x, hi.y, texsize ), ReturnT{}) )
    };


    auto uv = coord1 * vector<2, FloatT>(texsize) - vector<2, FloatT>(lo);

    auto p1 = lerp(samples[0], samples[1], uv[0]);
    auto p2 = lerp(samples[2], samples[3], uv[0]);

    return ReturnT(lerp(p1, p2, uv[1]));
}


//-------------------------------------------------------------------------------------------------
// 3D
//

template <
    typename ReturnT,
    typename InternalT,
    typename TexelT,
    typename FloatT,
    typename SizeT
    >
inline ReturnT linear(
        ReturnT                                 /* */,
        InternalT                               /* */,
        TexelT const*                           tex,
        vector<3, FloatT> const&                coord,
        vector<3, SizeT> const&                 texsize,
        std::array<tex_address_mode, 3> const&  address_mode
        )
{
    auto coord1 = map_tex_coord(
            coord - FloatT(0.5) / vector<3, FloatT>(texsize),
            texsize,
            address_mode
            );

    auto coord2 = map_tex_coord(
            coord + FloatT(0.5) / vector<3, FloatT>(texsize),
            texsize,
            address_mode
            );

    auto lo = min(convert_to_int(coord1 * vector<3, FloatT>(texsize)), texsize - SizeT(1));
    auto hi = min(convert_to_int(coord2 * vector<3, FloatT>(texsize)), texsize - SizeT(1));

    InternalT samples[8] =
    {
        InternalT( point(tex, index( lo.x, lo.y, lo.z, texsize ), ReturnT{}) ),
        InternalT( point(tex, index( hi.x, lo.y, lo.z, texsize ), ReturnT{}) ),
        InternalT( point(tex, index( lo.x, hi.y, lo.z, texsize ), ReturnT{}) ),
        InternalT( point(tex, index( hi.x, hi.y, lo.z, texsize ), ReturnT{}) ),
        InternalT( point(tex, index( lo.x, lo.y, hi.z, texsize ), ReturnT{}) ),
        InternalT( point(tex, index( hi.x, lo.y, hi.z, texsize ), ReturnT{}) ),
        InternalT( point(tex, index( lo.x, hi.y, hi.z, texsize ), ReturnT{}) ),
        InternalT( point(tex, index( hi.x, hi.y, hi.z, texsize ), ReturnT{}) )
    };


    auto uvw = coord1 * vector<3, FloatT>(texsize) - vector<3, FloatT>(lo);

    auto p1  = lerp(samples[0], samples[1], uvw[0]);
    auto p2  = lerp(samples[2], samples[3], uvw[0]);
    auto p3  = lerp(samples[4], samples[5], uvw[0]);
    auto p4  = lerp(samples[6], samples[7], uvw[0]);

    auto p12 = lerp(p1, p2, uvw[1]);
    auto p34 = lerp(p3, p4, uvw[1]);

    return ReturnT(lerp(p12, p34, uvw[2]));
}

} // detail
} // visionaray

#endif // VSNRAY_TEXTURE_DETAIL_FILTER_LINEAR_H
