#include "Vec-impl.h"
using SumFunc = void (*)(Vec*, Vec*, Vec*, const Vec*, const Vec&);

#include "mandelbrot-impl.h"

inline void calcSum(Vec* r, Vec* i, Vec* sum, const Vec* init_r, const Vec& init_i)
{
    for (size_t vec = 0; vec < 2; vec++) {
        Vec r2 = r[vec] * r[vec];
        Vec i2 = i[vec] * i[vec];
        Vec ri = r[vec] * i[vec];

        sum[vec] = r2 + i2;
        r[vec] = r2 - i2 + init_r[vec];
        i[vec] = ri + ri + init_i;
    }
}

int main(int argc, char** argv)
{
    int res = 16000;
    if (argc >= 2) {
        res = atoi(argv[1]);
    }

    mandelbrot(res, calcSum);
}