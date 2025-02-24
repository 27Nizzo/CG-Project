
#include "pugixml.hpp"
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

using namespace std;
//using namespace glm;

struct Camera {
    vector<float> position;
    vector<float> lookAt;
    vector<float> up;
    float fov, nearPlane, farPlane;
};

Camera camera;
vector<string> modelFiles;
int windowWidth = 800, windowHeight = 600;

void loadConfig(const char* filePath) {
    pugi::xml_document doc;
    if (!doc.load_file(filePath)) {
        cerr << "Failed to load XML file." << endl;
        exit(1);
    }

    auto root = doc.child("world");

    // Window settings
    auto windowNode = root.child("resolution");
    if (!windowNode) {
        cerr << "Window node not found." << endl;
        exit(1);
    }
    windowWidth = windowNode.attribute("width").as_int(1920);
    windowHeight = windowNode.attribute("height").as_int(1080);

    // Camera settings
    auto camNode = root.child("camera");
    if (!camNode) {
        cerr << "Camera node not found." << endl;
        exit(1);
    }
    camera.position.push_back(camNode.child("position").attribute("x").as_float());
    camera.position.push_back(camNode.child("position").attribute("y").as_float());
    camera.position.push_back(camNode.child("position").attribute("z").as_float());

    camera.lookAt.push_back(camNode.child("lookAt").attribute("x").as_float());
    camera.lookAt.push_back(camNode.child("lookAt").attribute("y").as_float());
    camera.lookAt.push_back(camNode.child("lookAt").attribute("z").as_float());

    camera.up.push_back(camNode.child("up").attribute("x").as_float());
    camera.up.push_back(camNode.child("up").attribute("y").as_float());
    camera.up.push_back(camNode.child("up").attribute("z").as_float());

    auto projNode = camNode.child("projection");
    camera.fov = projNode.attribute("fov").as_float(60.0f);
    camera.nearPlane = projNode.attribute("near").as_float(0.1f);
    camera.farPlane = projNode.attribute("far").as_float(1000.0f);

    // Model files
    for (auto model : root.child("models").children("model")) {
        modelFiles.push_back(model.attribute("file").as_string());
    }
}

void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(camera.fov, (float)windowWidth / windowHeight, camera.nearPlane, camera.farPlane);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.position[0], camera.position[1], camera.position[2],
              camera.lookAt[0], camera.lookAt[1], camera.lookAt[2],
              camera.up[0], camera.up[1], camera.up[2]);

    // Desenha um cubo simples como placeholder
    glColor3f(0.5f, 0.8f, 1.0f);
    glutWireCube(2.0);

    glutSwapBuffers();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <config.xml>" << endl;
        return 1;
    }

    loadConfig(argv[1]);

  
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("3D Engine with pugixml");

    glewInit();
    initOpenGL();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}
