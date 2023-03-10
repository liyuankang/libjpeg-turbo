// decode a jpeg file into a raw YUV buffer
// gcc -o test test.cpp -ljpeg  -I/usr/local/include -L/usr/local/lib   -ljpeg
// ./test 1.jpg 1.yuv
// Path: test.h
#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

#endif

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s input.jpg output.yuv", argv[0]);
        return -1;
    }

    FILE *fp = fopen(argv[1], "rb");

    // decode jpeg file
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    // set output color space to YUV 4:2:0
    cinfo.out_color_space = JCS_YCbCr;
    jpeg_stdio_src(&cinfo, fp);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    // get image info
    int width = cinfo.output_width;
    int height = cinfo.output_height;
    int pixel_size = cinfo.output_components;
    printf("width: %d, height: %d, pixel_size: %d\\n", width, height, pixel_size);

    // read scanlines one by one
    JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, width * pixel_size, 1);
    FILE *fp_yuv = fopen(argv[2], "wb");
    while (cinfo.output_scanline < cinfo.output_height)
    {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        fwrite(buffer[0], width * pixel_size, 1, fp_yuv);
    }

    // finish decompress
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(fp);
    fclose(fp_yuv);

    return 0;
}