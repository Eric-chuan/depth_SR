#include "depth_SR.h"
#include "common.h"



int main(int argc, char** argv)
{
    cv::Mat input_color = cv::imread("../datas/images/view1.png");
    cv::Mat input_depth = cv::imread("../datas/depths/disp1.png", cv::IMREAD_ANYDEPTH);
    input_depth.convertTo(input_depth,CV_32FC1, 1.0);
    // cv::Mat gray = cv::Mat::zeros(input_depth.rows, input_depth.cols, CV_8UC1);
    // cv::cvtColor(input_color, gray, cv::COLOR_RGB2GRAY);
    // double ga = gray.at<uchar>(1,2);

    // double a = input_color.at<uchar>(1,2);
    // uint8_t b = input_color.at<cv::Vec3b>(1,2)[0];
    // uint8_t g = input_color.at<cv::Vec3b>(1,2)[1];
    // uint8_t r = input_color.at<cv::Vec3b>(1,2)[2];
    // cv::Vec3b v(b, g, r);
    // double gray_data = 0.114*b + 0.587*g + 0.299*r;
    // uint8_t value = 0xFF000000 | (input_color.at<cv::Vec3b>(1,2)[2] & 0xFF) << 16 | (input_color.at<cv::Vec3b>(1,2)[1] & 0xFF) << 8 | (input_color.at<cv::Vec3b>(1,2)[0] & 0xFF);
    // printf("value = %f %i %i %i %f %i %f \n", a, r, g, b, gray_data, (int)value, ga);

    int height = input_color.rows;
    int width  =input_color.cols;
    int scale_h = height / input_depth.rows;
    int scale_w = width / input_depth.cols;
    printf("scale: %i, %i \n", scale_h, scale_w);
    cv::Mat LR_color  = cv::Mat::zeros(input_depth.rows, input_depth.cols, input_color.type());
    cv::resize(input_color, LR_color, LR_color.size(), 0, 0, cv::INTER_NEAREST);

    uint8_t* inputcolor = (uint8_t*)malloc(width * height * 3 * sizeof(uint8_t));
    uint8_t* LRcolor = (uint8_t*)malloc(width / scale_w * height / scale_h * 3 * sizeof(uint8_t));
    float* inputdepth = (float*)malloc(width / scale_w  * height / scale_h * sizeof(float));
    for(int i=0; i<height/scale_h; i++){
        for(int j=0; j<width/scale_w; j++){
            LRcolor[3*(i * width / scale_w + j) + 2] = LR_color.at<cv::Vec3b>(i, j)[0];
            LRcolor[3*(i * width / scale_w + j) + 1] = LR_color.at<cv::Vec3b>(i, j)[1];
            LRcolor[3*(i * width / scale_w + j) + 0] = LR_color.at<cv::Vec3b>(i, j)[2];
            inputdepth[(i * width / scale_w + j)] = input_depth.at<float>(i, j);
        }
	}
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            inputcolor[3*(i * width + j) + 2] = input_color.at<cv::Vec3b>(i, j)[0];
            inputcolor[3*(i * width + j) + 1] = input_color.at<cv::Vec3b>(i, j)[1];
            inputcolor[3*(i * width + j) + 0] = input_color.at<cv::Vec3b>(i, j)[2];
        }
	}
    DepthSR depthSR(width, height, scale_w, scale_h);
    depthSR.loadImage(input_color.data, LR_color.data, (float*)input_depth.data);
    depthSR.extractVariance();

    free(inputcolor);
    free(LRcolor);
    free(inputdepth);
    return 0;
}
