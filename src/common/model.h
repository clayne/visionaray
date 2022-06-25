// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#ifndef VSNRAY_COMMON_MODEL_H
#define VSNRAY_COMMON_MODEL_H 1

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <visionaray/math/forward.h>
#include <visionaray/math/triangle.h>
#include <visionaray/math/unorm.h>
#include <visionaray/math/vector.h>
#include <visionaray/texture/texture.h>
#include <visionaray/aligned_vector.h>

#include "sg/material.h"
#include "file_base.h"

namespace visionaray
{

namespace sg
{
class node;
} // sg

class model : public file_base
{
public:

    using material_type     = sg::obj_material;

    using triangle_type     = basic_triangle<3, float>;
    using normal_type       = vector<3, float>;
    using tex_coord_type    = vector<2, float>;
    using color_type        = vector<3, float>;
    using texture_type      = texture<vector<4, unorm<8>>, 2>;

    using triangle_list     = aligned_vector<triangle_type>;
    using normal_list       = aligned_vector<normal_type>;
    using tex_coord_list    = aligned_vector<tex_coord_type>;
    using color_list        = aligned_vector<color_type>;
    using mat_list          = aligned_vector<material_type>;
    using tex_map           = std::map<std::string, texture_type>;
    using tex_list          = aligned_vector<typename texture_type::ref_type>;

public:

    model();

    // Load single file
    bool load(std::string const& filename);

    // Save to file
    bool save(std::string const& filename, save_options const& options);

    // Load multiple files at once
    bool load(std::vector<std::string> const& filenames);

public:

    // Scene graph
    std::shared_ptr<sg::node> scene_graph = nullptr;

    // These lists will be filled if the file format is so simple
    // that no scene graph is required (i.e. scene_graph == nullptr)
    triangle_list   primitives;
    normal_list     shading_normals;
    normal_list     geometric_normals;
    tex_coord_list  tex_coords;
    color_list      colors;
    mat_list        materials;
    tex_map         texture_map;
    tex_list        textures;
    aabb            bbox;
};

} // visionaray

#endif // VSNRAY_COMMON_MODEL_H
