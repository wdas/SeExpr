#pragma once

#include <immintrin.h>

class Vec {
  public:
    // specifying this alignment over using __m128d increases performance 2x
    // on a Dell T7600 -- results may differ on other machines
    typedef double __attribute__((vector_size(32), aligned(16))) __m128d_aligned;
    union {
        struct {
            double x{0.}, y{0.}, z{0.}, w{0.};
        };
        __m128d_aligned v;
    };

    Vec() { }

    Vec(__m128d_aligned v_) : v(v_)
    {
    }
    Vec(double v) : x(v), y(v), z(v), w(v)
    {
    }
    Vec(double v0, double v1, double v2, double v3) : x(v0), y(v1), z(v2), w(v3)
    {
    }

    inline Vec operator+(const Vec& other)
    {
        return Vec(v + other.v);
    }
    inline Vec operator-(const Vec& other)
    {
        return Vec(v - other.v);
    }
    inline Vec operator*(const Vec& other)
    {
        return Vec(v * other.v);
    }
    inline Vec operator/(const Vec& other)
    {
        return Vec(v / other.v);
    }
    inline double& operator[](size_t i)
    {
        switch (i % 4) {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            case 3:
                return w;
        }
    }
    inline const double& operator[](size_t i) const
    {
        return v[i];
    }
};
