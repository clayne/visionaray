// This file is distributed under the MIT license.
// See the LICENSE file for details.

#include <visionaray/kernels.h> // make_kernel_params()

#include "render.h"

namespace visionaray
{

void render_plastic_cu(
        cuda_index_bvh<basic_triangle<3, float>>&         bvh,
        thrust::device_vector<vec3> const&                geometric_normals,
        thrust::device_vector<vec3> const&                shading_normals,
        thrust::device_vector<vec2> const&                tex_coords,
        thrust::device_vector<plastic_t> const&           materials,
        thrust::device_vector<cuda_texture_t> const&      textures,
        aligned_vector<point_light<float>> const&         host_lights,
        unsigned                                          bounces,
        float                                             epsilon,
        vec4                                              bgcolor,
        vec4                                              ambient,
        host_device_rt&                                   rt,
        cuda_sched<ray_type_gpu>&                         sched,
        camera_t const&                                   cam,
        unsigned&                                         frame_num,
        algorithm                                         algo,
        unsigned                                          ssaa_samples
        )
{
    using bvh_ref = cuda_index_bvh<basic_triangle<3, float>>::bvh_ref;

    thrust::device_vector<bvh_ref> primitives;

    primitives.push_back(bvh.ref());

    thrust::device_vector<point_light<float>> device_lights = host_lights;

    auto kparams = make_kernel_params(
            normals_per_vertex_binding{},
            thrust::raw_pointer_cast(primitives.data()),
            thrust::raw_pointer_cast(primitives.data()) + primitives.size(),
            thrust::raw_pointer_cast(geometric_normals.data()),
            thrust::raw_pointer_cast(shading_normals.data()),
            thrust::raw_pointer_cast(tex_coords.data()),
            thrust::raw_pointer_cast(materials.data()),
            thrust::raw_pointer_cast(textures.data()),
            thrust::raw_pointer_cast(device_lights.data()),
            thrust::raw_pointer_cast(device_lights.data()) + device_lights.size(),
            bounces,
            epsilon,
            bgcolor,
            ambient
            );

    call_kernel( algo, sched, kparams, frame_num, ssaa_samples, cam, rt );
}

} // visionaray
