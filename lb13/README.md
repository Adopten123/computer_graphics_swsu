1) sudo apt update
2) sudo apt-get install build-essential libx11-dev cimg-dev
3) wget https://raw.githubusercontent.com/dtschump/CImg/master/CImg.h
4) g++ -o lb13 main.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11
5) /clipping_fill
