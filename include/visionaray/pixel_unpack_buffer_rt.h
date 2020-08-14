// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#ifndef VSNRAY_PIXEL_UNPACK_BUFFER_RT
#define VSNRAY_PIXEL_UNPACK_BUFFER_RT 1

#include <memory>

#include "math/forward.h"
#include "math/vector.h"
#include "detail/macros.h"
#include "pixel_traits.h"
#include "render_target.h"

namespace visionaray
{

template <pixel_format ColorFormat, pixel_format DepthFormat>
class pixel_unpack_buffer_rt : public render_target
{
public:

    using color_type    = typename pixel_traits<ColorFormat>::type;
    using depth_type    = typename pixel_traits<DepthFormat>::type;

    using ref_type      = render_target_ref<ColorFormat, DepthFormat>;

public:

    pixel_unpack_buffer_rt();
   ~pixel_unpack_buffer_rt();

    color_type* color();
    depth_type* depth();

    color_type const* color() const;
    depth_type const* depth() const;

    ref_type ref();

    void clear_color_buffer(vec4 const& color = vec4(0.0f));
    void clear_depth_buffer(float depth = 1.0f);
    void begin_frame();
    void end_frame();
    void resize(int w, int h);
    void display_color_buffer() const;

private:

    struct impl;
    std::unique_ptr<impl> const impl_;

};

} // visionaray

#include "detail/pixel_unpack_buffer_rt.inl"

#endif // VSNRAY_PIXEL_UNPACK_BUFFER_RT
