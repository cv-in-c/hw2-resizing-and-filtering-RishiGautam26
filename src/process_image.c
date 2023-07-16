#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    return im.data[ c*im.w*im.h + y*im.w + x ];
}

void set_pixel(image im, int x, int y, int c, float v)
{

    if ( x < 0 ) x = 0 ;
    else if ( x > im.w ) x = im.w - 1 ;

    if ( y < 0 ) y = 0 ;
    else if ( y > im.h ) y = im.h - 1 ;

    if ( c < 0 || c > im.c ) return ;

    im.data[ c*im.h*im.w + y*im.w + x ] = v ;

}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    for (int i = 0 ; i < im.c ; i++){
        for ( int j = 0 ; j < im.h ; j++){
            for ( int k = 0 ; k < im.w ; k++){
                set_pixel( copy , k , j , i , get_pixel( im , k , j , i));
            }
        }
    }
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    for ( int i = 0 ; i < im.h ; i++){
        for ( int j = 0 ; j < im.w ; j++){
            set_pixel( gray , j , i , 0, get_pixel( im , j , i , 0)*0.299 + get_pixel( im , j , i , 1)*0.587 + get_pixel( im , j , i , 2)*0.114 ) ; 
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    for ( int i = 0 ; i < im.h ; i++){
        for ( int j = 0 ; j < im.w ; j++){
            set_pixel( im , j , i , c , get_pixel( im , j , i , c) + v) ;
        }
    }
}

void clamp_image(image im)
{
    for ( int i  = 0 ; i < im.c ; i++){
        for( int j = 0 ; j < im.h ; j++){
            for ( int k = 0 ; k < im.w ; k++){
                if (get_pixel( im , k , j , i) > 1) {
                    set_pixel( im , k , j , i , 1) ;
                }
                else if ( get_pixel( im , k , j , i) < 0 ) {
                    set_pixel( im , k , j , i , 0 ) ;   
                }
            }
        }
    }

    save_image( im , "fixed" ) ;
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    //for V 
    float V, C, m, S, H1, H;
    for ( int i = 0 ; i < im.h ; i++){
        for ( int j = 0 ; j < im.w ; j++){
            V = three_way_max( get_pixel(im, j, i, 0) , get_pixel(im, j, i, 1) , get_pixel(im, j, i, 2));
            m = three_way_min( get_pixel(im, j, i, 0) , get_pixel(im, j, i, 1) , get_pixel(im, j, i, 2) );
            C = V - m ;

            if ( V == 0) S = 0 ;
            else if ( V != 0) S = C/V ;

            if ( C == 0 ) {
                H1 = 0 ;
                H = 0 ;
            }
            else if ( V == get_pixel(im, j, i, 0) ){
                H1 = (get_pixel(im, j, i, 1) - get_pixel(im, j, i, 2))/C ;
                if ( H1 < 0 ){
                    H = H1/6 + 1 ;
                }
                else H = H1/6 ;
            }
            else if ( V == get_pixel(im, j, i, 1) ){
                H1 = (get_pixel(im, j, i, 2) - get_pixel(im, j, i, 0))/C + 2 ;
                if ( H1 < 0 ) H = H1/6 + 1 ;
                else H = H1/6 ;
            }
            else if ( V == get_pixel(im, j, i, 2) ){
                H1 = ( get_pixel(im, j, i, 0 ) - get_pixel(im, j, i, 1) )/C + 4 ;
                if ( H1 < 0 ) H = H1/6 + 1 ;
                else H = H1/6 ;
            }

            set_pixel( im , j, i, 0, H);
            set_pixel( im , j, i, 1, S);
            set_pixel( im , j, i, 2, V);            
        }
    }
}

void hsv_to_rgb(image im)
{
    float r, g, b, h, s, v ;
    for ( int i = 0 ; i < im.h ; i++){
        for ( int j = 0 ; j < im.w ; j++){
            h = im.data[ i*im.w + j ] ;
            s = im.data[ im.h*im.w + i*im.w + j];
            v = im.data[ 2*im.h*im.w + i*im.w + j ];
            r = im.data[ i*im.w + j ] ;
            g = im.data[ im.h*im.w + i*im.w + j];
            b = im.data[ 2*im.h*im.w + i*im.w + j ];
            
            if ( s == 0 ) {
                r = v;
                g = v;
                b = v;
            }

            else {
                float a = floor( h*6 ) ;
                float b = h*6 - i ;
                float c = v*(1-s) ;
                float d = v*(1-s*b) ;
                float e = v*( 1 - s*(1-b) ) ;

                switch ( (int)i%6 ) {
                    case 0:
                        r = v ;
                        g = e ;
                        b = c ;
                        break ;

                    case 1:
                        r = d ;
                        g = v ;
                        b = c ;
                        break;

                    case 2:
                        r = c;
                        g = v;
                        b = e;
                        break;

                    case 3:
                        r = c; 
                        g = d;
                        b = v;
                        break;

                    case 4:
                        r = e;
                        g = c;
                        b = v;
                        break;

                    case 5:
                        r = v;
                        g = c;
                        b = d;
                        break;
                }
            }

        }
    }

}
