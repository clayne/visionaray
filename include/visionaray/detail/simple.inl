// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#ifndef VSNRAY_DETAIL_SIMPLE_INL
#define VSNRAY_DETAIL_SIMPLE_INL 1

#include <visionaray/get_surface.h>
#include <visionaray/result_record.h>
#include <visionaray/spectrum.h>
#include <visionaray/traverse.h>


namespace visionaray
{
namespace simple
{

template <typename Params>
struct kernel
{

    Params params;

    template <typename Intersector, typename R>
    VSNRAY_FUNC result_record<typename R::scalar_type> operator()(Intersector& isect, R ray) const
    {
        using S = typename R::scalar_type;
        using V = typename result_record<S>::vec_type;
        using C = spectrum<S>;

        result_record<S> result;

        auto hit_rec = closest_hit(ray, params.prims.begin, params.prims.end, isect);

        if (any(hit_rec.hit))
        {
            hit_rec.isect_pos = ray.ori + ray.dir * hit_rec.t;

            auto surf = get_surface(hit_rec, params);
            auto env = params.amb_light.intensity(ray.dir);
            auto bgcolor = params.background.intensity(ray.dir);
            auto ambient = surf.material.ambient() * C(from_rgb(env));
            auto shaded_clr = select( hit_rec.hit, ambient, C(from_rgb(bgcolor)) );
            auto view_dir = -ray.dir;

            for (auto it = params.lights.begin; it != params.lights.end; ++it)
            {
                auto light_dir = normalize( V(it->position()) - hit_rec.isect_pos );

                auto clr = surf.shade(view_dir, light_dir, it->intensity(hit_rec.isect_pos));

                shaded_clr  += select( hit_rec.hit, clr, C(0.0) );
            }

            result.color     = select(hit_rec.hit, to_rgba(shaded_clr), vector<4, S>(bgcolor, S(1.0)));
            result.isect_pos = hit_rec.isect_pos;
        }
        else
        {
            result.color = vector<4, S>(params.background.intensity(ray.dir), S(1.0));
        }

        result.hit = hit_rec.hit;
        return result;
    }

    template <typename R>
    VSNRAY_FUNC result_record<typename R::scalar_type> operator()(R ray) const
    {
        default_intersector ignore;
        return (*this)(ignore, ray);
    }
};

} // simple
} // visionaray

#endif // VSNRAY_DETAIL_SIMPLE_INL
