
#include "pugixml.hpp"
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <cstring>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
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
enum Type {
    TRANSLATE,
    ROTATE,
    SCALE
};

struct Transformation {
    Type type;
    float x, y, z, angle = 0;
};

struct Group {
    list<string> files;
    list<Transformation> transformations;
    list<Group> children;
};

Camera camera;
Group mainGroup;
int windowWidth = 800, windowHeight = 600;
float cam_x = 1, cam_y = 1, cam_z = 1;
float raio_cam = 0, raio_change = 0;
float alpha_cam = 0, beta_cam = 0;

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
        group.files.push_back(modelNode.attribute("file").as_string());
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
        cerr << "Failed to load XML file." << endl;
        exit(1);
    }

    pugi::xml_node root = doc.child("world");

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

void drawFigures(Group group, int& index){
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

    for (string file : group.files){
        int counter = 0;

        // create path for file
        char filePath[11+strlen(file.c_str())] = "";
        strcat(filePath,"../models/");
        strcat(filePath,file.c_str());
        ifstream f(filePath);

        string line;
        Model modelo;

        if (f.is_open()) {
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
                    modelo.vertices.push_back(stof(line));
                    counter++;
                }
            }
    
            // Close the file stream once all lines have been
            // read.
            f.close();
            modelo.numVertices = counter;

            if (modelo.type == "plane" || modelo.type == "box" || modelo.type == "sphere" || modelo.type == "torus") {
                for(int j = 0; j < modelo.dividers.size(); j+=2){
                    glBegin(GL_TRIANGLE_STRIP);
                    for (int k = modelo.dividers[j]; k < modelo.dividers[j+1]; k += 3) {
                        glVertex3f(modelo.vertices[k], modelo.vertices[k + 1], modelo.vertices[k + 2]);
                    }
    
                    glEnd();
                }
            }
            else if (modelo.type == "cone") {
                for(int j = 0; j < modelo.dividers.size()-1; j+=2){
                    if (modelo.faces[j] == "GL_TRIANGLE_FAN") {
                        glBegin(GL_TRIANGLE_FAN);
                    } else if (modelo.faces[j] == "GL_TRIANGLE_STRIP") {
                        glBegin(GL_TRIANGLE_STRIP);
                    }
                    for (int k = modelo.dividers[j]; k < modelo.dividers[j+1]; k += 3) {
                        //cerr << "entered3" << " " << j << " " << modelo.dividers.size() << " " << k << " " << modelo.dividers[j+1] << endl;
                        glVertex3f(modelo.vertices[k], modelo.vertices[k + 1], modelo.vertices[k + 2]);
                    }
    
                    glEnd();
                }
            }
        }
        else {
            // Print an error message to the standard error
            // stream if the file cannot be opened.
            cerr << "Unable to open file!" << endl;
        }
    }

    for (Group child : group.children){
        glPushMatrix();
        drawFigures(child,index);
        glPopMatrix();
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

    cam_x = camera.position[0];
    cam_y = camera.position[1];
    cam_z = camera.position[2];

    raio_cam = sqrt(pow((camera.position[0]),2) + pow((camera.position[1]),2) +  pow((camera.position[2]),2));

    float arc = (2 * M_PI) / 360;

    raio_cam += raio_change;

    cam_x = raio_cam * sin(arc * alpha_cam) * cos(arc * beta_cam);
    cam_y = raio_cam * sin(arc * beta_cam);
    cam_z = raio_cam * cos(arc * alpha_cam) * cos(arc * beta_cam);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cam_x, cam_y, cam_z,
              camera.lookAt[0], camera.lookAt[1], camera.lookAt[2],
              camera.up[0], camera.up[1], camera.up[2]);
    
    glBegin(GL_LINES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-500.0f, 0.0f, 0.0f);
        glVertex3f(500.0f, 0.0f, 0.0f);

        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, -500.0f, 0.0f);
        glVertex3f(0.0f, 500.0f, 0.0f);

        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, -500.0f);
        glVertex3f(0.0f, 0.0f, 500.0f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);

    int index = 0;
    drawFigures(mainGroup,index);

    glColor3f(0.5f, 0.8f, 1.0f);
    //glutWireCube(2.0);

    glutSwapBuffers();
}

void processSpecialKeys (int key, int x, int y){

    switch (key)
    {
    case GLUT_KEY_UP:
        beta_cam += 0.5;
        if (beta_cam > 80) beta_cam = 80;
        break;

    case GLUT_KEY_LEFT:
        alpha_cam += 1;
        break;

    case GLUT_KEY_DOWN:
        beta_cam -= 0.5;
        if (beta_cam < -80) beta_cam = -80;
        break;

    case GLUT_KEY_RIGHT:
        alpha_cam -= 1;
        break;
    }

    glutPostRedisplay();
}

void processKeys (unsigned char key, int x, int y){
    switch (key)
    {
    case '+':
        raio_change += 3;
        break;
    case '-':
        raio_change -= 3;
        break;
    }
    glutPostRedisplay();
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

    char filePath[strlen(argv[1]) + strlen(argv[2]) + 11] = "../Fase2/";

    strcat(filePath,argv[1]);
    strcat(filePath,"/");
    strcat(filePath,argv[2]);
    readConfig(filePath);

  
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Engine bombadão");

    glMatrixMode(GL_MODELVIEW);
    glewInit();
    initOpenGL();
    glutDisplayFunc(display);

    glutSpecialFunc(processSpecialKeys);
    glutKeyboardFunc(processKeys);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glutMainLoop();

    return 0;
}
