#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    return get_pixel(im , roundf(x) , roundf(y), c);
}

image nn_resize(image im, int w, int h)
{
    image im1 = make_image( w , h , im.c);
    for ( int i = 0 ; i < im.c ; i++){
        for ( int j = 0 ; j < h ; j++){
            for( int k = 0 ; k < w ; k++){
                set_pixel( im1 , k , j , i , nn_interpolate( im , ( im.w / w ) * k  +  ( im.w * 0.5 / w ) - 0.5  , ( im.w / w ) * j  +  ( im.w * 0.5 / w ) - 0.5  , i));
            }
        }
    }
    return im1;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    int x1 = (int)x;
    int y1 = (int)y;
    float q1 = get_pixel( im , x1 , y1 , c )*( 1 - x + x1 ) + get_pixel(im , x1+1 , y1 , c)*(x - x1) ;
    float q2 = get_pixel( im , x1 , y1 - 1 , c)*( 1 - x + x1 ) + get_pixel( im , x1 + 1 , y1 - 1 ,c)*( x - x1) ; 
    return ( q1 * ( 1 - q + q1 ) + q2 * ( q - q1 ) );
}

image bilinear_resize(image im, int w, int h)
{
    float cnt = w/im.w ;
    image im1 = make_image( w , h , im.c);
    for ( int i = 0 ; i < im.c ; i++){
        for ( int j = 0 ; j < h ; j++){
            for( int k = 0 ; k < w ; k++){
                set_pixel( im1 , k , j , i , bilinear_interpolate( im , ( im.w / w ) * k  +  ( im.w * 0.5 / w ) - 0.5  , ( im.w / w ) * j  +  ( im.w * 0.5 / w ) - 0.5  , i));
            }
        }
    }
    return im1;
}

