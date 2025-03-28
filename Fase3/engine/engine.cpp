
#include "pugixml.hpp"
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <cstring>
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
    GLuint vbo; //Index do vbo
};
enum Type {
    TRANSLATE,
    ROTATE,
    SCALE
};

struct Transformation {
    Type type;
    float x, y, z, angle = 0;
};

struct File{
    string file;
    float dayPeriod = 0;
    float yearPeriod = 0;
    Model modelo;
};

struct Group {
    list<File> files;
    list<Transformation> transformations;
    list<Group> children;
};

bool simulate = false;
Camera camera;

Group mainGroup;
int buffer = 0;
int maxBuffers = 100;
GLuint buffers[100];

int windowWidth = 800, windowHeight = 600;

Group processGroup(pugi::xml_node groupNode){
    Group group;

    for(pugi::xml_node transformNode = groupNode.child("transform"); transformNode; transformNode = transformNode.next_sibling("transform")){
        Transformation transformation;

        for(pugi::xml_node childNode = transformNode.first_child(); childNode; childNode = childNode.next_sibling()){
            if (strcmp(childNode.name(),"translate") == 0){
                transformation.type = TRANSLATE;
                transformation.x = childNode.attribute("x").as_float();
                transformation.y = childNode.attribute("y").as_float();
                transformation.z = childNode.attribute("z").as_float();
                group.transformations.push_back(transformation);
            }
            else if (strcmp(childNode.name(),"rotate") == 0){
                transformation.type = ROTATE;
                transformation.x = childNode.attribute("x").as_float();
                transformation.y = childNode.attribute("y").as_float();
                transformation.z = childNode.attribute("z").as_float();
                transformation.angle = childNode.attribute("angle").as_float();
                group.transformations.push_back(transformation);
            }
            else if (strcmp(childNode.name(),"scale") == 0){
                transformation.type = SCALE;
                transformation.x = childNode.attribute("x").as_float();
                transformation.y = childNode.attribute("y").as_float();
                transformation.z = childNode.attribute("z").as_float();
                group.transformations.push_back(transformation);
            }
        }
    }

    pugi::xml_node modelsNode = groupNode.child("models");
    for(pugi::xml_node modelNode = modelsNode.child("model"); modelNode; modelNode = modelNode.next_sibling()){
        File modelo;
        Model m;
        modelo.file = modelNode.attribute("file").as_string();
        if (simulate){
            modelo.dayPeriod = modelNode.attribute("dayPeriod").as_float();
            modelo.yearPeriod = modelNode.attribute("yearPeriod").as_float();
        }
        group.files.push_back(modelo);
    }

    pugi::xml_node groupsNode = groupNode.child("group");
    for(pugi::xml_node childGroupNode = groupsNode; childGroupNode; childGroupNode = childGroupNode.next_sibling()){
        Group childGroup = processGroup(childGroupNode);
        group.children.push_back(childGroup);

    }

    return group;
}

void readConfig(const char* filePath) {
    pugi::xml_document doc;
    if (!doc.load_file(filePath)) {
        cerr << "Failed to load XML file. " << filePath << endl;
        exit(1);
    }

    pugi::xml_node root = doc.child("world");

    simulate = root.attribute("simulate").as_bool();

    // Window settings
    pugi::xml_node windowNode = root.child("window");
    if (!windowNode) {
        cerr << "Window node not found." << endl;
        exit(1);
    }
    windowWidth = windowNode.attribute("width").as_int(1920);
    windowHeight = windowNode.attribute("height").as_int(1080);

    // Camera settings
    pugi::xml_node camNode = root.child("camera");
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

    pugi::xml_node projNode = camNode.child("projection");
    camera.fov = projNode.attribute("fov").as_float(60.0f);
    camera.nearPlane = projNode.attribute("near").as_float(0.1f);
    camera.farPlane = projNode.attribute("far").as_float(1000.0f);

    // Model files
    pugi::xml_node groupNode = root.child("group");
    mainGroup = processGroup(groupNode);
}

void drawFigures(Group group){
    for (Transformation transform : group.transformations){
        if (transform.type == TRANSLATE){
            glTranslatef(transform.x, transform.y, transform.z);
        }
        else if (transform.type == ROTATE){
            glRotatef(transform.angle, transform.x, transform.y, transform.z);
        }
        else if (transform.type == SCALE){
            glScalef(transform.x, transform.y, transform.z);
        }
    }

    for (File file : group.files){
        glPushMatrix();

        Model modelo = file.modelo;
        cerr << "beforeBind " << modelo.type <<endl;
        glBindBuffer(GL_ARRAY_BUFFER, buffers[modelo.vbo]);
        cerr << "afterBind" << endl;

        for(int i = 0; i < modelo.dividers.size(); i+=2){
            cerr << "file" << endl;
            if (modelo.faces[i/2] == "GL_TRIANGLE_STRIP") {
                glDrawArrays(GL_TRIANGLE_STRIP, modelo.dividers[i], modelo.dividers[i+1] - modelo.dividers[i]);
            } else if (modelo.faces[i/2] == "GL_TRIANGLE_FAN") {
                glDrawArrays(GL_TRIANGLE_FAN, modelo.dividers[i], modelo.dividers[i+1] - modelo.dividers[i]);
            }
        }

        glPopMatrix();
    }

    for (Group child : group.children){
        glPushMatrix();
        drawFigures(child);
        glPopMatrix();
    }
}

void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glGenBuffers(maxBuffers, buffers);
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
    
    glBegin(GL_LINES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-100.0f, 0.0f, 0.0f);
        glVertex3f(100.0f, 0.0f, 0.0f);

        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, -100.0f, 0.0f);
        glVertex3f(0.0f, 100.0f, 0.0f);

        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, -100.0f);
        glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);

    drawFigures(mainGroup);

    glColor3f(0.5f, 0.8f, 1.0f);
    //glutWireCube(2.0);

    glutSwapBuffers();
}

void processVBO(Group group){
    for (File m : group.files){
        int counter = 0;

        // create path for file
        char filePath[11+strlen(m.file.c_str())] = "";
        strcat(filePath,"../models/");
        strcat(filePath,m.file.c_str());
        ifstream f(filePath);

        string line;
        Model modelo;

        if (f.is_open()) {
            vector<float> p;
            // Read each line from the file and store it in the
            // 'line' variable.
            while (getline(f, line)) {
                if (line == "cone" || line == "box" || line == "plane" || line == "sphere" || line == "torus") {
                    modelo.type = line;
                }
                else if (line[0] == 'G') {
                    modelo.faces.push_back(line);
                    modelo.dividers.push_back(counter);
                }
                else if (line != "") {
                    p.push_back(stof(line));
                    modelo.vertices.push_back(stof(line));
                    counter++;
                }
            }
    
            // Close the file stream once all lines have been
            // read.
            f.close();
            glBindBuffer(GL_ARRAY_BUFFER, buffers[buffer]);
            glBufferData(GL_ARRAY_BUFFER, p.size() * sizeof(float), p.data(), GL_STATIC_DRAW);
            modelo.vbo = buffer;
            cerr << modelo.vbo << endl;
            modelo.numVertices = counter;
            m.modelo = modelo;
            buffer++;
            cerr << buffer << endl;
            if (buffer > maxBuffers){
                cerr << "Max buffers reached" << endl;
                exit(1);
            }

        }
        else {
            // Print an error message to the standard error
            // stream if the file cannot be opened.
            cerr << "Unable to open file!" << endl;
        }
    }

    for (Group child : group.children){
        processVBO(child);
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <config directory here>" << " <config.xml>" << endl;
        return 1;
    }

    if (strcmp(argv[1],"configs") != 0 && strcmp(argv[1],"tests") != 0){
        cerr << "Config directory must be 'configs' or 'tests'" << endl;
        return 1;
    }

    char filePath[strlen(argv[1]) + strlen(argv[2]) + 11] = "../Fase3/";

    strcat(filePath,argv[1]);
    strcat(filePath,"/");
    strcat(filePath,argv[2]);
  
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Engine bombadão");

    glMatrixMode(GL_MODELVIEW);
    glewInit();
    initOpenGL();
    readConfig(filePath);

    processVBO(mainGroup);
    for (File f : mainGroup.files){
        cerr << f.modelo.type << endl;
    }
    glutDisplayFunc(display);
    glutIdleFunc(display);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glutMainLoop();

    return 0;
}
