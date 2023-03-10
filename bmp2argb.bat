REM Use ffmpeg to convert 32bit bmp to ARGB raw data file
ffmpeg -i %1.bmp -f rawvideo -pix_fmt argb -vcodec rawvideo %1.raw

REM Use ffplay to display the raw data file
ffplay -f rawvideo -pixel_format argb -video_size 1920x1080 %1.raw