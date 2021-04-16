#include "depthSR_kernel.cuh"

__global__ void memcpy_float(int width, float* dst, float* src)
{
    int x = threadIdx.x + blockIdx.x * blockDim.x;
    int y = threadIdx.y + blockIdx.y * blockDim.y;
    dst[y * width + x] = src[y * width + x];
}

__global__ void extractCorrelation_kernel(ContextDepthSR *context, uint8_t *LR_GrayBorder, float *LR_DepthBorder, float *GD_correlation)
{
    int x = threadIdx.x + blockIdx.x * blockDim.x;
    int y = threadIdx.y + blockIdx.y * blockDim.y;
    int pixel = y + x * blockDim.y * gridDim.y;

    int width = context->width;
    int height = context->height;
    int scale_w = context->scale_w;
    int scale_h = context->scale_h;
    int s_width = width / scale_w;
    int s_height = height / scale_h;


    int win_size = 3; //3x3 windows
    float sumPixG = 0.0, sumPixG2 = 0.0;
    float sumPixD = 0.0, sumPixD2 = 0.0;
    float sumPixGD = 0.0;
    for (int m = -win_size / 2; m <= win_size / 2; m++){
        int i = x + 1 + m;
        i = (i > 0 ? (i < s_height + 2 ? i : s_height + 1) : 0);  //make sure the index (x+n, y+m) is located in the image.
        for (int n = -win_size / 2; n <= win_size / 2; n++){
            int j = y + 1 + n;
            j = (j > 0 ? (j < s_width + 2 ? j : s_width + 1) : 0);
            uint8_t a1 = LR_GrayBorder[i * (s_width + 2) + j];

            float a2 = LR_DepthBorder[i * (s_width + 2) + j];
            sumPixG += a1;
            sumPixD += a2;
            sumPixG2 += (a1 * a1);
            sumPixD2 += (a2 * a2);
            sumPixGD += (a1 * a2);
        }//end for n
    }//end for m
    float meanPixG = sumPixG / (win_size * win_size);//EX
    float meanPixD = sumPixD / (win_size * win_size);
    float meanPixG2 = sumPixG2 / (win_size * win_size); //E(X^2)
    float meanPixD2 = sumPixD2 / (win_size * win_size);
    float meanPixGD = sumPixGD / (win_size * win_size);
    float CA = meanPixGD - meanPixG * meanPixD;
    float variancePixG = meanPixG2 - meanPixG * meanPixG;
    float variancePixD = meanPixD2 - meanPixD * meanPixD;
    CA /= sqrt(variancePixG * variancePixD);
    GD_correlation[pixel] = CA;
}


ContextDepthSR* create_context(ContextDepthSR* context)
{
    ContextDepthSR *cu_context;
    cudaMallocManaged((void**)&cu_context, sizeof(ContextDepthSR));

    cudaMemcpy(&cu_context->width, &context->width, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(&cu_context->height, &context->height, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(&cu_context->scale_w, &context->scale_w, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(&cu_context->scale_h, &context->scale_h, sizeof(int), cudaMemcpyHostToDevice);

    return cu_context;
}
void extractCorrelation(ContextDepthSR *context, uint8_t *LR_Gray, float *LR_Depth, float *GD_correlation)
{
    int width = context->width;
    int height = context->height;
    int scale_w = context->scale_w;
    int scale_h = context->scale_h;
    int s_width = width / scale_w;
    int s_height = height / scale_h;

 //get the padding border image
    int border = 2;
    size_t size = (s_width + border) * (s_height + border);
    uint8_t *LR_GrayBorder = (uint8_t*)calloc(size, sizeof(uint8_t));
    float *LR_DepthBorder = (float*)calloc(size, sizeof(float));
    for(int i = 1; i < s_height + 1; i++){
        for(int j = 1; j < s_width + 1; j++){
            int pixel_b = i * (s_width + border) + j;
            int pixel = (i-1) * s_width + (j-1);
            LR_GrayBorder[pixel_b] = LR_Gray[pixel];
            LR_DepthBorder[pixel_b] = LR_Depth[pixel];
        }
    }//CENTER
    for(int i = 1; i < s_width + 1; i++){
        LR_GrayBorder[i] = LR_Gray[i - 1];
        LR_DepthBorder[i] = LR_Depth[i - 1]; //TOP

        int pixel_b = (s_width + border) * (s_height + 1) + i;
        int pixel = s_width * (s_height - 1) + i - 1;
        LR_GrayBorder[pixel_b] = LR_Gray[pixel];
        LR_DepthBorder[pixel_b] = LR_Depth[pixel];//BOTTOM
    }
    for(int j = 1; j < s_height + 1; j++){
        LR_GrayBorder[j * (s_width + border)] = LR_GrayBorder[j * (s_width + border) + 1];
        LR_DepthBorder[j * (s_width + border)] = LR_DepthBorder[j * (s_width + border) + 1];//LEFT

        LR_GrayBorder[(j + 1) * (s_width + border) - 1] = LR_GrayBorder[(j + 1) * (s_width + border) - 2];
        LR_DepthBorder[(j + 1) * (s_width + border) - 1] = LR_DepthBorder[(j + 1) * (s_width + border) - 2];//RIGHT
    }

    //the four corner value
    int pixel_corner;
    LR_GrayBorder[0] = (LR_GrayBorder[1] + LR_GrayBorder[(s_width + border)]) / 2;
    LR_DepthBorder[0] = (LR_DepthBorder[1] + LR_DepthBorder[(s_width + border)]) / 2;

    pixel_corner = (s_width + border) - 1;
    LR_GrayBorder[pixel_corner] = (LR_GrayBorder[(pixel_corner - 1)] + LR_GrayBorder[(pixel_corner + s_width + border)]) / 2;
    LR_DepthBorder[pixel_corner] = (LR_DepthBorder[(pixel_corner - 1)] + LR_DepthBorder[(pixel_corner + s_width + border)]) / 2;

    pixel_corner = ((s_height + border) - 1) * (s_width + border);
    LR_GrayBorder[pixel_corner] = (LR_GrayBorder[(pixel_corner + 1)] + LR_GrayBorder[(pixel_corner - s_width - border)]) / 2;
    LR_DepthBorder[pixel_corner] = (LR_DepthBorder[(pixel_corner + 1)] + LR_DepthBorder[(pixel_corner - s_width - border)]) / 2;

    pixel_corner = (s_height + border) * (s_width + border) - 1;
    LR_GrayBorder[pixel_corner] = (LR_GrayBorder[(pixel_corner - 1)] + LR_GrayBorder[(pixel_corner - s_width - border)]) / 2;
    LR_DepthBorder[pixel_corner] = (LR_DepthBorder[(pixel_corner - 1)] + LR_DepthBorder[(pixel_corner - s_width - border)]) / 2;

    dim3 blocks(s_height / THREADS_PER_BLOCK, s_width / ThreadsPerBlock);
    dim3 threads(THREADS_PER_BLOCK, ThreadsPerBlock);


    uint8_t* cu_LR_GrayBorder;
    float* cu_LR_DepthBorder;
    float* cu_GD_correlation;

    cudaMalloc((void**)&cu_LR_GrayBorder, (s_height + border) * (s_width + border) * sizeof(uint8_t));
    cudaMalloc((void**)&cu_LR_DepthBorder, (s_height + border) * (s_width + border) * sizeof(float));
    cudaMemcpy(cu_LR_GrayBorder, LR_GrayBorder, (s_height + border) * (s_width + border) * sizeof(uint8_t), cudaMemcpyHostToDevice);
    cudaMemcpy(cu_LR_DepthBorder, LR_DepthBorder, (s_height + border) * (s_width + border) * sizeof(float), cudaMemcpyHostToDevice);

    cudaMallocManaged((void**)&cu_GD_correlation, s_width * s_height * sizeof(float));

    extractCorrelation_kernel<<<blocks, threads>>>(context, cu_LR_GrayBorder, cu_LR_DepthBorder, cu_GD_correlation);

    //memcpy_float<<<blocks, threads>>>(s_width, gray_variance, cu_gray_variance);
    //memcpy_float<<<blocks, threads>>>(s_width, depth_variance, cu_depth_variance);
    cudaMemcpy(GD_correlation, cu_GD_correlation, s_width * s_height * sizeof(float), cudaMemcpyDeviceToHost);
    //memcpy(GD_correlation, cu_GD_correlation, s_width * s_height * sizeof(float));

    cudaFree(cu_LR_GrayBorder);
    cudaFree(cu_LR_DepthBorder);
    cudaFree(cu_GD_correlation);
}