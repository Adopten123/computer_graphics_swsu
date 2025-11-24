./processor ../inputed_image.jpg out_scale_nearest.png  scale nearest  2.0 2.0
./processor ../inputed_image.jpg out_scale_bilinear.png scale bilinear 2.0 2.0
./processor ../inputed_image.jpg out_scale_bicubic.png  scale bicubic  2.0 2.0

./processor ../inputed_image.jpg out_rot_nearest.png  rotate nearest  45
./processor ../inputed_image.jpg out_rot_bilinear.png rotate bilinear 45
./processor ../inputed_image.jpg out_rot_bicubic.png  rotate bicubic  45

./processor ../inputed_image.jpg out_shearx_nearest.png  shearx nearest 0.5
./processor ../inputed_image.jpg out_shearx_bicubic.png  shearx bicubic 0.5
./processor ../inputed_image.jpg out_sheary_bilinear.png sheary bilinear 0.3
