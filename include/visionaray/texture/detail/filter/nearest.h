// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#ifndef VSNRAY_TEXTURE_DETAIL_FILTER_NEAREST_H
#define VSNRAY_TEXTURE_DETAIL_FILTER_NEAREST_H 1

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
    typename TexSize
    >
inline ReturnT nearest(
        ReturnT                                 /* */,
        InternalT                               /* */,
        TexelT const*                           tex,
        vector<1, FloatT>                       coord,
        TexSize                                 texsize,
        std::array<tex_address_mode, 1> const&  address_mode
        )
{
    coord[0] = map_tex_coord(coord[0], texsize[0], address_mode);

    auto lo = convert_to_int(coord[0] * FloatT(texsize[0]));
    return point(tex, lo, ReturnT{});
}


//-------------------------------------------------------------------------------------------------
// 2D
//

template <
    typename ReturnT,
    typename InternalT,
    typename TexelT,
    typename FloatT,
    typename TexSize
    >
inline ReturnT nearest(
        ReturnT                                 /* */,
        InternalT                               /* */,
        TexelT const*                           tex,
        vector<2, FloatT>                       coord,
        TexSize                                 texsize,
        std::array<tex_address_mode, 2> const&  address_mode
        )
{
    coord = map_tex_coord(coord, texsize, address_mode);

    auto lo = convert_to_int(coord * vector<2, FloatT>(texsize));

    auto idx = linear_index(lo[0], lo[1], texsize);
    return point(tex, idx, ReturnT{});
}


//-------------------------------------------------------------------------------------------------
// 3D
//

template <
    typename ReturnT,
    typename InternalT,
    typename TexelT,
    typename FloatT,
    typename TexSize
    >
inline ReturnT nearest(
        ReturnT                                 /* */,
        InternalT                               /* */,
        TexelT const*                           tex,
        vector<3, FloatT>                       coord,
        TexSize                                 texsize,
        std::array<tex_address_mode, 3> const&  address_mode
        )
{
    coord = map_tex_coord(coord, texsize, address_mode);

    auto lo = convert_to_int(coord * vector<3, FloatT>(texsize));

    auto idx = linear_index(lo[0], lo[1], lo[2], texsize);
    return point(tex, idx, ReturnT{});
}

} // detail
} // visionaray

#endif // VSNRAY_TEXTURE_DETAIL_FILTER_NEAREST_H
