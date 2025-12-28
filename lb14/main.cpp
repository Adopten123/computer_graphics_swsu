#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <string>
#include <cstdlib>

struct Vec3 {
    float x, y, z;
    Vec3 operator+(const Vec3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vec3 operator-(const Vec3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
};

struct Plane {
    float a, b, c, d;
};

struct Face {
    std::vector<int> vertexIndices;
    Plane plane;
    bool isVisible;
    Vec3 normal;
    Vec3 center;
};

struct Object3D {
    std::vector<Vec3> vertices;
    std::vector<Face> faces;
    Vec3 internalPoint;
};

float scale = 1.0f;
float camX = 4.0f, camY = 3.0f, camZ = 10.0f;
int currentObjectIndex = 0;
std::vector<Object3D> objects;
int selectedMethod = 3;

float dotProduct(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 crossProduct(Vec3 a, Vec3 b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

Vec3 normalize(Vec3 v) {
    float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len == 0) return {0, 0, 0};
    return {v.x / len, v.y / len, v.z / len};
}

Plane calculatePlaneThreePoints(Vec3 p1, Vec3 p2, Vec3 p3) {
    Vec3 v1 = p2 - p1;
    Vec3 v2 = p3 - p1;
    Vec3 n = crossProduct(v1, v2);
    float d = -(n.x * p1.x + n.y * p1.y + n.z * p1.z);
    return {n.x, n.y, n.z, d};
}

Plane calculatePlaneNormalPoint(Vec3 p1, Vec3 p2, Vec3 p3) {
    Vec3 v1 = p2 - p1;
    Vec3 v2 = p3 - p1;
    Vec3 n = normalize(crossProduct(v1, v2));
    float d = -(n.x * p1.x + n.y * p1.y + n.z * p1.z);
    return {n.x, n.y, n.z, d};
}

Plane calculatePlaneNewell(const std::vector<Vec3>& verts) {
    float a = 0, b = 0, c = 0;
    int n = verts.size();
    
    for (int i = 0; i < n; i++) {
        int next = (i + 1) % n;
        a += (verts[i].y - verts[next].y) * (verts[i].z + verts[next].z);
        b += (verts[i].z - verts[next].z) * (verts[i].x + verts[next].x);
        c += (verts[i].x - verts[next].x) * (verts[i].y + verts[next].y);
    }
    
    float d = -(a * verts[0].x + b * verts[0].y + c * verts[0].z);
    return {a, b, c, d};
}

void updateObjectVisibility(Object3D& obj) {
    for (auto& face : obj.faces) {
        std::vector<Vec3> faceVerts;
        Vec3 center = {0,0,0};
        for (int idx : face.vertexIndices) {
            faceVerts.push_back(obj.vertices[idx]);
            center = center + obj.vertices[idx];
        }
        face.center = center * (1.0f / faceVerts.size());

        if (selectedMethod == 1 && faceVerts.size() >= 3)
            face.plane = calculatePlaneThreePoints(faceVerts[0], faceVerts[1], faceVerts[2]);
        else if (selectedMethod == 2 && faceVerts.size() >= 3)
            face.plane = calculatePlaneNormalPoint(faceVerts[0], faceVerts[1], faceVerts[2]);
        else
            face.plane = calculatePlaneNewell(faceVerts);

        float val = face.plane.a * obj.internalPoint.x + 
                    face.plane.b * obj.internalPoint.y + 
                    face.plane.c * obj.internalPoint.z + 
                    face.plane.d;
        
        if (val < 0) {
            face.plane.a = -face.plane.a;
            face.plane.b = -face.plane.b;
            face.plane.c = -face.plane.c;
            face.plane.d = -face.plane.d;
        }

        face.normal = normalize({face.plane.a, face.plane.b, face.plane.c});

        Vec3 viewVector = {camX - face.center.x, camY - face.center.y, camZ - face.center.z};
        float dot = dotProduct(normalize(viewVector), face.normal);
        
        if (dot < 0) { 
            face.isVisible = true;
        } else {
            face.isVisible = false;
        }
    }
}

void initObjects() {
    Object3D cube;
    cube.vertices = {
        {-1,-1, 1}, { 1,-1, 1}, { 1, 1, 1}, {-1, 1, 1},
        {-1,-1,-1}, { 1,-1,-1}, { 1, 1,-1}, {-1, 1,-1}
    };
    cube.internalPoint = {0, 0, 0};
    cube.faces = {
        {{0, 1, 2, 3}, {}, false},
        {{1, 5, 6, 2}, {}, false},
        {{5, 4, 7, 6}, {}, false},
        {{4, 0, 3, 7}, {}, false},
        {{3, 2, 6, 7}, {}, false},
        {{4, 5, 1, 0}, {}, false}
    };
    objects.push_back(cube);

    Object3D prism;
    prism.vertices = {
        {-1, -1, 1}, {1, -1, 1}, {0, 1, 1},
        {-1, -1, -1}, {1, -1, -1}, {0, 1, -1}
    };
    prism.internalPoint = {0, 0, 0};
    prism.faces = {
        {{0, 1, 2}, {}, false},
        {{3, 5, 4}, {}, false},
        {{1, 4, 5, 2}, {}, false},
        {{0, 2, 5, 3}, {}, false},
        {{0, 3, 4, 1}, {}, false}
    };
    objects.push_back(prism);
}

void drawGrid() {
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    for (float i = -10; i <= 10; i += 1.0f) {
        glVertex3f(i, 0, -10); glVertex3f(i, 0, 10);
        glVertex3f(-10, 0, i); glVertex3f(10, 0, i);
    }
    glEnd();
}

void drawAxes() {
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(5, 0, 0);
    glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 5, 0);
    glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 5);
    glEnd();
    glLineWidth(1.0f);
}

void drawObject(const Object3D& obj) {
    for (const auto& face : obj.faces) {
        if (face.isVisible) {
            glColor3f(0.0f, 0.7f, 0.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
            glColor3f(1.0f, 0.0f, 0.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        glBegin(GL_POLYGON);
        for (int idx : face.vertexIndices) {
            glVertex3f(obj.vertices[idx].x, obj.vertices[idx].y, obj.vertices[idx].z);
        }
        glEnd();
        
        if (face.isVisible) glColor3f(1,1,0); else glColor3f(0.3, 0, 0);
        glBegin(GL_LINES);
        glVertex3f(face.center.x, face.center.y, face.center.z);
        glVertex3f(face.center.x + face.normal.x*0.5, 
                   face.center.y + face.normal.y*0.5, 
                   face.center.z + face.normal.z*0.5);
        glEnd();
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    gluLookAt(camX, camY, camZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    glScalef(scale, scale, scale);

    drawGrid();
    drawAxes();

    updateObjectVisibility(objects[currentObjectIndex]);
    drawObject(objects[currentObjectIndex]);

    glColor3f(1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    std::string text = "Method (1,2,3): " + std::to_string(selectedMethod);
    glRasterPos2i(10, 580);
    for(char c : text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    
    std::string objText = "Object (Space): " + std::to_string(currentObjectIndex);
    glRasterPos2i(10, 560);
    for(char c : objText) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

    std::string info = "Green: Visible, Red: Invisible";
    glRasterPos2i(10, 540);
    for(char c : info) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

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
        case '1': selectedMethod = 1; break;
        case '2': selectedMethod = 2; break;
        case '3': selectedMethod = 3; break;
        case '+': scale += 0.1f; break;
        case '-': scale -= 0.1f; break;
        case ' ': currentObjectIndex = (currentObjectIndex + 1) % objects.size(); break;
        case 27: exit(0); break;
    }
    glutPostRedisplay();
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

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Lab 2: Ubuntu");

    glEnable(GL_DEPTH_TEST);
    
    initObjects();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}