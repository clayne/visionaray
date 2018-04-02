// This file is distributed under the MIT license.
// See the LICENSE file for details.

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>

#if VSNRAY_HAVE_TBB
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>
#else
#include "../parallel_for.h"
#endif

#include "../random_sampler.h"
#include "sched_common.h"

namespace visionaray
{
namespace tiled_sched_impl
{

//-------------------------------------------------------------------------------------------------
// Generate primary ray and sample pixel
//

template <typename R, typename K, typename SP, typename Sampler, typename ...Args>
void call_sample_pixel(
        std::false_type /* has intersector */,
        R               /* */,
        K               kernel,
        SP              sparams,
        Sampler&        samp,
        unsigned        frame_num,
        Args&&...       args
        )
{
    auto r = detail::make_primary_rays(
            R{},
            typename SP::pixel_sampler_type{},
            samp,
            std::forward<Args>(args)...
            );

    sample_pixel(
            kernel,
            typename SP::pixel_sampler_type(),
            r,
            samp,
            frame_num,
            sparams.rt.ref(),
            std::forward<Args>(args)...
            );
}

template <typename R, typename K, typename SP, typename Sampler, typename ...Args>
void call_sample_pixel(
        std::true_type  /* has intersector */,
        R               /* */,
        K               kernel,
        SP              sparams,
        Sampler&        samp,
        unsigned        frame_num,
        Args&&...       args
        )
{
    auto r = detail::make_primary_rays(
            R{},
            typename SP::pixel_sampler_type{},
            samp,
            std::forward<Args>(args)...
            );

    sample_pixel(
            detail::have_intersector_tag(),
            sparams.intersector,
            kernel,
            typename SP::pixel_sampler_type(),
            r,
            samp,
            frame_num,
            sparams.rt.ref(),
            std::forward<Args>(args)...
            );
}

} // tiled_sched_impl


//-------------------------------------------------------------------------------------------------
// tiled_sched implementation
//

template <typename R>
tiled_sched<R>::tiled_sched(unsigned num_threads)
    : pool_(num_threads)
{
}

template <typename R>
template <typename K, typename SP>
void tiled_sched<R>::frame(K kernel, SP sched_params, unsigned frame_num)
{
    sched_params.cam.begin_frame();

    sched_params.rt.begin_frame();

    int pw = packet_size<typename R::scalar_type>::w;
    int ph = packet_size<typename R::scalar_type>::h;

    // Tile size must be be a multiple of packet size.
    int dx = round_up(16, pw);
    int dy = round_up(16, ph);

    int x0 = sched_params.scissor_box.x;
    int y0 = sched_params.scissor_box.y;

    int nx = x0 + sched_params.scissor_box.w;
    int ny = y0 + sched_params.scissor_box.h;

#if VSNRAY_HAVE_TBB
    tbb::parallel_for(
        tbb::blocked_range2d<int>(x0, nx, dx, y0, ny, dy),
        [=](tbb::blocked_range2d<int> const& r)
#else
    parallel_for(
        pool_,
        tiled_range2d<int>(x0, nx, dx, y0, ny, dy),
        [=](range2d<int> const& r)
#endif
        {
            for (int y = r.cols().begin(); y < r.cols().end(); y += ph)
            {
                for (int x = r.rows().begin(); x < r.rows().end(); x += pw)
                {
                    random_sampler<typename R::scalar_type> samp(detail::tic(typename R::scalar_type{}));

                    tiled_sched_impl::call_sample_pixel(
                            typename detail::sched_params_has_intersector<SP>::type(),
                            R{},
                            kernel,
                            sched_params,
                            samp,
                            frame_num,
                            x,
                            y,
                            sched_params.rt.width(),
                            sched_params.rt.height(),
                            sched_params.cam
                            );
                }
            }
        });

    sched_params.rt.end_frame();

    sched_params.cam.end_frame();
}

template <typename R>
void tiled_sched<R>::reset(unsigned num_threads)
{
#if VSNRAY_HAVE_TBB
    pool_ = tbb::task_scheduler_init(num_threads);
#else
    if (pool_.num_threads == num_threads)
    {
        return;
    }

    pool_.reset(num_threads);
#endif
}

} // visionaray
