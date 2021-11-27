// This file is distributed under the MIT license.
// See the LICENSE file for details.

#include <GL/glew.h>

#include <visionaray/math/forward.h>
#include <visionaray/math/project.h>
#include <visionaray/math/quaternion.h>
#include <visionaray/pinhole_camera.h>

#include "rotate_manipulator.h"

using namespace visionaray;

namespace circle
{

//-------------------------------------------------------------------------------------------------
// circle in the plane
//

template <typename F>
static void for_each_line(F func, float radius = 1.0f, vec2 center = { 0.0f, 0.0f } )
{
    for (int i = 0; i < 360; ++i)
    {
        auto rad1 = i * constants::degrees_to_radians<float>();
        auto x1 = center.x + cos(rad1) * radius;
        auto y1 = center.y + sin(rad1) * radius;

        auto rad2 = (i + 1) * constants::degrees_to_radians<float>();
        auto x2 = center.x + cos(rad2) * radius;
        auto y2 = center.y + sin(rad2) * radius;

        func(x1, y1, x2, y2);
    }
}


//-------------------------------------------------------------------------------------------------
// circle in space
//

template <typename F>
static void for_each_line(F func, vec3 vx, vec3 vy, vec3 center = { 0.0f, 0.0f, 0.0f } )
{
    for (int i = 0; i < 360; ++i)
    {
        auto rad1 = i * constants::degrees_to_radians<float>();
        auto v1 = center + cos(rad1) * vx + sin(rad1) * vy;

        auto rad2 = (i + 1) * constants::degrees_to_radians<float>();
        auto v2 = center + cos(rad2) * vx + sin(rad2) * vy;

        func(v1, v2);
    }
}

} // circle


//-------------------------------------------------------------------------------------------------
// Convert from Visionaray / GL window coordinates to left/upper origin coordinates
//

static disci flip(disci const& d, recti const& viewport)
{
    disci result = d;
    result.center.y = viewport.h - d.center.y - 1;
    return result;
}

static recti flip(recti const& r, recti const& viewport)
{
    recti result = r;
    result.y = viewport.h - r.h - r.y - 1;
    return result;
}


//-------------------------------------------------------------------------------------------------
//
//

rotate_manipulator::rotate_manipulator(
        pinhole_camera const& cam,
        mat4& model_matrix,
        vec3 size,
        mouse::buttons buttons,
        int line_width
        )
    : model_manipulator(cam, model_matrix, size)
    , buttons_(buttons)
    , ball_(1.1f)
    , line_width_(line_width)
{
}

void rotate_manipulator::render()
{
    glPushAttrib(GL_CURRENT_BIT | GL_TRANSFORM_BIT);

    auto scaling = get_scaling(model_matrix_);
    auto radius = (length((scaling * vec4(size_, 1.0f)).xyz()) / 2.0f) * ball_.radius;
    vec3 center(model_matrix_(0, 3), model_matrix_(1, 3), model_matrix_(2, 3));

    mat3 mm3(model_matrix_(0).xyz(), model_matrix_(1).xyz(), model_matrix_(2).xyz());
    auto X = normalize(mm3 * vec3(1.0f, 0.0f, 0.0f));
    auto Y = normalize(mm3 * vec3(0.0f, 1.0f, 0.0f));
    auto Z = normalize(mm3 * vec3(0.0f, 0.0f, 1.0f));


    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(camera_.get_proj_matrix().data());

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf((camera_.get_view_matrix() * mat4::translation(vec3(center))).data());


    // parallel to screen

    auto dir   = normalize(center - camera_.eye());
    auto right = normalize(cross(dir, vec3(0.0f, 1.0f, 0.0f)));
    auto up    = cross(dir, right);

    GLint prev_line_width = 1;
    glGetIntegerv(GL_LINE_WIDTH, &prev_line_width);

    glLineWidth(line_width_);

    glBegin(GL_LINES);
    glColor3f(0.5f, 0.5f, 0.5f);
    circle::for_each_line([&](vec3 const& v1, vec3 const& v2)
        {
            glVertex3f(v1.x, v1.y, v1.z);
            glVertex3f(v2.x, v2.y, v2.z);
        },
        up * radius,
        right * radius
        );
    glEnd();


    // X

    right      = normalize(cross(dir, X));
    auto front = normalize(cross(right, X));

    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    circle::for_each_line([&](vec3 const& v1, vec3 const& v2)
        {
            if (dot(v2, normalize(camera_.eye() - center)) > 0.0f)
            {
                glVertex3f(v1.x, v1.y, v1.z);
                glVertex3f(v2.x, v2.y, v2.z);
            }
        },
        right * radius,
        front * radius
        );
    glEnd();


    // Y

    right = normalize(cross(dir, Y));
    front = normalize(cross(right, Y));

    glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f);
    circle::for_each_line([&](vec3 const& v1, vec3 const& v2)
        {
            if (dot(v2, normalize(camera_.eye() - center)) > 0.0f)
            {
                glVertex3f(v1.x, v1.y, v1.z);
                glVertex3f(v2.x, v2.y, v2.z);
            }
        },
        right * radius,
        front * radius
        );
    glEnd();


    // Z

    right = normalize(cross(dir, Z));
    front = normalize(cross(right, Z));

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 1.0f);
    circle::for_each_line([&](vec3 const& v1, vec3 const& v2)
        {
            if (dot(v2, normalize(camera_.eye() - center)) > 0.0f)
            {
                glVertex3f(v1.x, v1.y, v1.z);
                glVertex3f(v2.x, v2.y, v2.z);
            }
        },
        right * radius,
        front * radius
        );
    glEnd();


    glLineWidth(prev_line_width);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glPopAttrib();
}

bool rotate_manipulator::handle_mouse_down(visionaray::mouse_event const& event)
{
    auto bdisc = flip( bounding_disc(), camera_.get_viewport() );

    if (bdisc.contains(event.pos()) && event.buttons() & buttons_)
    {
        dragging_ = true;

        auto brect = flip( bounding_rect(), camera_.get_viewport() );

        ball_.down_pos = ball_.project(
            event.pos().x,
            event.pos().y,
            brect
            );
        ball_.down_rotation = ball_.rotation;
        down_pos_ = event.pos();
//      selected_ = select_from_mouse_pointer(event);

        return true;
    }
    else
    {
        return false;
    }
}

bool rotate_manipulator::handle_mouse_up(visionaray::mouse_event const& event)
{
    VSNRAY_UNUSED(event);

    dragging_ = false;
    selected_ = -1;
    return false;
}

bool rotate_manipulator::handle_mouse_move(visionaray::mouse_event const& event)
{
    auto bdisc = flip( bounding_disc(), camera_.get_viewport() );

    mouse_over_ = bdisc.contains(event.pos());

    if (dragging_ && event.buttons() & buttons_)
    {
        auto brect = flip( bounding_rect(), camera_.get_viewport() );

        vec3 curr_pos = ball_.project(
                event.pos().x,
                event.pos().y,
                brect
                );

        auto last_vsr = rotation(ball_.rotation);
        ball_.rotation = quat::rotation(ball_.down_pos, curr_pos) * ball_.down_rotation;

        auto vsr = rotation(ball_.rotation);

        auto trans = model_matrix_(3);

        model_matrix_.col3 = vec4(0.0f, 0.0f, 0.0f, 1.0f);

        auto vm = camera_.get_view_matrix();
        vm.col3 = vec4(0.0f, 0.0f, 0.0f, 1.0f);

        model_matrix_ = inverse(vm) * vsr * inverse(last_vsr) * vm * model_matrix_;
        model_matrix_.col3 = trans;

        return true;
    }
    else
    {
        hovered_ = select_from_mouse_pointer(event);
        return false;
    }
}

int rotate_manipulator::select_from_mouse_pointer(visionaray::mouse_event const& event)
{
    auto scaling = get_scaling(model_matrix_);
    auto radius = length((scaling * vec4(size_, 1.0f)).xyz()) / 2.0f;
    vec2i pos(
            event.pos().x,
            camera_.get_viewport().h - 1 - event.pos().y
            );

    int result = -1;

    for (int i = 0; i < 3; ++i)
    {
        auto modelview = camera_.get_view_matrix() * model_matrix_ * inverse(scaling);

        circle::for_each_line(
            [&](float x1, float y1, float x2, float y2)
            {
                auto v1 = rotate( vec3(0.0f, x1, y1), i );
                auto v2 = rotate( vec3(0.0f, x2, y2), i );

                vec3 win1;
                project(win1, v1, modelview, camera_.get_proj_matrix(), camera_.get_viewport());

                vec3 win2;
                project(win2, v2, modelview, camera_.get_proj_matrix(), camera_.get_viewport());

                auto minx = min(win1.x, win2.x);
                auto maxx = max(win1.x, win2.x);
                auto miny = min(win1.y, win2.y);
                auto maxy = max(win1.y, win2.y);

                minx -= 2;
                miny -= 2;
                maxx += 2;
                maxy += 2;

                recti bounds(
                        static_cast<int>(floor(minx)),
                        static_cast<int>(floor(miny)),
                        static_cast<int>(ceil(abs(maxx - minx))),
                        static_cast<int>(ceil(abs(maxy - miny)))
                        );

                if (bounds.contains(pos))
                {
                    result = i;
                    return;
                }

            },
            radius * ball_.radius
            );
    }

    return result;

}

disci rotate_manipulator::bounding_disc()
{
    auto scaling = get_scaling(model_matrix_);
    auto radius = length((scaling * vec4(size_, 1.0f)).xyz()) / 2.0f;

    vec3 win_center(0.0f);
    vec3 win_right(0.0f);

    project(
            win_center,
            vec3(0.0f),
            camera_.get_view_matrix() * model_matrix_,
            camera_.get_proj_matrix(),
            camera_.get_viewport()
            );

    project(
            win_right,
            vec3(0.0f),
            mat4::translation(radius * ball_.radius, 0.0f, 0.0f) * camera_.get_view_matrix() * model_matrix_,
            camera_.get_proj_matrix(),
            camera_.get_viewport()
            );

    disci result;
    result.center = vec2i(win_center.xy());
    result.radius = round(abs(win_right.x - win_center.x)) + 0;
    return result;
}

recti rotate_manipulator::bounding_rect()
{
    auto disc = bounding_disc();

    return recti(
            disc.center.x - disc.radius,
            disc.center.y - disc.radius,
            disc.radius + disc.radius,
            disc.radius + disc.radius
            );
}
