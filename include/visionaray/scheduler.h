// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#ifndef VSNRAY_SCHEDULER_H
#define VSNRAY_SCHEDULER_H 1

#include <type_traits>
#include <utility>

#include "detail/sched_common.h"
#include "math/forward.h"
#include "math/matrix.h"
#include "matrix_camera.h"

namespace visionaray
{

//-------------------------------------------------------------------------------------------------
// Base classes for scheduler params
//

struct sched_params_base
{
};

template <typename Intersector>
struct sched_params_intersector_base : sched_params_base
{
    using has_intersector = void;

    sched_params_intersector_base(Intersector& i)
        : sched_params_base()
        , intersector(i)
    {
    }

    Intersector& intersector;
};


//-------------------------------------------------------------------------------------------------
// Param structs for different pixel sampling strategies
//

template <typename... Args>
struct sched_params;

template <typename Base, typename Camera, typename RT, typename PxSamplerT>
struct sched_params<Base, Camera, RT, PxSamplerT> : Base
{
    using camera_type           = Camera;
    using rt_type               = RT;
    using pixel_sampler_type    = PxSamplerT;

    template <typename ...Args>
    sched_params(Camera const& c, RT& r, PxSamplerT sp, Args&&... args)
        : Base( std::forward<Args>(args)... )
        , cam(c)
        , rt(r)
        , sample_params(sp)
    {
    }

    Camera cam;
    RT& rt;
    PxSamplerT sample_params;
};


//-------------------------------------------------------------------------------------------------
// Deduce sched params type from members
//

namespace detail
{

template <typename SP>
class sched_params_has_intersector
{
private:

    template <typename U>
    static std::true_type  test(typename U::has_intersector*);

    template <typename U>
    static std::false_type test(...);

public:

    using type = decltype( test<typename std::decay<SP>::type>(nullptr) );

};

} // detail


//-------------------------------------------------------------------------------------------------
// Sched params factory
//

template <
    typename PxSamplerT,
    typename = typename std::enable_if<std::is_base_of<pixel_sampler::base_type, PxSamplerT>::value>::type,
    typename Camera,
    typename RT
    >
auto make_sched_params(
        PxSamplerT    sample_params,
        Camera const& cam,
        RT&           rt
        )
    -> sched_params<sched_params_base, Camera, RT, PxSamplerT>
{
    return { cam, rt, sample_params };
}

template <
    typename PxSamplerT,
    typename = typename std::enable_if<std::is_base_of<pixel_sampler::base_type, PxSamplerT>::value>::type,
    typename Camera,
    typename RT,
    typename Intersector
    >
auto make_sched_params(
        PxSamplerT    sample_params,
        Camera const& cam,
        RT&           rt,
        Intersector&  isect
        )
    -> sched_params<sched_params_intersector_base<Intersector>, Camera, RT, PxSamplerT>
{
    return { cam, rt, sample_params, isect };
}

template <
    typename PxSamplerT,
    typename = typename std::enable_if<std::is_base_of<pixel_sampler::base_type, PxSamplerT>::value>::type,
    typename RT
    >
auto make_sched_params(
        PxSamplerT sample_params,
        mat4       view_matrix,
        mat4       proj_matrix,
        RT&        rt
        )
    -> sched_params<sched_params_base, matrix_camera, RT, PxSamplerT>
{
    return {
        matrix_camera(view_matrix, proj_matrix),
        rt,
        sample_params
        };
}

template <
    typename PxSamplerT,
    typename = typename std::enable_if<std::is_base_of<pixel_sampler::base_type, PxSamplerT>::value>::type,
    typename RT,
    typename Intersector
    >
auto make_sched_params(
        PxSamplerT   sample_params,
        mat4         view_matrix,
        mat4         proj_matrix,
        RT&          rt,
        Intersector& isect
        )
    -> sched_params<sched_params_intersector_base<Intersector>, matrix_camera, RT, PxSamplerT>
{
    return {
        matrix_camera(view_matrix, proj_matrix),
        rt,
        sample_params,
        isect
        };
}


//-------------------------------------------------------------------------------------------------
// Convenience overload with default pixel sampler being uniform_type
//

template <
    typename First,
    typename = typename std::enable_if<!std::is_base_of<pixel_sampler::base_type, First>::value>::type,
    typename ...Args
    >
auto make_sched_params(First first, Args&&... args)
    -> decltype(make_sched_params(pixel_sampler::uniform_type{}, first, std::forward<Args>(args)...))
{
    return make_sched_params(pixel_sampler::uniform_type{}, first, std::forward<Args>(args)...);
}

} // visionaray

#ifdef __CUDACC__
#include "detail/cuda_sched.h"
#endif
#include "detail/simple_sched.h"
#if !defined(__MINGW32__) && !defined(__MINGW64__)
#include "detail/tiled_sched.h"
#endif
#if VSNRAY_HAVE_TBB
#include "detail/tbb_sched.h"
#endif

#endif // VSNRAY_SCHEDULER_H
