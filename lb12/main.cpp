#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <cmath>

const int WIDTH = 1024;
const int HEIGHT = 768;
float rotateX = 30.0f, rotateY = -30.0f;
int subdivisionLevel = 10;
int dooSabinIterations = 0;
bool showControlPoints = true;
bool showSurface = true;
int currentMode = 1;

using Point3D = glm::vec3;

long long factorial(int n) {
    return (n <= 1) ? 1 : n * factorial(n - 1);
}

float binomialCoeff(int n, int k) {
    return (float)(factorial(n) / (factorial(k) * factorial(n - k)));
}

float bernstein(int i, int n, float t) {
    return binomialCoeff(n, i) * pow(t, i) * pow(1.0f - t, n - i);
}

class BezierSurface {
public:
    std::vector<std::vector<Point3D>> controlPoints;

    BezierSurface() {
        controlPoints.resize(4, std::vector<Point3D>(4));
        float startX = -1.5f, startZ = -1.5f;
        for(int i=0; i<4; i++) {
            for(int j=0; j<4; j++) {
                controlPoints[i][j] = Point3D(startX + i, ((i+j)%2 == 0 ? 1.5f : -0.5f), startZ + j);
            }
        }
    }

    Point3D calculatePoint(float u, float v) {
        Point3D p(0.0f);
        int n = controlPoints.size() - 1;
        int m = controlPoints[0].size() - 1;
        for(int i=0; i <= n; i++) {
            float b_i = bernstein(i, n, u);
            for(int j=0; j <= m; j++) {
                float b_j = bernstein(j, m, v);
                p += controlPoints[i][j] * b_i * b_j;
            }
        }
        return p;
    }

    void draw(int resolution) {
        if(showControlPoints) {
            glColor3f(1.0f, 0.0f, 0.0f);
            glPointSize(8.0f);
            glBegin(GL_POINTS);
            for(auto& row : controlPoints) for(auto& p : row) glVertex3f(p.x, p.y, p.z);
            glEnd();

            glColor3f(0.5f, 0.0f, 0.0f);
            glBegin(GL_LINES);
            for(int i=0; i<controlPoints.size(); i++) {
                for(int j=0; j<controlPoints[i].size(); j++) {
                    if(i < (int)controlPoints.size()-1) {
                        glVertex3f(controlPoints[i][j].x, controlPoints[i][j].y, controlPoints[i][j].z);
                        glVertex3f(controlPoints[i+1][j].x, controlPoints[i+1][j].y, controlPoints[i+1][j].z);
                    }
                    if(j < (int)controlPoints[i].size()-1) {
                        glVertex3f(controlPoints[i][j].x, controlPoints[i][j].y, controlPoints[i][j].z);
                        glVertex3f(controlPoints[i][j+1].x, controlPoints[i][j+1].y, controlPoints[i][j+1].z);
                    }
                }
            }
            glEnd();
        }

        if(showSurface) {
            glColor3f(0.0f, 1.0f, 1.0f);
            for(int i=0; i < resolution; i++) {
                for(int j=0; j < resolution; j++) {
                    float u = (float)i / resolution;
                    float v = (float)j / resolution;
                    float u_next = (float)(i+1) / resolution;
                    float v_next = (float)(j+1) / resolution;
                    Point3D p1 = calculatePoint(u, v);
                    Point3D p2 = calculatePoint(u_next, v);
                    Point3D p3 = calculatePoint(u_next, v_next);
                    Point3D p4 = calculatePoint(u, v_next);
                    glBegin(GL_LINE_LOOP);
                    glVertex3f(p1.x, p1.y, p1.z); glVertex3f(p2.x, p2.y, p2.z);
                    glVertex3f(p3.x, p3.y, p3.z); glVertex3f(p4.x, p4.y, p4.z);
                    glEnd();
                }
            }
        }
    }
};

struct Face { std::vector<Point3D> vertices; };

class DooSabinMesh {
public:
    std::vector<Face> faces;
    DooSabinMesh() {
        Point3D v[8] = {{-1,-1, 1}, { 1,-1, 1}, { 1, 1, 1}, {-1, 1, 1}, {-1,-1,-1}, { 1,-1,-1}, { 1, 1,-1}, {-1, 1,-1}};
        faces.push_back({{v[0], v[1], v[2], v[3]}}); faces.push_back({{v[5], v[4], v[7], v[6]}});
        faces.push_back({{v[4], v[0], v[3], v[7]}}); faces.push_back({{v[1], v[5], v[6], v[2]}});
        faces.push_back({{v[3], v[2], v[6], v[7]}}); faces.push_back({{v[4], v[5], v[1], v[0]}});
    }

    void subdivide() {
        std::vector<Face> newFaces;
        for (const auto& face : faces) {
            int n = face.vertices.size();
            Point3D faceCenter(0.0f);
            for(const auto& p : face.vertices) faceCenter += p;
            faceCenter /= (float)n;
            std::vector<Point3D> newInnerVertices;
            for (int i = 0; i < n; ++i) {
                Point3D P = face.vertices[i];
                Point3D P_prev = face.vertices[(i - 1 + n) % n];
                Point3D P_next = face.vertices[(i + 1) % n];
                Point3D midPrev = (P + P_prev) * 0.5f;
                Point3D midNext = (P + P_next) * 0.5f;
                newInnerVertices.push_back((P + midPrev + midNext + faceCenter) * 0.25f);
            }
            newFaces.push_back({newInnerVertices});
        }
        faces = newFaces;
    }

    void reset() { faces.clear(); *this = DooSabinMesh(); }

    void draw() {
        if(showSurface) {
            glColor3f(0.0f, 1.0f, 0.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            for(const auto& face : faces) {
                glBegin(GL_POLYGON);
                for(const auto& v : face.vertices) glVertex3f(v.x, v.y, v.z);
                glEnd();
            }
        }
        if(showControlPoints) {
            glColor3f(1.0f, 1.0f, 0.0f);
            glPointSize(5.0f);
            glBegin(GL_POINTS);
            for(const auto& face : faces) for(const auto& v : face.vertices) glVertex3f(v.x, v.y, v.z);
            glEnd();
        }
    }
};

BezierSurface bezier;
DooSabinMesh dooSabin;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_1) currentMode = 1;
        if (key == GLFW_KEY_2) currentMode = 2;
        if (key == GLFW_KEY_UP) {
            if(currentMode == 1) subdivisionLevel += 2;
            else { dooSabin.subdivide(); }
        }
        if (key == GLFW_KEY_DOWN) {
            if(currentMode == 1 && subdivisionLevel > 2) subdivisionLevel -= 2;
            else dooSabin.reset();
        }
        if (key == GLFW_KEY_Z) showControlPoints = !showControlPoints;
        if (key == GLFW_KEY_X) showSurface = !showSurface;
    }
}

void setupProjection(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)width / (float)height;
    float fH = tan(45.0f / 360.0f * 3.14159f) * 0.1f;
    float fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Bezier & Doo-Sabin Lab", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();
    glfwSetKeyCallback(window, key_callback);
    glEnable(GL_DEPTH_TEST);
    setupProjection(WIDTH, HEIGHT);
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -8.0f);
        glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
        glRotatef((float)glfwGetTime() * 15.0f, 0.0f, 1.0f, 0.0f);
        if(currentMode == 1) bezier.draw(subdivisionLevel);
        else dooSabin.draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}