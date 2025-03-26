
#include "pugixml.hpp"
#include <iostream>
#include <fstream>
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

struct Model {
    string type; //Define a primitiva que representa o modelo
    vector<float> vertices; //Guarda os vértices do modelo
    vector<int> dividers; //Guarda onde começa cada face do modelo
    vector<string> faces; //Guarda o tipo de face do modelo
    int numVertices; //Guarda o número de vértices do modelo
};

Camera camera;
vector<Model> modelFiles;
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
    auto modelNode = root.child("models");
    if (modelNode) {
        for (auto model : modelNode.children("model")) {
            int counter = 0;
            ifstream file(model.text().as_string());

            string line;
            Model modelo;

            if (file.is_open()) {
                // Read each line from the file and store it in the
                // 'line' variable.
                while (getline(file, line)) {
                    if (line == "cone" || line == "box" || line == "plane" || line == "sphere") {
                        modelo.type = line;
                    }
                    else if (line[0] == 'G') {
                        modelo.faces.push_back(line);
                        modelo.dividers.push_back(counter);
                    }
                    else if (line != ""){
                        modelo.vertices.push_back(stof(line));
                        counter++;
                    }
                }
        
                // Close the file stream once all lines have been
                // read.
                file.close();
                modelo.numVertices = counter;
            }
            else {
                // Print an error message to the standard error
                // stream if the file cannot be opened.
                cerr << "Unable to open file!" << endl;
            }
            modelFiles.push_back(modelo);
        }
    }
    else {
        cerr << "No models found." << endl;
        //exit(1);
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
    
    // uncomment for referencial axis
    /*glBegin(GL_LINES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-100.0f, 0.0f, 0.0f);
        glVertex3f(100.0f, 0.0f, 0.0f);

        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, -100.0f, 0.0f);
        glVertex3f(0.0f, 100.0f, 0.0f);

        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, -100.0f);
        glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();*/

    glColor3f(1.0f, 1.0f, 1.0f);
    int draws = modelFiles.size();
    for (int i = 0; i < draws; i++) {
        if (modelFiles[i].type == "plane") {
            for(int j = 0; j < modelFiles[i].dividers.size(); j+=2){
                glBegin(GL_TRIANGLE_STRIP);
                for (int k = modelFiles[i].dividers[j]; k < modelFiles[i].dividers[j+1]; k += 3) {
                    glVertex3f(modelFiles[i].vertices[k], modelFiles[i].vertices[k + 1], modelFiles[i].vertices[k + 2]);
                }
                glEnd();
            }
        }
        else if (modelFiles[i].type == "box") {
            for(int j = 0; j < modelFiles[i].dividers.size(); j+=2){
                glBegin(GL_TRIANGLE_STRIP);
                for (int k = modelFiles[i].dividers[j]; k < modelFiles[i].dividers[j+1]; k += 3) {
                    glVertex3f(modelFiles[i].vertices[k], modelFiles[i].vertices[k + 1], modelFiles[i].vertices[k + 2]);
                }
                glEnd();
            }
        }
        else if (modelFiles[i].type == "cone") {
            for(int j = 0; j < modelFiles[i].dividers.size(); j+=2){
                if (modelFiles[i].faces[j] == "GL_TRIANGLE_FAN") {
                    glBegin(GL_TRIANGLE_FAN);
                } else if (modelFiles[i].faces[j] == "GL_TRIANGLE_STRIP") {
                    glBegin(GL_TRIANGLE_STRIP);
                }
                for (int k = modelFiles[i].dividers[j]; k < modelFiles[i].dividers[j+1]; k += 3) {
                    glVertex3f(modelFiles[i].vertices[k], modelFiles[i].vertices[k + 1], modelFiles[i].vertices[k + 2]);
                }

                glEnd();
            }
        }
        else if (modelFiles[i].type == "sphere") {
            for(int j = 0; j < modelFiles[i].dividers.size(); j+=2){
                glBegin(GL_TRIANGLE_STRIP);
                for (int k = modelFiles[i].dividers[j]; k < modelFiles[i].dividers[j+1]; k += 3) {
                    glVertex3f(modelFiles[i].vertices[k], modelFiles[i].vertices[k + 1], modelFiles[i].vertices[k + 2]);
                }

                glEnd();
            }
        }
    }
    glColor3f(0.5f, 0.8f, 1.0f);
    //glutWireCube(2.0);

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
    glutCreateWindow("Engine bombadão");

    glewInit();
    initOpenGL();
    glutDisplayFunc(display);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glutMainLoop();

    return 0;
}
