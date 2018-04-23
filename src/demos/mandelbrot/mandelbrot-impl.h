#include <iostream>

#define VEC_SIZE 4

bool vec8_le(double* v, double f)
{
    return (v[0] <= f || v[1] <= f || v[2] <= f || v[3] <= f || v[4] <= f || v[5] <= f || v[6] <= f || v[7] <= f);
}

char pixels8(double* v, double f)
{
    char result = 0;
    if (v[0] <= f)
        result |= 0b10000000;
    if (v[1] <= f)
        result |= 0b01000000;
    if (v[2] <= f)
        result |= 0b00100000;
    if (v[3] <= f)
        result |= 0b00010000;
    if (v[4] <= f)
        result |= 0b00001000;
    if (v[5] <= f)
        result |= 0b00000100;
    if (v[6] <= f)
        result |= 0b00000010;
    if (v[7] <= f)
        result |= 0b00000001;
    return result;
}

// Mandelbrot periodicity check based on https://en.wikipedia.org/wiki/Mandelbrot_set#Periodicity_checking
inline char mand8(const double* init_r, double iy, SumFunc sumFunc)
{
    double r[8], i[8], sum[8];
    for (int k = 0; k < 8; k++) {
        r[k] = init_r[k];
        i[k] = iy;
    }

    Vec init_i(iy);

    for (int j = 0; j < 50; j++) {
        sumFunc((Vec*)r, (Vec*)i, (Vec*)sum, (const Vec*)init_r, init_i);
        if (!vec8_le(sum, 4.0)) {
            break;
        }
    }
    return pixels8(sum, 4.0);
}

void mandelbrot(int res, SumFunc sumFunc)
{
    res = (res + 7) & ~7;  // round up to multiple of 8

    int bytes = res * (res >> 3);
    char* pixels = new char[bytes];

    // calculate initial x values, store in r0
    double* r0 = new double[res];
    for (int x = 0; x < res; x++) {
        r0[x] = 2.0 / res * x - 1.5;
    }

#pragma omp parallel for schedule(guided)
    for (int y = 0; y < res; y++) {
        double iy = 2.0 / res * y - 1.0;
        int rowstart = y * res / 8;
        for (int x = 0; x < res; x += 8) {
            pixels[rowstart + x / 8] = mand8(&r0[x], iy, sumFunc);
        }
    }
    delete[] r0;

    std::cout << "P4\n" << res << " " << res << "\n";
    std::cout.write(pixels, bytes);
    std::cout.flush();
    delete[] pixels;
}