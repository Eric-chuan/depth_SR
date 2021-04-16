#ifndef DEPTH_SR_KERNEL_CUH
#define DEPTH_SR_KERNEL_CUH

#include <stdio.h>
#include "common.h"
#include "typedef.h"

extern "C" void extractCorrelation(ContextDepthSR *context, uint8_t *LR_Gray, float *LR_Depth, float *GD_correlation);
extern "C" ContextDepthSR* create_context(ContextDepthSR* context);

#endif