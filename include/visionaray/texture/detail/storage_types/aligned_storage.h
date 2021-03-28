// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#ifndef VSNRAY_TEXTURE_DETAIL_STORAGE_TYPES_ALIGNED_STORAGE_H
#define VSNRAY_TEXTURE_DETAIL_STORAGE_TYPES_ALIGNED_STORAGE_H 1

#include <algorithm>
#include <array>
#include <cstddef>

#include <visionaray/aligned_vector.h>
#include <visionaray/pixel_format.h>
#include <visionaray/swizzle.h>

namespace visionaray
{

//-------------------------------------------------------------------------------------------------
// Simple linear storage type. Data is aligned to allow for SIMD access
//

template <typename T, unsigned Dim, size_t A = 16>
class aligned_storage
{
public:

    using value_type = T;

public:

    aligned_storage() = default;

    explicit aligned_storage(std::array<unsigned, Dim> size)
        : data_(linear_size(size))
        , size_(size)
    {
    }

    explicit aligned_storage(unsigned w)
    {
        realloc(w);
    }

    explicit aligned_storage(unsigned w, unsigned h)
    {
        realloc(w, h);
    }

    explicit aligned_storage(unsigned w, unsigned h, unsigned d)
    {
        realloc(w, h, d);
    }

    std::array<unsigned, Dim> size() const
    {
        return size_;
    }

    void realloc(unsigned w)
    {
        size_[0] = w;
        data_.resize(linear_size(size_));
    }

    void realloc(unsigned w, unsigned h)
    {
        size_[0] = w;
        size_[1] = h;
        data_.resize(linear_size(size_));
    }

    void realloc(unsigned w, unsigned h, unsigned d)
    {
        size_[0] = w;
        size_[1] = h;
        size_[2] = d;
        data_.resize(linear_size(size_));
    }

    void reset(T const* data)
    {
        std::copy(data, data + data_.size(), data_.begin());
    }

    void reset(
            T const* data,
            pixel_format format,
            pixel_format internal_format
            )
    {
        if (format != internal_format)
        {
            // Swizzle in-place
            aligned_vector<T> tmp(data, data + data_.size());
            swizzle(tmp.data(), internal_format, format, tmp.size());
            reset(tmp.data());
        }
        else
        {
            // Simple copy
            reset(data);
        }
    }

    template <typename U>
    void reset(
            U const* data,
            pixel_format format,
            pixel_format internal_format
            )
    {
        // Copy to temporary array, then swizzle
        aligned_vector<T> dst(data_.size());
        swizzle(dst.data(), internal_format, data, format, dst.size());
        reset(dst.data());
    }

    template <typename U>
    void reset(
            U const* data,
            pixel_format format,
            pixel_format internal_format,
            swizzle_hint hint
            )
    {
        // Copy with temporary array, hint about how to handle alpha
        aligned_vector<T> dst(data_.size());
        swizzle(dst.data(), internal_format, data, format, dst.size(), hint);
        reset(dst.data());
    }

    value_type const* data() const
    {
        return data_.data();
    }

    operator bool() const
    {
        return !data_.empty();
    }

protected:

    inline size_t linear_size(std::array<unsigned, 1> size)
    {
        return size_t(size[0]);
    }

    inline size_t linear_size(std::array<unsigned, 2> size)
    {
        return size[0] * size_t(size[1]);
    }

    inline size_t linear_size(std::array<unsigned, 3> size)
    {
        return size[0] * size[1] * size_t(size[2]);
    }

    aligned_vector<T, A> data_;
    std::array<unsigned, Dim> size_;

};

} // visionaray

#endif // VSNRAY_TEXTURE_DETAIL_STORAGE_TYPES_ALIGNED_STORAGE_H
