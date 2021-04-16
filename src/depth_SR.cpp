#include "depth_SR.h"

DepthSR::DepthSR(int width, int height, int scale_w, int scale_h)
{
    this->ctx = new ContextDepthSR();
    ctx->width = width;
    this->ctx->height = height;
    this->ctx->scale_w = scale_w;
    this->ctx->scale_h = scale_h;
    this->cu_context = create_context(ctx);                      //to do: free this cu_context
    size_t HR_size = width * height;
    this->HR_Gray = (uint8_t*)calloc(HR_size, sizeof(uint8_t));
    this->LR_Gray = (uint8_t*)calloc((HR_size / (scale_w * scale_h)), sizeof(uint8_t));
    this->LR_Depth = (float*)calloc(HR_size / (scale_w * scale_h), sizeof(float));
}

DepthSR::~DepthSR()
{
    free(this->HR_Gray);
    free(this->LR_Gray);
    free(this->LR_Depth);
    //cuda_free();
}

void DepthSR::loadImage(uint8_t *input_color, uint8_t *LR_Gray, float *input_depth)
{
    int width = ctx->width;
    int height = ctx->height;
    int scale_w = ctx->scale_w;
    int scale_h = ctx->scale_h;
    int s_width = width / scale_w;
    int s_height = height / scale_h;
    size_t HR_size = width * height;
    memcpy(this->HR_Gray, input_color, HR_size * sizeof(uint8_t));
    memcpy(this->LR_Gray, LR_Gray, (HR_size / (scale_w * scale_h)) * sizeof(uint8_t));
    memcpy(this->LR_Depth, input_depth, (HR_size / (scale_w * scale_h)) * sizeof(float));
}

void DepthSR::process()
{
    int width = ctx->width;
    int height = ctx->height;
    int scale_w = ctx->scale_w;
    int scale_h = ctx->scale_h;
    int s_width = width / scale_w;
    int s_height = height / scale_h;

    float *GD_correlation = (float*)calloc(s_height * s_width, sizeof(float));

    extractCorrelation(cu_context, LR_Gray, LR_Depth, GD_correlation);

    cv::Mat VarG = cv::Mat::zeros((s_height), (s_width), CV_32FC1);
    for(int i = 0; i < s_height; i++){
        for(int j = 0; j < s_width; j++){
            VarG.at<float>(i, j) = GD_correlation[s_width * i + j];
        }
    }
    cv::imwrite("../results/gd_correlation.png", VarG);

    // cv::mat borderimage;
    // cv::mat((s_height), (s_width), cv_32fc1, gd_correlation).copyto(borderimage);
    // cv::imwrite("../results/gd_correlation.png", borderimage);


    // cv::Mat((s_height), (s_width), CV_32FC1, depth_variance).copyTo(borderimage);
    // cv::imwrite("../results/depth_variance.png", borderimage);

    free(GD_correlation);
}

