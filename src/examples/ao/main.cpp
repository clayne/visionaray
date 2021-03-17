// This file is distributed under the MIT license.
// See the LICENSE file for details.

#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

#include <GL/glew.h>

#include <Support/CmdLine.h>
#include <Support/CmdLineUtil.h>

#include <visionaray/detail/platform.h>

#include <visionaray/math/io.h>
#include <visionaray/bvh.h>
#include <visionaray/cpu_buffer_rt.h>
#include <visionaray/get_normal.h>
#include <visionaray/pinhole_camera.h>
#include <visionaray/random_generator.h>
#include <visionaray/sampling.h>
#include <visionaray/scheduler.h>
#include <visionaray/traverse.h>

#include <common/manip/arcball_manipulator.h>
#include <common/manip/pan_manipulator.h>
#include <common/manip/zoom_manipulator.h>

#include <common/model.h>
#include <common/obj_loader.h>
#include <common/viewer_glut.h>

using namespace visionaray;

using viewer_type = viewer_glut;


//-------------------------------------------------------------------------------------------------
// struct with state variables
//

struct renderer : viewer_type
{
    using host_ray_type = basic_ray<simd::float4>;

    renderer()
        : viewer_type(512, 512, "Visionaray Ambient Occlusion Example")
        , host_sched(8)
    {
        using namespace support;

        add_cmdline_option( cl::makeOption<std::string&>(
            cl::Parser<>(),
            "filename",
            cl::Desc("Input file in wavefront obj format"),
            cl::Positional,
            cl::Required,
            cl::init(this->filename)
            ) );

        add_cmdline_option( cl::makeOption<std::string&>(
            cl::Parser<>(),
            "camera",
            cl::Desc("Text file with camera parameters"),
            cl::ArgRequired,
            cl::init(this->initial_camera)
            ) );

        add_cmdline_option( cl::makeOption<bvh_build_strategy&>({
                { "default",            Binned,         "Binned SAH" },
                { "split",              Split,          "Binned SAH with spatial splits" },
                { "lbvh",               LBVH,           "LBVH (CPU)" }
            },
            "bvh",
            cl::Desc("BVH build strategy"),
            cl::ArgRequired,
            cl::init(this->build_strategy)
            ) );

        add_cmdline_option( cl::makeOption<int&>(
            cl::Parser<>(),
            "samples",
            cl::Desc("Number of shadow rays for ambient occlusion"),
            cl::ArgRequired,
            cl::init(this->AO_Samples)
            ) );

        add_cmdline_option( cl::makeOption<float&>(
            cl::Parser<>(),
            "radius",
            cl::Desc("Ambient occlusion radius"),
            cl::ArgRequired,
            cl::init(this->AO_Radius)
            ) );
    }

    enum bvh_build_strategy
    {
        Binned = 0, // Binned SAH builder, no spatial splits
        Split,      // Split BVH, also binned and with SAH
        LBVH,       // LBVH builder on the CPU
    };

    pinhole_camera                              cam;
    cpu_buffer_rt<PF_RGBA32F, PF_UNSPECIFIED>   host_rt;
    tiled_sched<host_ray_type>                  host_sched;
    bvh_build_strategy                          build_strategy  = Binned;

    std::string                                 filename;
    std::string                                 initial_camera;

    model mod;
    index_bvh<model::triangle_type>             host_bvh;
    unsigned                                    frame_num       = 0;

    int                                         AO_Samples      = 8;
    float                                       AO_Radius       = 0.1f;

protected:

    void on_display();
    void on_key_press(visionaray::key_event const& event);
    void on_mouse_move(visionaray::mouse_event const& event);
    void on_resize(int w, int h);

};


//-------------------------------------------------------------------------------------------------
// I/O utility for camera lookat only - not fit for the general case!
//

std::istream& operator>>(std::istream& in, pinhole_camera& cam)
{
    vec3 eye;
    vec3 center;
    vec3 up;

    in >> eye >> std::ws >> center >> std::ws >> up >> std::ws;
    cam.look_at(eye, center, up);

    return in;
}

std::ostream& operator<<(std::ostream& out, pinhole_camera const& cam)
{
    out << cam.eye() << '\n';
    out << cam.center() << '\n';
    out << cam.up() << '\n';
    return out;
}


//-------------------------------------------------------------------------------------------------
// Display function, implements the AO kernel
//

void renderer::on_display()
{
    // some setup

    using R = renderer::host_ray_type;
    using S = R::scalar_type;
    using C = vector<4, S>;
    using V = vector<3, S>;

    float alpha = 1.0f / ++frame_num;
    auto blend_params = pixel_sampler::jittered_blend_type{};
    blend_params.sfactor = alpha;
    blend_params.dfactor = 1.0f - alpha;

    auto sparams = make_sched_params(
            blend_params,
            cam,
            host_rt
            );


    using bvh_ref = index_bvh<model::triangle_type>::bvh_ref;

    std::vector<bvh_ref> bvhs;
    bvhs.push_back(host_bvh.ref());

    auto prims_begin = bvhs.data();
    auto prims_end   = bvhs.data() + bvhs.size();

    auto bgcolor = background_color();

    host_sched.frame([&](R ray, random_generator<S>& gen) -> result_record<S>
    {
        result_record<S> result;
        result.color = C(bgcolor, 1.0f);

        auto hit_rec = closest_hit(
                ray,
                prims_begin,
                prims_end
                );

        result.hit = hit_rec.hit;

        if (any(hit_rec.hit))
        {
            hit_rec.isect_pos = ray.ori + ray.dir * hit_rec.t;
            result.depth  = hit_rec.t;

            C clr(1.0);

            auto n = get_normal(
                mod.geometric_normals.data(),
                hit_rec,
                model::triangle_type{}
                );

            n = faceforward(n, -ray.dir, n);

            V u;
            V v;
            V w = n;
            make_orthonormal_basis(u, v, w);

            S radius= AO_Radius;

            S ao(0.0);

            for (int i = 0; i < AO_Samples; ++i)
            {
                auto sp = cosine_sample_hemisphere(gen.next(), gen.next());

                auto dir = normalize( sp.x * u + sp.y * v + sp.z * w );

                R ao_ray;
                ao_ray.ori = hit_rec.isect_pos + dir * S(1E-3f);
                ao_ray.dir = dir;
                ao_ray.tmin = S(0.0);
                ao_ray.tmax = radius;

                auto ao_rec = any_hit(
                        ao_ray,
                        prims_begin,
                        prims_end
                        );

                ao = select(
                        ao_rec.hit,
                        ao + S(1.0f / AO_Samples),
                        ao
                        );
            }

            ao = S(1.0) - ao;
            result.color = select( hit_rec.hit, C(ao, ao, ao, S(1.0)), result.color );

        }

        return result;
    }, sparams);


    // display the rendered image

    glClearColor(bgcolor.x, bgcolor.y, bgcolor.z, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    host_rt.display_color_buffer();
}


//-------------------------------------------------------------------------------------------------
// mouse handling
//

void renderer::on_mouse_move(visionaray::mouse_event const& event)
{
    if (event.buttons() != mouse::NoButton)
    {
        frame_num = 0;
        host_rt.clear_color_buffer();
    }

    viewer_type::on_mouse_move(event);
}


//-------------------------------------------------------------------------------------------------
// keyboard handling
//

void renderer::on_key_press(key_event const& event)
{
    static const std::string camera_filename = "visionaray-camera.txt";

    switch (event.key())
    {
    case 'u':
        {
            std::ofstream file( camera_filename );
            if (file.good())
            {
                std::cout << "Storing camera to file: " << camera_filename << '\n';
                file << cam;
            }
        }
        break;

    case 'v':
        {
            std::ifstream file( camera_filename );
            if (file.good())
            {
                file >> cam;
                host_rt.clear_color_buffer();
                std::cout << "Load camera from file: " << camera_filename << '\n';
            }
        }
        break;

    default:
        break;
    }

    viewer_type::on_key_press(event);
}


//-------------------------------------------------------------------------------------------------
// resize event
//

void renderer::on_resize(int w, int h)
{
    frame_num = 0;
    host_rt.clear_color_buffer();

    cam.set_viewport(0, 0, w, h);
    float aspect = w / static_cast<float>(h);
    cam.perspective(45.0f * constants::degrees_to_radians<float>(), aspect, 0.001f, 1000.0f);
    host_rt.resize(w, h);

    viewer_type::on_resize(w, h);
}


//-------------------------------------------------------------------------------------------------
// Main function, performs initialization
//

int main(int argc, char** argv)
{
    renderer rend;

    try
    {
        rend.init(argc, argv);
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    if (!rend.mod.load(rend.filename))
    {
        std::cerr << "Failed loading obj model\n";
        return EXIT_FAILURE;
    }

    std::cout << "Creating BVH...\n";

    binned_sah_builder builder;
    builder.enable_spatial_splits(rend.build_strategy == renderer::Split);

    rend.host_bvh = builder.build(
            index_bvh<model::triangle_type>{},
            rend.mod.primitives.data(),
            rend.mod.primitives.size()
            );

    std::cout << "Ready\n";

    float aspect = rend.width() / static_cast<float>(rend.height());

    rend.cam.perspective(45.0f * constants::degrees_to_radians<float>(), aspect, 0.001f, 1000.0f);

    // Load camera from file or set view-all
    std::ifstream file(rend.initial_camera);
    if (file.good())
    {
        file >> rend.cam;
    }
    else
    {
        rend.cam.view_all( rend.mod.bbox );
    }

    rend.add_manipulator( std::make_shared<arcball_manipulator>(rend.cam, mouse::Left) );
    rend.add_manipulator( std::make_shared<pan_manipulator>(rend.cam, mouse::Middle) );
    // Additional "Alt + LMB" pan manipulator for setups w/o middle mouse button
    rend.add_manipulator( std::make_shared<pan_manipulator>(rend.cam, mouse::Left, keyboard::Alt) );
    rend.add_manipulator( std::make_shared<zoom_manipulator>(rend.cam, mouse::Right) );

    rend.event_loop();
}
