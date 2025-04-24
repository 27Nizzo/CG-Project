#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>

using namespace std;

float bMatrix[4][4] = {
    {-1.0f, 3.0f, -3.0f, 1.0f},
    {3.0f, -6.0f, 3.0f, 0.0f},
    {-3.0f, 3.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f, 0.0f}
};

void multMatrixVector(float* m, float* v, float* result){
    for(int i = 0; i < 4; i++){
        result[i] = 0;
        for(int j = 0; j < 4; j++){
            result[i] += m[i*4+j] * v[j];
        }
    }
}

float multVectorVector(float* v1, float* v2){
    float acc = 0;

    for(int i = 0; i < 4; i++){
        acc += v1[i] * v2[i];
    }
    return acc;
}

void generatePoint(float step, int k, int j, float* pontos_x, float* pontos_y, float* pontos_z, float* ponto){
    float v=step*k;
	float u=step*j;
				
	float u_array[4] = {u * u * u, u * u, u, 1};
	float v_array[4] = {v * v * v, v * v, v, 1}; //Vertical

	float transpostaM_V[4]={0};
	multMatrixVector(*bMatrix, v_array, transpostaM_V);

    float mulX[4];
	float mulY[4];
	float mulZ[4];
	multMatrixVector(pontos_x, transpostaM_V, mulX);
	multMatrixVector(pontos_y, transpostaM_V, mulY);
	multMatrixVector(pontos_z, transpostaM_V, mulZ);

	float M_mulX[4];
	float M_mulY[4];
	float M_mulZ[4];
	multMatrixVector(*bMatrix, mulX, M_mulX);
	multMatrixVector(*bMatrix, mulY, M_mulY);
	multMatrixVector(*bMatrix, mulZ, M_mulZ);

	float result_X=multVectorVector(u_array, M_mulX);
	float result_Y=multVectorVector(u_array, M_mulY);
	float result_Z=multVectorVector(u_array, M_mulZ);

	ponto[0]=result_X;
	ponto[1]=result_Y;
	ponto[2]=result_Z;
}

int main(int argc, char** argv){
    if (argc < 5) {
        cerr << "Too few arguments" << endl;
        return 1;
    }
    else if (argc > 7) {
        cerr << "Too many arguments" << endl;
        return 1;
    }
    
    char filePath[11+strlen(argv[argc-1])] = "";

    strcat(filePath,"../models/");
    strcat(filePath,argv[argc-1]);
    remove(filePath);
    
    ofstream file(filePath);
    string primitive = argv[1];

    if (!file) {
		cout << "Error opening file to write.\n";
		return 1;
	}
    

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
        float slices = stoi(argv[4]);
        float stacks = stoi(argv[5]);

        float angleStep = 360 / slices;
        float heightStep = height / stacks;
        float ratio = radius / height;

        // Base circle of the cone
        file << "GL_TRIANGLE_FAN" << endl;
        // Vertex at the top
        file << 0.0f << endl;
        file << 0.0f << endl;
        file << 0.0f << endl;

        for (int i = 0; i < slices; i++) {
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
    } else if (primitive.compare("torus") == 0) {
        file << "torus" << endl;

        float innerRadius = stof(argv[2]);
        float outerRadius = stof(argv[3]);
        float slices = stof(argv[4]);
        float stacks = stof(argv[5]);

        float sliceStep = 2 * M_PI / slices;
        float stackStep = 2 * M_PI / stacks;

        for (int i = 0; i < stacks; i++) {
            file << "GL_TRIANGLE_STRIP" << endl;

            float stackAngle = i * stackStep;
            float nextStackAngle = (i + 1) * stackStep;

            for (int j = 0; j <= slices; j++) {
                float sliceAngle = j * sliceStep;

                file << (outerRadius + innerRadius * cos(sliceAngle)) * cos(stackAngle) << endl;
                file << innerRadius * sin(sliceAngle) << endl;
                file << (outerRadius + innerRadius * cos(sliceAngle)) * sin(stackAngle) << endl;

                file << (outerRadius + innerRadius * cos(sliceAngle)) * cos(nextStackAngle) << endl;
                file << innerRadius * sin(sliceAngle) << endl;
                file << (outerRadius + innerRadius * cos(sliceAngle)) * sin(nextStackAngle) << endl;
            }
            file << "GL_END" << endl;
        }
    } else if (primitive.compare("bezier") == 0){
        file << "bezier" << endl;

        char bezierFile[18+strlen(argv[2])] = "";
        strcat(bezierFile,"../bezierPatches/");
        strcat(bezierFile,argv[2]);

        int tesselation = stoi(argv[3]);

        ifstream bezier(bezierFile);
        if (!bezier.is_open()) {
            cout << "Error opening bezier file.\n";
            return 1;
        }
        string line;

        getline(bezier,line);
        int numPatches = stoi(line);
        vector<vector<int>> patchIndex;

        for(int i = 0; i < numPatches; i++){
            getline(bezier,line);
            vector<int> indices;
            int index = 0;
            int start = 0;
            int end;
            while ((end = line.find(',', start)) != string::npos) {
                indices.push_back(stoi(line.substr(start, end - start)));
                start = end + 2;
                index += 1;
            }
            indices.push_back(stoi(line.substr(start)));
            patchIndex.push_back(indices);
        }

        getline(bezier,line);
        int numControlPoints = stoi(line);
        vector<vector<float>> controlPoints;

        for(int i = 0; i < numControlPoints; i++){
            getline(bezier,line);
            vector<float> points;
            int index = 0;
            int start = 0;
            int end;
            while ((end = line.find(',', start)) != string::npos) {
                points.push_back(stof(line.substr(start, end - start)));
                start = end + 2;
                index += 1;
            }
            points.push_back(stof(line.substr(start)));
            controlPoints.push_back(points);
        }
        bezier.close();

        float patchPoints[16][3];
        for(int i = 0; i < numPatches; i++){
            float tesselationStep = 1.0f / tesselation;
            float pontos_x[4][4] = {
                {controlPoints[patchIndex[i][0]][0],controlPoints[patchIndex[i][1]][0],controlPoints[patchIndex[i][2]][0],controlPoints[patchIndex[i][3]][0]},
                {controlPoints[patchIndex[i][4]][0],controlPoints[patchIndex[i][5]][0],controlPoints[patchIndex[i][6]][0],controlPoints[patchIndex[i][7]][0]},
                {controlPoints[patchIndex[i][8]][0],controlPoints[patchIndex[i][9]][0],controlPoints[patchIndex[i][10]][0],controlPoints[patchIndex[i][11]][0]},
                {controlPoints[patchIndex[i][12]][0],controlPoints[patchIndex[i][13]][0],controlPoints[patchIndex[i][14]][0],controlPoints[patchIndex[i][15]][0]}
            };

            float pontos_y[4][4] = {
                {controlPoints[patchIndex[i][0]][1],controlPoints[patchIndex[i][1]][1],controlPoints[patchIndex[i][2]][1],controlPoints[patchIndex[i][3]][1]},
                {controlPoints[patchIndex[i][4]][1],controlPoints[patchIndex[i][5]][1],controlPoints[patchIndex[i][6]][1],controlPoints[patchIndex[i][7]][1]},
                {controlPoints[patchIndex[i][8]][1],controlPoints[patchIndex[i][9]][1],controlPoints[patchIndex[i][11]][1],controlPoints[patchIndex[i][11]][1]},
                {controlPoints[patchIndex[i][12]][1],controlPoints[patchIndex[i][13]][1],controlPoints[patchIndex[i][14]][1],controlPoints[patchIndex[i][15]][1]}
            };

            float pontos_z[4][4] = {
                {controlPoints[patchIndex[i][0]][2],controlPoints[patchIndex[i][1]][2],controlPoints[patchIndex[i][2]][2],controlPoints[patchIndex[i][3]][2]},
                {controlPoints[patchIndex[i][4]][2],controlPoints[patchIndex[i][5]][2],controlPoints[patchIndex[i][6]][2],controlPoints[patchIndex[i][7]][2]},
                {controlPoints[patchIndex[i][8]][2],controlPoints[patchIndex[i][9]][2],controlPoints[patchIndex[i][12]][2],controlPoints[patchIndex[i][11]][2]},
                {controlPoints[patchIndex[i][12]][2],controlPoints[patchIndex[i][13]][2],controlPoints[patchIndex[i][14]][2],controlPoints[patchIndex[i][15]][2]}
            };

            for(int j = 0; j < tesselation; j++){
                file << "GL_TRIANGLE_STRIP" << endl;
                for(int k = 0; k <= tesselation; k++){
                    float ponto[3];
                    generatePoint(tesselationStep,j,k,*pontos_x, *pontos_y, *pontos_z, ponto);
                    file << ponto[0] << endl;
                    file << ponto[1] << endl;
                    file << ponto[2] << endl;
                    generatePoint(tesselationStep,j+1,k,*pontos_x, *pontos_y, *pontos_z, ponto);
                    file << ponto[0] << endl;
                    file << ponto[1] << endl;
                    file << ponto[2] << endl;
                }
                file << "GL_END" << endl;
            }


        }
    }
    else {
        printf("Invalid primitive.\nChoose between a sphere, box, plane, cone or torus.\n");
    }
    file.close();
    return 0;
}