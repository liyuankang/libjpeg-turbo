#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>
#include <time.h>


int main(int argc, char *argv[]) {
    // convert the second argument to a integer
    int quality = atoi(argv[1]);
    int color_space = atoi(argv[2]);

    unsigned char* pixels = (unsigned char*)malloc(1920 * 1080 * 4);
    unsigned char* out_buf = (unsigned char*)malloc(1920 * 1080 * 2);
    unsigned long out_size = 1920 * 1080 * 2;

    memset(pixels, 0xff, 1920 * 1080 * 4);
    // read the jpeg_test_32bpp.raw file into the pixels array
    FILE* infile = fopen("jpeg_test_32bpp.raw", "rb");
    fread(pixels, 1920 * 1080 * 4, 1, infile);
    fclose(infile);

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    FILE* outfile = fopen("output.jpg", "wb");
    //jpeg_stdio_dest(&cinfo, outfile);
    jpeg_mem_dest(&cinfo, &out_buf, &out_size);
    cinfo.image_width = 1920;
    cinfo.image_height = 1080;
    cinfo.input_components = 4;
    cinfo.in_color_space = JCS_EXT_ARGB;
    jpeg_set_defaults(&cinfo);

    cinfo.jpeg_color_space = JCS_YCbCr;
    if (color_space == 422) {
        // set the sampling factors to 4:2:2
        cinfo.comp_info[0].h_samp_factor = 2;
        cinfo.comp_info[0].v_samp_factor = 1;
        cinfo.comp_info[1].h_samp_factor = 1;
        cinfo.comp_info[1].v_samp_factor = 1;
        cinfo.comp_info[2].h_samp_factor = 1;
        cinfo.comp_info[2].v_samp_factor = 1;
    } else if (color_space == 444) {
        // set the sampling factors to 4:4:4
        cinfo.comp_info[0].h_samp_factor = 1;
        cinfo.comp_info[0].v_samp_factor = 1;
        cinfo.comp_info[1].h_samp_factor = 1;
        cinfo.comp_info[1].v_samp_factor = 1;
        cinfo.comp_info[2].h_samp_factor = 1;
        cinfo.comp_info[2].v_samp_factor = 1;
    }

    // insert the MCU restart marker every block
    cinfo.restart_interval = 0;
    cinfo.restart_in_rows = 0;
    jpeg_set_quality(&cinfo, quality, TRUE);

    // Print the time consumed by the jpeg_start_compress function
    clock_t start, end;
    double time_elapsed;
    start = clock();

    for (int i=0; i<600; i++) {
        jpeg_mem_dest(&cinfo, &out_buf, &out_size);
        jpeg_start_compress(&cinfo, TRUE);
        JSAMPROW row_pointer[1];
        int rows_written = 1;
        for (int y = 0; y < cinfo.image_height; y+=rows_written) {
            row_pointer[0] = &pixels[y * cinfo.image_width * cinfo.input_components];
            jpeg_write_scanlines(&cinfo, row_pointer, rows_written);
        }
        jpeg_finish_compress(&cinfo);
    }

    end = clock();
    time_elapsed = ((double) (end - start));
    printf("YUV %d, Quality: %d\n", color_space, quality);
    printf("Time elapsed: %f ms, %f ms per frame, %f fps\n", time_elapsed, 
                                                             time_elapsed/600, 600000/time_elapsed);
    printf("Output size: %lu bytes\n", out_size);
    fwrite(out_buf, out_size, 1, outfile);
    free(pixels);
    free(out_buf);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
    return 0;
}
