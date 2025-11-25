./processor ../inputed_image.jpg out_scale_nearest.jpg scale nearest 2.0 2.0
./processor ../inputed_image.jpg out_scale_bilinear.jpg scale bilinear 2.0 2.0
./processor ../inputed_image.jpg out_scale_bicubic.jpg scale bicubic 2.0 2.0

./processor ../inputed_image.jpg out_rotate_nearest.jpg rotate nearest 45
./processor ../inputed_image.jpg out_rotate_bilinear.jpg rotate bilinear 45
./processor ../inputed_image.jpg out_rotate_bicubic.jpg rotate bicubic 45

./processor ../inputed_image.jpg out_shearx_nearest.jpg shearx nearest 0.5
./processor ../inputed_image.jpg out_shearx_bicubic.jpg shearx bicubic 0.5
./processor ../inputed_image.jpg out_sheary_bilinear.jpg sheary bilinear 0.3