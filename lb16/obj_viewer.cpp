#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <map>

struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

struct Material {
    std::string name;
    Vec3 Ka;
    Vec3 Kd;
    Vec3 Ks;
    float Ns;
    float d;
    Material() : Ns(0), d(1.0f) {}
};

struct Face {
    std::vector<int> vertexIndices;
    std::string materialName;
};

struct Model {
    std::string name;
    std::vector<Vec3> vertices;
    std::vector<Face> faces;
    std::map<std::string, Material> materials;
    std::string currentMaterial;
};

Model loadedModel;
float scale = 1.0f;
float camX = 4.0f, camY = 3.0f, camZ = 10.0f;
float rotX = 0.0f, rotY = 0.0f;
int mouseX = 0, mouseY = 0;
bool mouseLeftDown = false;

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

bool loadMTL(const std::string& filename, const std::string& basePath, std::map<std::string, Material>& materials) {
    std::string fullPath = basePath + filename;
    std::ifstream file(fullPath);
    
    if (!file.is_open()) {
        std::cerr << "Ошибка: невозможно открыть файл " << fullPath << std::endl;
        return false;
    }
    
    Material currentMat;
    std::string line;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        
        if (token == "newmtl") {
            if (!currentMat.name.empty()) {
                materials[currentMat.name] = currentMat;
            }
            iss >> currentMat.name;
            currentMat = Material();
            currentMat.name = currentMat.name;
        }
        else if (token == "Ka") {
            iss >> currentMat.Ka.x >> currentMat.Ka.y >> currentMat.Ka.z;
        }
        else if (token == "Kd") {
            iss >> currentMat.Kd.x >> currentMat.Kd.y >> currentMat.Kd.z;
        }
        else if (token == "Ks") {
            iss >> currentMat.Ks.x >> currentMat.Ks.y >> currentMat.Ks.z;
        }
        else if (token == "Ns") {
            iss >> currentMat.Ns;
        }
        else if (token == "d" || token == "Tr") {
            iss >> currentMat.d;
        }
    }
    
    if (!currentMat.name.empty()) {
        materials[currentMat.name] = currentMat;
    }
    
    file.close();
    std::cout << "Загружено материалов: " << materials.size() << std::endl;
    return true;
}

bool loadOBJ(const std::string& filename) {
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Ошибка: невозможно открыть файл " << filename << std::endl;
        return false;
    }
    
    loadedModel = Model();
    loadedModel.name = filename;
    loadedModel.currentMaterial = "";
    
    std::string basePath = "";
    size_t lastSlash = filename.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        basePath = filename.substr(0, lastSlash + 1);
    }
    
    std::string line;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        
        try {
            if (token == "v") {
                float x, y, z;
                if (!(iss >> x >> y >> z)) {
                    std::cerr << "Ошибка в строке " << lineNum << ": неверный формат вершины" << std::endl;
                    continue;
                }
                loadedModel.vertices.push_back(Vec3(x, y, z));
            }
            else if (token == "mtllib") {
                std::string mtlFile;
                iss >> mtlFile;
                if (!loadMTL(mtlFile, basePath, loadedModel.materials)) {
                    std::cerr << "Предупреждение: не удалось загрузить материалы из " << mtlFile << std::endl;
                }
            }
            else if (token == "usemtl") {
                iss >> loadedModel.currentMaterial;
            }
            else if (token == "f") {
                Face face;
                face.materialName = loadedModel.currentMaterial;
                
                std::string vertexStr;
                while (iss >> vertexStr) {
                    size_t slash1 = vertexStr.find('/');
                    if (slash1 != std::string::npos) {
                        vertexStr = vertexStr.substr(0, slash1);
                    }
                    
                    int idx = std::stoi(vertexStr);
                    if (idx < 0) {
                        idx = loadedModel.vertices.size() + idx + 1;
                    }
                    face.vertexIndices.push_back(idx - 1);
                }
                
                if (face.vertexIndices.size() >= 3) {
                    loadedModel.faces.push_back(face);
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка в строке " << lineNum << ": " << e.what() << std::endl;
        }
    }
    
    file.close();
    
    std::cout << "Файл загружен: " << filename << std::endl;
    std::cout << "Вершин: " << loadedModel.vertices.size() << std::endl;
    std::cout << "Граней: " << loadedModel.faces.size() << std::endl;
    std::cout << "Материалов: " << loadedModel.materials.size() << std::endl;
    
    return true;
}

void drawModel() {
    if (loadedModel.vertices.empty()) return;
    
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(2, 0, 0);
    glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 2, 0);
    glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 2);
    glEnd();
    glLineWidth(1.0f);
    
    for (const auto& face : loadedModel.faces) {
        Material mat;
        auto it = loadedModel.materials.find(face.materialName);
        if (it != loadedModel.materials.end()) {
            mat = it->second;
        } else {
            mat.Kd = Vec3(0.7f, 0.7f, 0.7f);
        }
        
        glColor3f(mat.Kd.x, mat.Kd.y, mat.Kd.z);
        
        glBegin(GL_POLYGON);
        for (int idx : face.vertexIndices) {
            if (idx >= 0 && idx < (int)loadedModel.vertices.size()) {
                glVertex3f(loadedModel.vertices[idx].x,
                          loadedModel.vertices[idx].y,
                          loadedModel.vertices[idx].z);
            }
        }
        glEnd();
        
        glColor3f(0.2f, 0.2f, 0.2f);
        glBegin(GL_LINE_LOOP);
        for (int idx : face.vertexIndices) {
            if (idx >= 0 && idx < (int)loadedModel.vertices.size()) {
                glVertex3f(loadedModel.vertices[idx].x,
                          loadedModel.vertices[idx].y,
                          loadedModel.vertices[idx].z);
            }
        }
        glEnd();
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    gluLookAt(camX, camY, camZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glScalef(scale, scale, scale);
    
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_LINES);
    for (float i = -10; i <= 10; i += 1.0f) {
        glVertex3f(i, 0, -10); glVertex3f(i, 0, 10);
        glVertex3f(-10, 0, i); glVertex3f(10, 0, i);
    }
    glEnd();
    
    drawModel();
    
    glColor3f(1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    std::string info = "OBJ Viewer: " + loadedModel.name;
    glRasterPos2i(10, 580);
    for(char c : info) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    
    std::string stats = "Vertices: " + std::to_string(loadedModel.vertices.size()) +
                       ", Faces: " + std::to_string(loadedModel.faces.size()) +
                       ", Materials: " + std::to_string(loadedModel.materials.size());
    glRasterPos2i(10, 560);
    for(char c : stats) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    
    std::string controls = "Controls: +/- zoom, LMB drag rotate, R reset";
    glRasterPos2i(10, 540);
    for(char c : controls) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case '+': scale *= 1.1f; break;
        case '-': scale /= 1.1f; break;
        case 'r':
        case 'R':
            scale = 1.0f;
            rotX = rotY = 0.0f;
            camX = 4.0f;
            camY = 3.0f;
            camZ = 10.0f;
            break;
        case 'o':
        case 'O': {
            std::cout << "Введите имя файла OBJ для загрузки: ";
            std::string filename;
            std::cin >> filename;
            loadOBJ(filename);
            break;
        }
        case 27: exit(0); break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseLeftDown = true;
            mouseX = x;
            mouseY = y;
        } else if (state == GLUT_UP) {
            mouseLeftDown = false;
        }
    }
}

void mouseMotion(int x, int y) {
    if (mouseLeftDown) {
        rotY += (x - mouseX) * 0.5f;
        rotX += (y - mouseY) * 0.5f;
        mouseX = x;
        mouseY = y;
        glutPostRedisplay();
    }
}

void specialKeys(int key, int x, int y) {
    float speed = 0.5f;
    switch (key) {
        case GLUT_KEY_UP: camY += speed; break;
        case GLUT_KEY_DOWN: camY -= speed; break;
        case GLUT_KEY_LEFT: camX -= speed; break;
        case GLUT_KEY_RIGHT: camX += speed; break;
    }
    glutPostRedisplay();
}

void printUsage() {
    std::cout << "=== OBJ File Viewer with Materials ===\n";
    std::cout << "Usage:\n";
    std::cout << "  Drag with left mouse button - rotate model\n";
    std::cout << "  +/- - zoom in/out\n";
    std::cout << "  Arrow keys - move camera\n";
    std::cout << "  R - reset view\n";
    std::cout << "  O - load OBJ file\n";
    std::cout << "  ESC - exit\n";
    std::cout << "\nЗагрузка файла по умолчанию: cube.obj\n";
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OBJ File Viewer with Materials");
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    
    std::string filename = "cube.obj";
    if (argc > 1) {
        filename = argv[1];
    }
    
    if (!loadOBJ(filename)) {
        std::cerr << "Не удалось загрузить файл по умолчанию. Используйте O для загрузки другого файла.\n";
    }
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    
    printUsage();
    
    glutMainLoop();
    return 0;
}