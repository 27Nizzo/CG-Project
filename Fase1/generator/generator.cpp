#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

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

        float angleStep = 360 / slices;
        float heightStep = height / stacks;
        float ratio = radius / height;

        // Base circle of the cone
        file << "GL_TRIANGLE_FAN" << endl;
        // Vertex at the top
        file << 0.0f << endl;
        file << 0.0f << endl;
        file << 0.0f << endl;

        for (int i = 0; i <= slices; i++) {
            float angle = i * angleStep *  M_PI/180;
            file << radius * -sin(angle) << endl;
            file << 0.0f << endl;
            file << radius * cos(angle) << endl;
        }
        file << "GL_END" << endl;

        // Side of the cone
        for(int i = 0; i < slices; i++){
            file << "GL_TRIANGLE_STRIP" << endl;
            // Vertex at the top
            file << 0.0f << endl;
            file << height << endl;
            file << 0.0f << endl;

            for(int j = 0; j < stacks; j++){

                float angle = i * angleStep *  M_PI/180;
                float nextAngle = (i+1) * angleStep *  M_PI/180;

                float stackHeight = height - heightStep*(j+1);
                float stackRadius = (height - stackHeight) * ratio;

                file << stackRadius * sin(angle) << endl;
                file << stackHeight << endl;
                file << stackRadius * cos(angle) << endl;

                file << stackRadius * sin(nextAngle) << endl;
                file << stackHeight << endl;
                file << stackRadius * cos(nextAngle) << endl;
            }
            file << "GL_END" << endl;

        }

    } else if(primitive.compare("sphere") == 0) {
        file << "sphere" << endl;

        float radius = stof(argv[2]);
        float slices = stof(argv[3]);
        float stacks = stof(argv[4]);

        float angleStepH = 2*M_PI / slices;
        float angleStepV = M_PI / stacks;

        for(int i = 0; i < slices; i++){
            file << "GL_TRIANGLE_STRIP" << endl;

            // Vertex at the top
            file << 0.0f << endl;
            file << radius << endl;
            file << 0.0f << endl;

            for(int j = 0; j < stacks-1; j++){

                float angleV = (j+1) * angleStepV;
                float angleH = i * angleStepH;
                float nextAngleH = (i+1) * angleStepH;

                float y = cos(angleV);

                file << radius * sin(angleH) * sin(angleV) << endl;
                file << radius * y << endl;
                file << radius * cos(angleH) * sin(angleV) << endl;

                file << radius * sin(nextAngleH) * sin(angleV) << endl;
                file << radius * y << endl;
                file << radius * cos(nextAngleH) * sin(angleV) << endl;
            }

            // Vertex at the bottom
            file << 0.0f << endl;
            file << -radius << endl;
            file << 0.0f << endl;

            file << "GL_END" << endl;

        }

    } else {
        printf("Invalid primitive.\nChoose between a sphere, box, plane or cone.\n");
    }
    return 0;
}