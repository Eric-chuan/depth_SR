#include "depth_SR.h"

DepthSR::DepthSR(int width, int height, int scale_w, int scale_h)
{   this->width = width;
    this->height = height;
    this->scale_w = scale_w;
    this->scale_h = scale_h;
    size_t HR_size = width * height;
    this->HR_Color = (uint8_t*)calloc(HR_size * 3, sizeof(uint8_t));
    this->LR_Color = (uint8_t*)calloc((HR_size / (scale_w * scale_h)) * 3, sizeof(uint8_t));
    this->LR_Depth = (float*)calloc(HR_size / (scale_w * scale_h), sizeof(float));
}

DepthSR::~DepthSR()
{
    free(this->HR_Color);
    free(this->LR_Color);
    free(this->LR_Depth);
}

void DepthSR::loadImage(uint8_t *input_color, uint8_t *LR_color, float *input_depth)
{
    size_t HR_size = width * height;
    memcpy(this->HR_Color, input_color, HR_size * 3 * sizeof(uint8_t));
    memcpy(this->LR_Color, LR_color, (HR_size / (scale_w * scale_h)) * 3 * sizeof(uint8_t));
    memcpy(this->LR_Depth, input_depth, (HR_size / (scale_w * scale_h)) * sizeof(float));
}

void DepthSR::extractVariance()
{
    int s_width = width / scale_w;
    int s_height = height / scale_h;

 //get the padding border image
    int border = 2;
    size_t size = (s_width + border) * (s_height + border);
    uint8_t *LR_ColorBorder = (uint8_t*)calloc(size * 3, sizeof(uint8_t));
    float *LR_DepthBorder = (float*)calloc(size, sizeof(float));
    for(int i = 1; i < s_height + 1; i++){
        for(int j = 1; j < s_width + 1; j++){
            int pixel_b = i * (s_width + border) + j;
            int pixel = (i-1) * s_width + (j-1);
            LR_ColorBorder[3 * pixel_b] = LR_Color[3 * pixel];
            LR_ColorBorder[3 * pixel_b + 1] = LR_Color[3 * pixel + 1];
            LR_ColorBorder[3 * pixel_b + 2] = LR_Color[3 * pixel + 2];
            LR_DepthBorder[pixel_b] = LR_Depth[pixel];
        }
    }//CENTER
    for(int i = 1; i < s_width + 1; i++){
        LR_ColorBorder[3 * i] = LR_Color[3 * (i - 1)];
        LR_ColorBorder[3 * i + 1] = LR_Color[3 * (i - 1) + 1];
        LR_ColorBorder[3 * i + 2] = LR_Color[3 * (i - 1) + 2];
        LR_DepthBorder[i] = LR_Depth[i - 1]; //TOP

        int pixel_b = (s_width + border) * (s_height + 1) + i;
        int pixel = s_width * (s_height - 1) + i - 1;
        LR_ColorBorder[3 * pixel_b] = LR_Color[3 * pixel];
        LR_ColorBorder[3 * pixel_b + 1] = LR_Color[3 * pixel + 1];
        LR_ColorBorder[3 * pixel_b + 2] = LR_Color[3 * pixel + 2];
        LR_DepthBorder[pixel_b] = LR_Depth[pixel];//BOTTOM
    }
    for(int j = 1; j < s_height + 1; j++){
        LR_ColorBorder[3 * j * (s_width + border)] = LR_ColorBorder[3 * (j * (s_width + border) + 1)];
        LR_ColorBorder[3 * j * (s_width + border) + 1] = LR_ColorBorder[3 * (j * (s_width + border) + 1) + 1];
        LR_ColorBorder[3 * j * (s_width + border) + 2] = LR_ColorBorder[3 * (j * (s_width + border) + 1) + 2];
        LR_DepthBorder[j * (s_width + border)] = LR_DepthBorder[j * (s_width + border) + 1];//LEFT

        LR_ColorBorder[3 * ((j + 1) * (s_width + border) - 1)] = LR_ColorBorder[3 * ((j + 1) * (s_width + border) - 2)];
        LR_ColorBorder[3 * ((j + 1) * (s_width + border) - 1) + 1] = LR_ColorBorder[3 * ((j + 1) * (s_width + border) - 2) + 1];
        LR_ColorBorder[3 * ((j + 1) * (s_width + border) - 1) + 2] = LR_ColorBorder[3 * ((j + 1) * (s_width + border) - 2) + 2];
        LR_DepthBorder[(j + 1) * (s_width + border) - 1] = LR_DepthBorder[(j + 1) * (s_width + border) - 2];//RIGHT
    }

    //the four corner value
    int pixel_corner;
    LR_ColorBorder[0] = (LR_ColorBorder[3] + LR_ColorBorder[3 * (s_width + border)]) / 2;
    LR_ColorBorder[1] = (LR_ColorBorder[3 + 1] + LR_ColorBorder[3 * (s_width + border) + 1]) / 2;
    LR_ColorBorder[2] = (LR_ColorBorder[3 + 2] + LR_ColorBorder[3 * (s_width + border) + 2]) / 2;
    LR_DepthBorder[0] = (LR_DepthBorder[3] + LR_DepthBorder[(s_width + border)]) / 2;

    pixel_corner = (s_width + border) - 1;
    LR_ColorBorder[3 * pixel_corner] = (LR_ColorBorder[3 * (pixel_corner - 1)] + LR_ColorBorder[3 * (pixel_corner + s_width + border)]) / 2;
    LR_ColorBorder[3 * pixel_corner + 1] = (LR_ColorBorder[3 * (pixel_corner - 1) + 1] + LR_ColorBorder[3 * (pixel_corner + s_width + border) + 1]) / 2;
    LR_ColorBorder[3 * pixel_corner + 2] = (LR_ColorBorder[3 * (pixel_corner - 1) + 2] + LR_ColorBorder[3 * (pixel_corner + s_width + border) + 2]) / 2;
    LR_DepthBorder[pixel_corner] = (LR_DepthBorder[(pixel_corner - 1)] + LR_DepthBorder[(pixel_corner + s_width + border)]) / 2;

    pixel_corner = ((s_height + border) - 1) * (s_width + border);
    LR_ColorBorder[3 * pixel_corner] = (LR_ColorBorder[3 * (pixel_corner + 1)] + LR_ColorBorder[3 * (pixel_corner - s_width - border)]) / 2;
    LR_ColorBorder[3 * pixel_corner + 1] = (LR_ColorBorder[3 * (pixel_corner + 1) + 1] + LR_ColorBorder[3 * (pixel_corner - s_width - border) + 1]) / 2;
    LR_ColorBorder[3 * pixel_corner + 2] = (LR_ColorBorder[3 * (pixel_corner + 1) + 2] + LR_ColorBorder[3 * (pixel_corner - s_width - border) + 2]) / 2;
    LR_DepthBorder[pixel_corner] = (LR_DepthBorder[(pixel_corner + 1)] + LR_DepthBorder[(pixel_corner - s_width - border)]) / 2;

    pixel_corner = (s_height + border) * (s_width + border) - 1;
    LR_ColorBorder[3 * pixel_corner] = (LR_ColorBorder[3 * (pixel_corner - 1)] + LR_ColorBorder[3 * (pixel_corner - s_width - border)]) / 2;
    LR_ColorBorder[3 * pixel_corner + 1] = (LR_ColorBorder[3 * (pixel_corner - 1) + 1] + LR_ColorBorder[3 * (pixel_corner - s_width - border) + 1]) / 2;
    LR_ColorBorder[3 * pixel_corner + 2] = (LR_ColorBorder[3 * (pixel_corner - 1) + 2] + LR_ColorBorder[3 * (pixel_corner - s_width - border) + 2]) / 2;
    LR_DepthBorder[pixel_corner] = (LR_DepthBorder[(pixel_corner - 1)] + LR_DepthBorder[(pixel_corner - s_width - border)]) / 2;


    cv::Mat borderimage;
    cv::Mat((s_height + border), (s_width + border), CV_32FC1, LR_DepthBorder).copyTo(borderimage);
    cv::imwrite("../results/depthborder.png", borderimage);


    cv::Mat((s_height + border), (s_width + border), CV_8UC3, LR_ColorBorder).copyTo(borderimage);
    cv::imwrite("../results/colorborder.png", borderimage);

}

