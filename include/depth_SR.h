// created by hjc 20210324

#ifndef DEPTH_SR_H
#define DEPTH_SR_H

#include "common.h"
#include "typedef.h"

class DepthSR
{
private:
    uint8_t *HR_Color;
    uint8_t *LR_Color;
    float *LR_Depth;
    int width;
    int height;
    int scale_w, scale_h;

public:
public:
    DepthSR(int width, int height, int scale_w, int scale_h);
    ~DepthSR();
    void loadImage(uint8_t *input_color, uint8_t *LR_color, float *input_depth);
    void extractVariance();

};

#endif