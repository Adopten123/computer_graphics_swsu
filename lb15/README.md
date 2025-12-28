sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev

g++ -o lab15 main.cpp -lGL -lGLU -lglut -lm

./lab15

Нажать S, чтобы файл сохранился
