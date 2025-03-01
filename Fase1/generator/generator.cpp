#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

using namespace std;

int main(int argc, char** argv){
    if (argc < 5) {
        cerr << "Too few arguments" << endl;
        return 1;
    }
    else if (argc > 7) {
        cerr << "Too many arguments" << endl;
        return 1;
    }
    remove(argv[argc-1]);

    ofstream file(argv[argc-1]);
    string primitive = argv[1];

    if (primitive.compare("plane") == 0){
        file << "plane" << endl;
        float length = stof(argv[2]);
        int divisions = stoi(argv[3]);

        for(int i = 0; i < divisions;i++){
            file << "GL_TRIANGLE_STRIP" << endl;
            for(int j = 0; j <= divisions;j++){
                file << length/2 - j*(length/divisions) << endl;
                file << 0 << endl;
                file << length/2 - i*(length/divisions) << endl;
                file << length/2 - j*(length/divisions) << endl;
                file << 0 << endl;
                file << length/2 - (i+1)*(length/divisions) << endl;
            }
            file << "GL_END" << endl;
            
            file << "GL_TRIANGLE_STRIP" << endl;
            for(int j = 0; j <= divisions;j++){
                file << length/2 - j*(length/divisions) << endl;
                file << 0 << endl;
                file << length/2 - (i+1)*(length/divisions) << endl;
                file << length/2 - j*(length/divisions) << endl;
                file << 0 << endl;
                file << length/2 - i*(length/divisions) << endl;
            }
            file << "GL_END" << endl;
        }
    }

    if (primitive.compare("box") == 0) {
        file << "box" << endl;
        float length = stof(argv[2]);
        int divisions = stoi(argv[3]);
        float half = length / 2.0f;
        float step = length / divisions;

        for (int i = 0; i < divisions; i++) {
            //Bottom face
            file << "GL_TRIANGLE_STRIP" << endl;
            for(int j = 0; j <= divisions; j++){
                file << half - j*step << endl;
                file << -half << endl;
                file << half - (i+1)*step << endl;
                file << half - j*step << endl;
                file << -half << endl;
                file << half - i*step << endl;
            }
            file << "GL_END" << endl;
        }

        for(int i = 0; i < divisions; i++){
            //Top face
            file << "GL_TRIANGLE_STRIP" << endl;
            for(int j = 0; j <= divisions; j++){
                file << half - j*step << endl;
                file << half << endl;
                file << half - i*step << endl;
                file << half - j*step << endl;
                file << half << endl;
                file << half - (i+1)*step << endl;
            }
            file << "GL_END" << endl;
        }

        for(int i = 0; i < divisions; i++){
            //-X face
            file << "GL_TRIANGLE_STRIP" << endl;
            for(int j = 0; j <= divisions; j++){
                file << -half << endl;
                file << half - j*step << endl;
                file << half - i*step << endl;
                file << -half << endl;
                file << half - j*step << endl;
                file << half - (i+1)*step << endl;
            }
            file << "GL_END" << endl;
        }
        
        for(int i = 0; i < divisions; i++){
            //+X face
            file << "GL_TRIANGLE_STRIP" << endl;
            for(int j = 0; j <= divisions; j++){
                file << half << endl;
                file << half - j*step << endl;
                file << half - (i+1)*step << endl;
                file << half << endl;
                file << half - j*step << endl;
                file << half - i*step << endl;
            }
            file << "GL_END" << endl;
        }

        for(int i = 0; i < divisions; i++){
            //-Z face
            file << "GL_TRIANGLE_STRIP" << endl;
            for(int j = 0; j <= divisions; j++){
                file << half - j*step << endl;
                file << half - i*step << endl;
                file << -half << endl;
                file << half - j*step << endl;
                file << half - (i+1)*step << endl;
                file << -half << endl;
            }
            file << "GL_END" << endl;
        }
        for(int i = 0; i < divisions; i++){
            //+Z face
            file << "GL_TRIANGLE_STRIP" << endl;
            for(int j = 0; j <= divisions; j++){
                file << half - j*step << endl;
                file << half - (i+1)*step << endl;
                file << half << endl;
                file << half - j*step << endl;
                file << half - i*step << endl;
                file << half << endl;
            }
            file << "GL_END" << endl;
        }

    } else if(primitive.compare("cone") == 0) {
        file << "cone" << endl;
        float radius = stof(argv[2]);
        float height = stof(argv[3]);
        int slices = stoi(argv[4]);
        int stacks = stoi(argv[5]);

        float hAngleStep = 360 / slices;
        float vAngleStep = 90 / stacks;

        // Base circle of the cone
        file << "GL_TRIANGLE_FAN" << endl;
        // Vertex at the top
        file << 0.0f << endl;
        file << 0.0f << endl;
        file << 0.0f << endl;

        for (int i = 0; i <= slices; i++) {
            float angle = i * hAngleStep *  M_PI/180;
            file << radius * -sin(angle) << endl;
            file << 0.0f << endl;
            file << radius * cos(angle) << endl;
        }
        file << "GL_END" << endl;

        // Side of the cone
        for(int i = 0; i <= slices; i++){
            file << "GL_TRIANGLE_STRIP" << endl;
            // Vertex at the top
            file << 0.0f << endl;
            file << height << endl;
            file << 0.0f << endl;

            for(int j = 0; j < stacks; j++){

                float hAngle = i * hAngleStep *  M_PI/180;
                float vAngle = j * vAngleStep *  M_PI/180;
                file << radiusStep*j * -sin(hAngle) << endl;
                file << 0.0f << endl;
                file << radiusStep*j * cos(hAngle) << endl;

                float nextHAngle = (i+1) * hAngleStep *  M_PI/180;
                file << radiusStep*j * -sin(nextHAngle) << endl;
                file << 0.0f << endl;
                file << radiusStep*j * cos(nextHAngle) << endl;
            }
            file << "GL_END" << endl;

        }

    } else if(primitive.compare("sphere") == 0) {

    } else {
        printf("Poh caralho socio");
    }
    return 0;
}