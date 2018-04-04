/*
* Copyright Disney Enterprises, Inc.  All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License
* and the following modification to it: Section 6 Trademarks.
* deleted and replaced with:
*
* 6. Trademarks. This License does not grant permission to use the
* trade names, trademarks, service marks, or product names of the
* Licensor and its affiliates, except as required for reproducing
* the content of the NOTICE file.
*
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*/

#pragma once

#include <limits>
#include <memory>

namespace png {

template <typename T>
struct GrayPixel {
    typedef T component_type;

    GrayPixel() : value(0)
    {
    }

    GrayPixel(T value_) : value(value_)
    {
    }

    static constexpr size_t stride()
    {
        return 1;
    }
    static void writeHeader(std::ostream& os, int width, int height)
    {
        os << "P5" << std::endl;
        os << width << " " << height << std::endl;
        os << +std::numeric_limits<T>::max() << std::endl;
    }

    T value;
};

template <typename T>
struct RGBPixel {
    typedef T component_type;

    RGBPixel() : r(0), g(0), b(0)
    {
    }

    RGBPixel(T r_, T g_, T b_) : r(r_), g(g_), b(b_)
    {
    }

    static constexpr size_t stride()
    {
        return 3;
    }
    static void writeHeader(std::ostream& os, int width, int height)
    {
        os << "P6" << std::endl;
        os << width << " " << height << std::endl;
        os << +std::numeric_limits<T>::max() << std::endl;
    }

    T r, g, b;
};

template <typename T>
struct RGBAPixel {
    typedef T component_type;

    RGBAPixel() : r(0), g(0), b(0), a(0)
    {
    }

    RGBAPixel(T r_, T g_, T b_) : r(r_), g(g_), b(b_), a(std::numeric_limits<T>::max())
    {
    }
    RGBAPixel(T r_, T g_, T b_, T a_) : r(r_), g(g_), b(b_), a(a_)
    {
    }

    static constexpr size_t stride()
    {
        return 3;
    }
    static void writeHeader(std::ostream& os, int width, int height)
    {
        os << "P6" << std::endl;
        os << width << " " << height << std::endl;
        os << +std::numeric_limits<T>::max() << std::endl;
    }

    T r, g, b, a;
};

using RGB32Pixel = RGBAPixel<uint8_t>;

template <typename PixelT>
class Image;

template <typename PixelT>
class RowAccessor {
  public:
    RowAccessor(Image<PixelT>* image_, size_t row_) : _image(image_), _row(row_)
    {
    }

    PixelT& operator[](size_t col)
    {
        return _image->data()[_row * _image->width() + col];
    }

  private:
    Image<PixelT>* _image;
    size_t _row;
};

template <typename PixelT>
class ConstRowAccessor {
  public:
    ConstRowAccessor(const Image<PixelT>* image_, size_t row_) : _image(image_), _row(row_)
    {
    }

    const PixelT& operator[](size_t col) const
    {
        return _image->data()[_row * _image->width() + col];
    }

  private:
    const Image<PixelT>* _image;
    size_t _row;
};

template <typename PixelT>
class Image {
  public:
    typedef PixelT pixel_type;

    Image(int width_, int height_) : _width(width_), _height(height_), _data(nullptr)
    {
        _data.reset(new PixelT[_width * _height]);
    }

    inline size_t numPixels() const
    {
        return _width * _height;
    }
    inline size_t bytes() const
    {
        return numPixels() * sizeof(PixelT);
    }
    inline int width() const
    {
        return _width;
    }
    inline int height() const
    {
        return _height;
    }

    PixelT* data()
    {
        return _data.get();
    }
    const PixelT* data() const
    {
        return _data.get();
    }

    RowAccessor<PixelT> operator[](size_t row)
    {
        return RowAccessor<PixelT>(this, row);
    }

    ConstRowAccessor<PixelT> operator[](size_t row) const
    {
        return RowAccessor<PixelT>(this, row);
    }

    void write(std::ostream& os)
    {
        PixelT::writeHeader(os, _width, _height);
        os.write((const char*)data(), bytes());
    }

  private:
    int _width;
    int _height;
    std::unique_ptr<PixelT> _data;
};
}
