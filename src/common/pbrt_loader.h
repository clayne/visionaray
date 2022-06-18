// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#ifndef VSNRAY_COMMON_PBRT_LOADER_H
#define VSNRAY_COMMON_PBRT_LOADER_H 1

#include <string>
#include <vector>

#include "file_base.h"

namespace visionaray
{

class model;

void load_pbrt(std::string const& filename, model& mod);
//void save_pbrt(std::string const& filename, model const& mod, file_base::save_options const& options);
void load_pbrt(std::vector<std::string> const& filenames, model& mod);

} // visionaray

#endif // VSNRAY_COMMON_PBRT_LOADER_H
