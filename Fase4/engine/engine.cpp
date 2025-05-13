#include "pugixml.hpp"
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <cstring>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include "../aux/matrix.hpp"

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

enum LightType {
    POINT,
    DIRECTIONAL,
    SPOTLIGHT
};

struct Transformation {
    Type type;
    float x, y, z, angle = 0, time;
    bool align;
    list<Point> points;
};

struct Color {
    float diffuseR = 200, diffuseG = 200, diffuseB = 200;
    float ambientR = 50, ambientG = 50, ambientB = 50;
    float specularR = 0, specularG = 0, specularB = 0;
    float emissiveR = 0, emissiveG = 0, emissiveB = 0;
    float shininess = 0;
};

struct File{
    string file, texture;
    float dayPeriod = 0;
    Model modelo;
    Color color;
};

struct Light {
    LightType type;
    float pos[3];
    float dir[3];
    float cutoff;
};

struct Group {
    list<File> files;
    list<Transformation> transformations;
    list<Group> children;
    int yearPeriod = 0;
};

list<Light> lights;

GLenum mode = GL_LINE;
Camera camera;

Group mainGroup;
int buffer = 0;
int maxBuffers = 100;
GLuint buffers[100];
float prev_y[3] = {0, 1, 0};
float cam_x = 1, cam_y = 1, cam_z = 1;
float raio_cam = 0, raio_change = 0, raio_circ = 0;
float alpha_cam = 0, beta_cam = 0;
int check = 0;

int windowWidth = 800, windowHeight = 600;

Model processVBO(File file){
    int counter = 0;

    // create path for file
    char filePath[11+strlen(file.file.c_str())] = "";
    strcat(filePath,"../models/");
    strcat(filePath,file.file.c_str());
    ifstream f(filePath);

    string line;
    Model modelo;

    if (f.is_open()) {
        vector<float> p;
        // Read each line from the file and store it in the
        // 'line' variable.
        while (getline(f, line)) {
            if (line == "cone" || line == "box" || line == "plane" || line == "sphere" || line == "torus" || line == "bezier") {
                modelo.type = line;
            }
            else if (line[0] == 'G') {
                modelo.faces.push_back(line);
                modelo.dividers.push_back(counter/3);
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
        modelo.vbo = buffer;
        glBindBuffer(GL_ARRAY_BUFFER, buffers[buffer++]);
        glBufferData(GL_ARRAY_BUFFER, p.size() * sizeof(float), p.data(), GL_STATIC_DRAW);
        modelo.numVertices = counter;
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
    return modelo;
}

Group processGroup(pugi::xml_node groupNode){
    Group group;

    for(pugi::xml_node transformNode = groupNode.child("transform"); transformNode; transformNode = transformNode.next_sibling("transform")){
        Transformation transformation;

        for(pugi::xml_node childNode = transformNode.first_child(); childNode; childNode = childNode.next_sibling()){
            if (strcmp(childNode.name(),"translate") == 0){
                transformation.type = TRANSLATE;
                transformation.time = childNode.attribute("time").as_float();
                transformation.align = childNode.attribute("align").as_bool();
                transformation.x = childNode.attribute("x").as_float();
                transformation.y = childNode.attribute("y").as_float();
                transformation.z = childNode.attribute("z").as_float();

                Point point;
                for(pugi::xml_node pointNode = childNode.first_child(); pointNode; pointNode = pointNode.next_sibling()){
                    point.x = pointNode.attribute("x").as_float();
                    point.y = pointNode.attribute("y").as_float();
                    point.z = pointNode.attribute("z").as_float();
                    transformation.points.push_back(point);
                }

                group.transformations.push_back(transformation);
            }
            else if (strcmp(childNode.name(),"rotate") == 0){
                transformation.type = ROTATE;
                transformation.x = childNode.attribute("x").as_float();
                transformation.y = childNode.attribute("y").as_float();
                transformation.z = childNode.attribute("z").as_float();
                transformation.angle = childNode.attribute("angle").as_float();
                transformation.time = childNode.attribute("time").as_float();
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
        File file;
        file.file = modelNode.attribute("file").as_string();
        file.dayPeriod = modelNode.attribute("dayPeriod").as_float();
        file.texture = modelNode.child("texture").attribute("file").as_string();

        pugi::xml_node colorNode = modelNode.child("color");
        Color color;

        for(pugi::xml_node componentNode = colorNode.first_child(); componentNode; componentNode = componentNode.next_sibling()){
            if(strcmp(componentNode.name(),"diffuse") == 0){
                color.diffuseR = componentNode.attribute("R").as_float();
                color.diffuseG = componentNode.attribute("G").as_float();
                color.diffuseB = componentNode.attribute("B").as_float();
            }
            else if(strcmp(componentNode.name(),"ambient") == 0){
                color.ambientR = componentNode.attribute("R").as_float();
                color.ambientG = componentNode.attribute("G").as_float();
                color.ambientB = componentNode.attribute("B").as_float();
            }
            else if(strcmp(componentNode.name(),"specular") == 0){
                color.specularR = componentNode.attribute("R").as_float();
                color.specularG = componentNode.attribute("G").as_float();
                color.specularB = componentNode.attribute("B").as_float();
            }
            else if(strcmp(componentNode.name(),"emissive") == 0){
                color.emissiveR = componentNode.attribute("R").as_float();
                color.emissiveG = componentNode.attribute("G").as_float();
                color.emissiveB = componentNode.attribute("B").as_float();
            }
            else if(strcmp(componentNode.name(),"shininess") == 0){
                color.shininess = componentNode.attribute("value").as_float();
            }
        }

        file.color = color;

        Model m = processVBO(file);
        file.modelo = m;
        group.files.push_back(file);
    }

    pugi::xml_node groupsNode = groupNode.child("group");
    for(pugi::xml_node childGroupNode = groupsNode; childGroupNode; childGroupNode = childGroupNode.next_sibling()){
        Group childGroup = processGroup(childGroupNode);
        group.children.push_back(childGroup);
    }

    group.yearPeriod = groupNode.attribute("yearPeriod").as_float();

    return group;
}

void readConfig(const char* filePath) {
    pugi::xml_document doc;
    if (!doc.load_file(filePath)) {
        cerr << "Failed to load XML file. " << filePath << endl;
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

    pugi::xml_node lightsNode = root.child("lights");

    for(pugi::xml_node lightNode = lightsNode.child("light"); lightNode; lightNode = lightNode.next_sibling()){
        Light light;

        if (strcmp(lightNode.attribute("type").as_string(),"point") == 0){
            light.type = POINT;
            light.pos[0] = lightNode.attribute("posX").as_float();
            light.pos[1] = lightNode.attribute("posY").as_float();
            light.pos[2] = lightNode.attribute("posZ").as_float();
        }
        else if (strcmp(lightNode.attribute("type").as_string(),"directional") == 0){
            light.type = DIRECTIONAL;
            light.dir[0] = lightNode.attribute("dirX").as_float();
            light.dir[1] = lightNode.attribute("dirY").as_float();
            light.dir[2] = lightNode.attribute("dirZ").as_float();
        }
        else if (strcmp(lightNode.attribute("type").as_string(),"spotlight") == 0){
            light.type = SPOTLIGHT;
            light.pos[0] = lightNode.attribute("posX").as_float();
            light.pos[1] = lightNode.attribute("posY").as_float();
            light.pos[2] = lightNode.attribute("posZ").as_float();
            light.dir[0] = lightNode.attribute("dirX").as_float();
            light.dir[1] = lightNode.attribute("dirY").as_float();
            light.dir[2] = lightNode.attribute("dirZ").as_float();
            light.cutoff = lightNode.attribute("cutoff").as_float();
        }
        lights.push_back(light);
    }

    // Model files
    pugi::xml_node groupNode = root.child("group");
    mainGroup = processGroup(groupNode);
}

void renderCatmullRomCurve(std::list<Point> points) {
    float pos[3];
    float derivada[3];
    glBegin(GL_LINE_LOOP);
    float gt;
    for (gt = 0; gt < 1; gt += 1.0 / 100) {
        getGlobalCatmullRomPoint(gt, points, pos, derivada);
        glVertex3f(pos[0], pos[1], pos[2]);
    }
    glEnd();
}

void setupColor(const Color& color, int texture){
    float diffuse[4] = {color.diffuseR/255.0f,color.diffuseG/255.0f,color.diffuseB/255.0f,1.0f};
    float emissive[4] = {color.emissiveR/255.0f,color.emissiveG/255.0f,color.emissiveB/255.0f,1.0f};
    float specular[4] = {color.specularR/255.0f,color.specularG/255.0f,color.specularB/255.0f,1.0f};
    float ambient[4] = {0,0,0,0};

    if(texture == 0){
        ambient[0] = color.ambientR/255.0f;
        ambient[1] = color.ambientG/255.0f;
        ambient[2] = color.ambientB/255.0f;
        ambient[3] = 1.0f;
        glMaterialfv(GL_FRONT,GL_AMBIENT,ambient);
    }

    glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse);
    glMaterialfv(GL_FRONT,GL_SPECULAR,specular);
    glMaterialfv(GL_FRONT,GL_EMISSION,emissive);
    glMaterialfv(GL_FRONT,GL_SHININESS,&color.shininess);

    int index = 0;
    for(const Light& light : lights){
        GLenum glLight = GL_LIGHT0 + index;

        glEnable(glLight);
        glLightfv(glLight, GL_DIFFUSE, diffuse);
        glLightfv(glLight, GL_SPECULAR, specular);
        if(texture == 0) glLightfv(glLight, GL_AMBIENT, ambient);

        index++;
    }
}

void drawFigures(Group group){
    int elapsedTime = glutGet(GLUT_ELAPSED_TIME);

    for (Transformation transform : group.transformations){
        if (transform.type == TRANSLATE){
            if (transform.time == 0){
                glTranslatef(transform.x, transform.y, transform.z);
            } else {
                float pos[3], deriv[3];
                float gt = ( glutGet(GLUT_ELAPSED_TIME) / 1000.0) / transform.time;
        
                getGlobalCatmullRomPoint(gt,transform.points,pos,deriv);
                renderCatmullRomCurve(transform.points);

                glTranslatef(pos[0], pos[1], pos[2]);

                if(transform.align){
                    normalize(deriv);

                    float z[3];
                    product(deriv, prev_y, z);
                    normalize(z);

                    float y[3];
                    product(z, deriv, y);
                    normalize(y);

                    prev_y[0] = y[0];
                    prev_y[1] = y[1];
                    prev_y[2] = y[2];

                    float m[16];
                    buildRotationMatrix(deriv, y, z, m);

                    glMultMatrixf(m);
                }
            }
        }
        else if (transform.type == ROTATE){
            if (transform.time==0){
                glRotatef(transform.angle,transform.x,transform.y,transform.z);
            }else{
                double angle = 360.0 * (glutGet(GLUT_ELAPSED_TIME) / 1000.0) / transform.time;
                //para que nunca ultrapasse os 360º
                while (angle > 360) angle -= 360;
                glRotatef(angle, transform.x, transform.y, transform.z);
            }
        }
        else if (transform.type == SCALE){
            glScalef(transform.x, transform.y, transform.z);
        }
    }

    for (File file : group.files){
        glPushMatrix();

        Model modelo = file.modelo;

        if (file.dayPeriod > 0){
            float elapsedRTime = glutGet(GLUT_ELAPSED_TIME);
            if (file.dayPeriod > 0) {
                float angle = (360.0f * elapsedRTime)/(file.dayPeriod*1000.0f);
                while (angle > 360) angle -= 360;

                glRotatef(angle, 0, 1, 0);
            }
        }
        int texture = 0;

        if (file.texture != ""){
            texture = 1;
        }

        setupColor(file.color, texture);

        glBindBuffer(GL_ARRAY_BUFFER, buffers[modelo.vbo]);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        for(int i = 0; i < modelo.dividers.size(); i+=2){
            if (modelo.faces[i] == "GL_TRIANGLE_STRIP") {
                glDrawArrays(GL_TRIANGLE_STRIP, modelo.dividers[i], modelo.dividers[i+1] - modelo.dividers[i]);
            } else if (modelo.faces[i] == "GL_TRIANGLE_FAN") {
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
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glGenBuffers(maxBuffers, buffers);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, mode);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(camera.fov, (float)windowWidth / windowHeight, camera.nearPlane, camera.farPlane);

    raio_cam = sqrt(pow((camera.position[0]),2) + pow((camera.position[1]),2) +  pow((camera.position[2]),2));
    raio_circ = sqrt(pow((camera.position[0]),2) + pow((camera.position[2]),2));
    float arc = (2 * M_PI) / 360;

    if (check == 0){
        alpha_cam = asin(camera.position[0]/raio_circ) / arc;
        beta_cam = asin(camera.position[1]/raio_cam) / arc;
        check = 1;
    }


    raio_cam += raio_change;

    cam_x = raio_cam * sin(arc * alpha_cam) * cos(arc * beta_cam);
    cam_y = raio_cam * sin(arc * beta_cam);
    cam_z = raio_cam * cos(arc * alpha_cam) * cos(arc * beta_cam);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cam_x, cam_y, cam_z,
              camera.lookAt[0], camera.lookAt[1], camera.lookAt[2],
              camera.up[0], camera.up[1], camera.up[2]);

    glEnable(GL_LIGHTING);
    int lightIndex = 0;
    for(const Light& light : lights){
        float dir[4] = {light.dir[0],light.dir[1],light.dir[2], 0.0f};
        float pos[4] = {light.pos[0],light.pos[1],light.pos[2], 1.0f};

        GLenum glLight = GL_LIGHT0 + lightIndex;
        if (light.type == DIRECTIONAL){
            glLightfv(glLight, GL_POSITION, dir);
        }
        else if (light.type == POINT){
            glLightfv(glLight, GL_POSITION, pos);
        }
        else{
            glLightfv(glLight, GL_POSITION, pos);
            glLightfv(glLight, GL_SPOT_DIRECTION, light.dir);
            glLightfv(glLight, GL_SPOT_CUTOFF, &light.cutoff);
        }
        lightIndex++;
    }
    
    glDisable(GL_LIGHTING);
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
    glEnable(GL_LIGHTING);

    glColor3f(1.0f, 1.0f, 1.0f);

    glEnableClientState(GL_VERTEX_ARRAY);
    drawFigures(mainGroup);
    glDisableClientState(GL_VERTEX_ARRAY);

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

void processKeys(unsigned char key, int xx, int yy){
    switch (key) {
        case '+':
            raio_change -= 3;
            break;
        case '-':
            raio_change += 3;
            break;
        case 'f':
            mode = GL_FILL;
            break;

        case 'l':
            mode = GL_LINE;
            break;

        case 'p':
            mode = GL_POINT;
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
    glutReshapeWindow(windowWidth,windowHeight);

    glutDisplayFunc(display);
    glutIdleFunc(display);

    glutSpecialFunc(processSpecialKeys);
    glutKeyboardFunc(processKeys);

    glEnable(GL_CULL_FACE);

    glutMainLoop();

    return 0;
}
