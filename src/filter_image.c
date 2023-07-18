#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    float sum = 0.0 ;
    for ( int i = 0 ; i < im.c ; i++){
        for ( int j = 0 ; j < im.h ; j++){
            for ( int k = 0 ; k < im.w ; k++){
                sum += get_pixel(im , k , j , i);
            }
        }
    }
    for ( int i = 0 ; i < im.c ; i++){
        for ( int j = 0 ; j < im.h ; j++){
            for ( int k = 0 ; k < im.w ; k++){
                set_pixel(im , k , j , i , get_pixel(im,k,j,i)/sum);
            }
        }
    }
}

image make_box_filter(int w)
{
    image im1 = make_image(w ,w , 1);
     for ( int j = 0 ; j < im.h ; j++){
            for ( int k = 0 ; k < im.w ; k++){
                set_pixel(im , k , j , 1 , 1 / ( w*w ));
            }
     }

    l1_normalize(im1);

    return im1;
}

image convolve_image(image im, image filter, int preserve)
{
    
    assert(filter.c == im.c || filter.c == 1);

    int i, j, k, m, n, c;

    int output_width = im.w - filter.w + 1;
    int output_height = im.h - filter.h + 1;
    int output_channels = (preserve && filter.c == 1) ? im.c : 1;

    image output;
    output.w = output_width;
    output.h = output_height;
    output.c = output_channels;

    output.data = (float *)malloc(output_width * output_height * output_channels * sizeof(float));
    for (c = 0; c < output_channels; c++) {
        for (i = 0; i < output_height; i++) {
            for (j = 0; j < output_width; j++) {
                float sum = 0.0;
                for (k = 0; k < filter.h; k++) {
                    for (m = 0; m < filter.w; m++) {
                        for (n = 0; n < im.c; n++) {
                            int im_x = j + m;
                            int im_y = i + k;
                            int im_channel = n;

                            float im_pixel = im.data[im_channel * im.w * im.h + im_y * im.w + im_x];
                            float filter_coefficient = filter.data[(filter.c == 1 ? 0 : c) * filter.w * filter.h + k * filter.w + m];

                            sum += im_pixel * filter_coefficient;
                        }
                    }
                }
                output.data[c * output_width * output_height + i * output_width + j] = sum;
            }
        }
    }

    return output;
}

image make_highpass_filter()
{
    
    image filter;
    filter.w = 3;
    filter.h = 3;
    filter.c = 1;
    filter.data = (float *)malloc(9 * sizeof(float));
    float values[] = { -1, -1, -1,
                       -1,  8, -1,
                       -1, -1, -1 };
    int i;
    for (i = 0; i < 9; i++) {
        filter.data[i] = values[i];
    }

    return filter;

}

image make_sharpen_filter()
{
    
    image filter;
    filter.w = 3;
    filter.h = 3;
    filter.c = 1;
    filter.data = (float *)malloc(9 * sizeof(float));
    float values[] = {  0, -1,  0,
                       -1,  5, -1,
                        0, -1,  0 };
    int i;
    for (i = 0; i < 9; i++) {
        filter.data[i] = values[i];
    }

    return filter;


}

image make_emboss_filter()
{
    
    image filter;
    filter.w = 3;
    filter.h = 3;
    filter.c = 1;
    filter.data = (float *)malloc(9 * sizeof(float));
    float values[] = { -2, -1,  0,
                       -1,  1,  1,
                        0,  1,  2 };
    int i;
    for (i = 0; i < 9; i++) {
        filter.data[i] = values[i];
    }

    return filter;

}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: The box filter, sharpen filter, and emboss filter should be preserved (preserve = 1) since they smooth and enhance the image, resulting in an output that retains important details. On the other hand, the highpass filter does not require preservation (preserve = 0) as it focuses on highlighting high-frequency information.

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: It is important to apply post-processing techniques for the highpass, sharpen, and emboss filters to prevent pixel overflow, ensuring that the pixel values remain within the valid range of 0 to 1.

image make_gaussian_filter(float sigma)
{
    int size = (int)(ceil(sigma * 6.0));
    size = (size % 2 == 0) ? size + 1 : size; 

    image filter;
    filter.w = size;
    filter.h = size;
    filter.c = 1;
    filter.data = (float *)malloc(size * size * sizeof(float));

    float sum = 0.0;
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            int x = j - size / 2;
            int y = i - size / 2;
            float value = exp(-(x * x + y * y) / (2.0 * sigma * sigma)) / (2.0 * M_PI * sigma * sigma);
            filter.data[i * size + j] = value;
            sum += value;
        }
    }
    for (i = 0; i < size * size; i++) {
        filter.data[i] /= sum;
    }

    return filter;

}

image add_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);

    int i;

    image result;
    result.w = a.w;
    result.h = a.h;
    result.c = a.c;
    result.data = (float *)malloc(a.w * a.h * a.c * sizeof(float));
    for (i = 0; i < a.w * a.h * a.c; i++) {
        result.data[i] = a.data[i] + b.data[i];
    }

    return result;
}

image sub_image(image a, image b)
{
     assert(a.w == b.w && a.h == b.h && a.c == b.c);

    int i;

    image result;
    result.w = a.w;
    result.h = a.h;
    result.c = a.c;
    result.data = (float *)malloc(a.w * a.h * a.c * sizeof(float));
    for (i = 0; i < a.w * a.h * a.c; i++) {
        result.data[i] = a.data[i] - b.data[i];
    }

    return result;
}

image make_gx_filter()
{
     float gx_data[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    image gx;
    gx.w = 3;
    gx.h = 3;
    gx.c = 1;
    gx.data = gx_data;
    return gx;
}

image make_gy_filter()
{
     float gy_data[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    image gy;
    gy.w = 3;
    gy.h = 3;
    gy.c = 1;
    gy.data = gy_data;
    return gy;
}

void feature_normalize(image im)
{
     float min_val = im.data[0];
    float max_val = im.data[0];
    int i;
    for (i = 1; i < im.w * im.h * im.c; i++) {
        if (im.data[i] < min_val) {
            min_val = im.data[i];
        }
        if (im.data[i] > max_val) {
            max_val = im.data[i];
        }
    }

    float range = max_val - min_val;
    if (range == 0.0) {
        for (i = 0; i < im.w * im.h * im.c; i++) {
            im.data[i] = 0.0;
        }
        return;
    }
    for (i = 0; i < im.w * im.h * im.c; i++) {
        im.data[i] = (im.data[i] - min_val) / range;
    }

}

image *sobel_image(image im)
{
     image *results = calloc(2, sizeof(image));
    image gx = make_gx_filter();
    image gy = make_gy_filter();
    image dx = convolve_image(im, gx, 0);
    image dy = convolve_image(im, gy, 0);
    int i;
    image magnitude;
    magnitude.w = dx.w;
    magnitude.h = dx.h;
    magnitude.c = dx.c;
    magnitude.data = malloc(dx.w * dx.h * dx.c * sizeof(float));

    image direction;
    direction.w = dx.w;
    direction.h = dx.h;
    direction.c = dx.c;
    direction.data = malloc(dx.w * dx.h * dx.c * sizeof(float));

    for (i = 0; i < dx.w * dx.h * dx.c; i++) {
        magnitude.data[i] = sqrt(dx.data[i] * dx.data[i] + dy.data[i] * dy.data[i]);
        direction.data[i] = atan2(dy.data[i], dx.data[i]);
    }

    results[0] = magnitude;
    results[1] = direction;
    free(dx.data);
    free(dy.data);
    free(gx.data);
    free(gy.data);

    return results;

}

image colorize_sobel(image im)
{
     image colorized;
    colorized.w = im.w;
    colorized.h = im.h;
    colorized.c = im.c;
    colorized.data = malloc(im.w * im.h * im.c * sizeof(float));
    int i;
    for (i = 0; i < im.w * im.h * im.c; i++) {
        colorized.data[i] = im.data[i];
    }
    return colorized;
}
