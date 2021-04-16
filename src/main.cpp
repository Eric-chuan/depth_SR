#include "depth_SR.h"
#include "common.h"

extern float get_time_diff_ms(timeval start, timeval end)
{
    long time_ms_end =  (end.tv_sec * 1000000 + end.tv_usec);
    long time_ms_start =  (start.tv_sec * 1000000 + start.tv_usec);
    return float(time_ms_end - time_ms_start) / 1000;
}

int main(int argc, char** argv)
{
    cv::Mat input_gray = cv::imread("../datas/images/image0_1920x1080.png", 0);
    cv::Mat input_depth = cv::imread("../datas/depths/depth0_960x540.png", cv::IMREAD_ANYDEPTH);
    input_depth.convertTo(input_depth,CV_32FC1, 1.0);

    int height = input_gray.rows;
    int width  = input_gray.cols;
    int scale_h = height / input_depth.rows;
    int scale_w = width / input_depth.cols;
    printf("scale: %i, %i \n", height, scale_w);
    cv::Mat LR_gray  = cv::Mat::zeros(input_depth.rows, input_depth.cols, input_gray.type());
    cv::resize(input_gray, LR_gray, LR_gray.size(), 0, 0, cv::INTER_NEAREST);

    DepthSR depthSR(width, height, scale_w, scale_h);
    depthSR.loadImage(input_gray.data, LR_gray.data, (float*)input_depth.data);
    timeval t_start, t_end;
    float time_diff;
    for(int i = 0; i < 1; i++){
        gettimeofday(&t_start, NULL);
        depthSR.process();
        gettimeofday(&t_end, NULL);
        time_diff = get_time_diff_ms(t_start, t_end);
        printf("[Time INFO]: process %f ms.\n", time_diff);
    }

    return 0;
}
