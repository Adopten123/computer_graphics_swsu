1) sudo apt update
2) sudo apt install build-essential freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev
3) g++ main.cpp -o lab14 -lGL -lGLU -lglut
4) ./lab14

Управление:
    1. Стрелки: Вращение/перемещение камеры.
    2. Пробел: Переключение между кубом и призмой.
    3. + / -: Увеличение и уменьшение масштаба.
    4. 1, 2, 3: Смена метода расчета (3 точки, нормаль, Ньюэл).
    5. Esc: Выход.