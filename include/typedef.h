// created by hjc 20210324
#ifndef TYPEDEF_H
#define TYPEDEF_H
#include "common.h"

typedef struct NoiseFilter
{
    const int8_t Gaus3x3[9] = { 1, 2, 1,
                            2, 4, 2,   // * 1/16
                            1, 2, 1};
    const int Gaus3x3Div = 16;

    const int8_t Gaus5x5[25] = { 2,  4,  5,  4, 2,
                                4,  9, 12,  9, 4,
                                5, 12, 15, 12, 5, // * 1/159
                                4,  9, 12,  9, 4,
                                2,  4,  5,  4, 2};
    const int Gaus5x5Div = 159;
}NoiseFilter;

typedef struct SobelFilter
{
    const int8_t Gx[9] = {-1, 0, 1,
                         -2, 0, 2,
                         -1, 0, 1};

    const int8_t Gy[9] = { 1, 2, 1,
                          0, 0, 0,
                         -1,-2,-1};
}SobelFilter;

typedef struct ContextDepthSR
{
    int width;
    int height;
    int scale_w;
    int scale_h;

}ContextDepthSR;

#define THREADS_PER_BLOCK 20
#define ThreadsPerBlock 32

#endif