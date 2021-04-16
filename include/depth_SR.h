// created by hjc 20210324

#ifndef DEPTH_SR_H
#define DEPTH_SR_H

#include "common.h"
#include "typedef.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

extern "C" void extractCorrelation(ContextDepthSR *context, uint8_t *input_gray, float *input_depth, float *GD_correlation);
extern "C" ContextDepthSR* create_context(ContextDepthSR* context);

class DepthSR
{
private:
    uint8_t *HR_Gray;
    uint8_t *LR_Gray;
    float *LR_Depth;
    ContextDepthSR *ctx;
    ContextDepthSR *cu_context;

public:
public:
    DepthSR(int width, int height, int scale_w, int scale_h);
    ~DepthSR();
    void loadImage(uint8_t *input_gray, uint8_t *LR_gray, float *input_depth);
    void process();
};

#endif