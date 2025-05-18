#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include "../aux/matrix.hpp"

using namespace std;

float bMatrix[4][4] = {
    {-1.0f, 3.0f, -3.0f, 1.0f},
    {3.0f, -6.0f, 3.0f, 0.0f},
    {-3.0f, 3.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f, 0.0f}
};

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

void generateNormal(float step, int k, int j, float* pontos_x, float* pontos_y, float* pontos_z, float* normal){
    float v=step*k;
	float u=step*j;
				
	float u_array[4] = {u * u * u, u * u, u, 1};
    float ud_array[4] = {3 * u * u, 2 * u, 1, 0};
	float v_array[4] = {v * v * v, v * v, v, 1}; //Vertical
    float vd_array[4] = {3 * v * v, 2 * v, 1, 0}; //Vertical

	float transpostaM_V[4]={0};
	multMatrixVector(*bMatrix, v_array, transpostaM_V);

    float transpostaM_VD[4]={0};
	multMatrixVector(*bMatrix, vd_array, transpostaM_VD);

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

	float result_X=multVectorVector(ud_array, M_mulX);
	float result_Y=multVectorVector(ud_array, M_mulY);
	float result_Z=multVectorVector(ud_array, M_mulZ);

    float result_u[3] = {result_X, result_Y, result_Z};

	multMatrixVector(pontos_x, transpostaM_VD, mulX);
	multMatrixVector(pontos_y, transpostaM_VD, mulY);
	multMatrixVector(pontos_z, transpostaM_VD, mulZ);

	multMatrixVector(*bMatrix, mulX, M_mulX);
	multMatrixVector(*bMatrix, mulY, M_mulY);
	multMatrixVector(*bMatrix, mulZ, M_mulZ);

	result_X=multVectorVector(u_array, M_mulX);
	result_Y=multVectorVector(u_array, M_mulY);
	result_Z=multVectorVector(u_array, M_mulZ);

	float result_v[3] = {result_X, result_Y, result_Z};

    normalize(result_u);
    normalize(result_v);
    product(result_v,result_u,normal);
    normalize(normal);
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

    strcat(filePath,"../Fase4-models/");
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
        float textureStep = (float) 1.0f/divisions;

        for(int i = 0; i < divisions;i++){
            file << "GL_TRIANGLE_STRIP" << endl;

            float t0 = i * textureStep, t1 = (i+1)*textureStep;

            for(int j = 0; j <= divisions;j++){
                // file format - Point x;Normal x;Texture x;\n Point y;Normal y;Texture y;\nPoint z;Normal z;Texture z; 
                // being a plane, the normal is always the same - (0,1,0)

                float s = 1 - (j * textureStep);

                file << length/2 - j*(length/divisions) << ";" << 0 << ";" << s << endl;
                file << 0 << ";" << 1 << ";" << t0 << endl;
                file << length/2 - i*(length/divisions) << ";" << 0 << endl;

                //Next point
                file << length/2 - j*(length/divisions) << ";" << 0 << ";" << s << endl;
                file << 0 << ";" << 1 << ";" << t1 << endl;
                file << length/2 - (i+1)*(length/divisions) << ";" << 0 << endl;
            }
            file << "GL_END" << endl;
        }
    }
    else if (primitive.compare("box") == 0) {
        file << "box" << endl;
        float length = stof(argv[2]);
        int divisions = stoi(argv[3]);
        float half = length / 2.0f;
        float step = length / divisions;
        float textureStep = (float) 1.0f/divisions;

        for (int i = 0; i < divisions; i++) {
            //Bottom face - Normal equals (0,-1,0)
            float t0 = 1 - (i * textureStep), t1 = 1 - ((i+1)*textureStep);

            file << "GL_TRIANGLE_STRIP" << endl;
            for(int j = 0; j <= divisions; j++){
                float s = j*textureStep;

                file << half - j*step << ";" << 0 << ";" << s << endl;
                file << -half << ";" << -1 << ";" << t1 << endl;
                file << half - (i+1)*step << ";" << 0 << endl;

                //Next point
                file << half - j*step << ";" << 0 << ";" << s << endl;
                file << -half << ";" << -1 << ";" << t0 << endl;
                file << half - i*step << ";" << 0 << endl;
            }
            file << "GL_END" << endl;
        }

        for(int i = 0; i < divisions; i++){
            //Top face - Normal equals (0,1,0)
            float t0 = i * textureStep, t1 = (i+1)*textureStep;

            file << "GL_TRIANGLE_STRIP" << endl;
            for(int j = 0; j <= divisions; j++){
                float s = 1 - (j*textureStep);

                file << half - j*step << ";" << 0 << ";" << s << endl;
                file << half << ";" << 1 << ";" << t0 << endl;
                file << half - i*step << ";" << 0 << endl;

                //Next point
                file << half - j*step << ";" << 0 << ";" << s << endl;
                file << half << ";" << 1 << ";" << t1 << endl;
                file << half - (i+1)*step << ";" << 0 << endl;
            }
            file << "GL_END" << endl;
        }

        for(int i = 0; i < divisions; i++){
            //-X face - Normal equals (-1,0,0)
            float t0 = 1 - (i * textureStep), t1 = 1 - ((i+1)*textureStep);
            
            file << "GL_TRIANGLE_STRIP" << endl;
            for(int j = 0; j <= divisions; j++){
                float s = 1 - (j*textureStep);

                file << -half << ";" << -1 << ";" << t0 << endl;
                file << half - j*step << ";" << 0 << ";" << s << endl;
                file << half - i*step << ";" << 0 << endl;

                //Next point
                file << -half << ";" << -1 << ";" << t1 << endl;
                file << half - j*step << ";" << 0 << ";" << s << endl;
                file << half - (i+1)*step << ";" << 0 << endl;
            }
            file << "GL_END" << endl;
        }

        for(int i = 0; i < divisions; i++){
            //+X face - Normal equals (1,0,0)
            float t0 = 1 - (i * textureStep), t1 = 1 - ((i+1)*textureStep);
            
            file << "GL_TRIANGLE_STRIP" << endl;
            for(int j = 0; j <= divisions; j++){
                float s = j*textureStep;

                file << half << ";" << 1 << ";" << t1 << endl;
                file << half - j*step << ";" << 0 << ";" << s << endl;
                file << half - (i+1)*step << ";" << 0 << endl;

                //Next point
                file << half << ";" << 1 << ";" << t0 << endl;
                file << half - j*step << ";" << 0 << ";" << s << endl;
                file << half - i*step << ";" << 0 << endl;
            }
            file << "GL_END" << endl;
        }

        for(int i = 0; i < divisions; i++){
            //-Z face - Normal equals (0,0,-1)
            float t0 = 1 - (i * textureStep), t1 = 1 - ((i+1)*textureStep);

            file << "GL_TRIANGLE_STRIP" << endl;
            for(int j = 0; j <= divisions; j++){
                float s = j*textureStep;

                file << half - j*step << ";" << 0 << ";" << s << endl;
                file << half - i*step << ";" << 0 << ";" << t0 << endl;
                file << -half << ";" << -1 << endl;

                //Next point
                file << half - j*step << ";" << 0 << ";" << s << endl;
                file << half - (i+1)*step << ";" << 0 << ";" << t1 << endl;
                file << -half << ";" << -1 << endl;
            }
            file << "GL_END" << endl;
        }
        for(int i = 0; i < divisions; i++){
            //+Z face - Normal equals (0,0,1)
            float t0 = 1 - (i * textureStep), t1 = 1 - ((i+1)*textureStep);

            file << "GL_TRIANGLE_STRIP" << endl;
            for(int j = 0; j <= divisions; j++){
                float s = 1 - j*textureStep;
                file << half - j*step << ";" << 0 << ";" << s << endl;
                file << half - (i+1)*step << ";" << 0 << ";" << t1 << endl;
                file << half << ";" << 1 << endl;

                //Next point
                file << half - j*step << ";" << 0 << ";" << s << endl;
                file << half - i*step << ";" << 0 << ";" << t0 << endl;
                file << half << ";" << 1 << endl;
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
        float sideLength = sqrt((radius*radius) + (height*height));

        float textureStackStep = (float) 1.0f / stacks;
        float textureSliceStep = (float) 1.0f / slices;

        // Base circle of the cone - Normal equals (0,-1-0)
        file << "GL_TRIANGLE_FAN" << endl;
        // Vertex on the middle

        file << 0.0f << ";" << 0 << ";" << 0 << endl;
        file << 0.0f << ";" << -1 << ";" << 1 << endl;
        file << 0.0f << ";" << 0 << endl;

        for (int i = 0; i <= slices; i++) {
            float angle = i * angleStep *  M_PI/180;

            file << radius * -sin(angle) << ";" << 0 << ";" << 0 << endl;
            file << 0.0f << ";" << -1 << ";" << 0 << endl;
            file << radius * cos(angle) << ";" << 0 << endl;
        }
        file << "GL_END" << endl;

        // Side of the cone
        for(int i = 0; i < slices; i++){
            file << "GL_TRIANGLE_STRIP" << endl;
            // Vertex at the top
            file << 0.0f << ";" << 0 << ";" << i*textureSliceStep << endl;
            file << height << ";" << 1 << ";" << 1 << endl;
            file << 0.0f << ";" << 0 << endl;

            float angle = i * angleStep *  M_PI/180;
            float nextAngle = (i+1) * angleStep *  M_PI/180;

            for(int j = 0; j < stacks; j++){

                float stackHeight = height - heightStep*(j+1);
                float stackRadius = (height - stackHeight) * ratio;

                float t = 1.0f - (j+1)*textureStackStep;
                float s1 = i * textureSliceStep;
                float s0 = (i+1) * textureSliceStep;

                float y = (float) sin(M_PI - atan(height/radius));

                float a[3] = {sin(angle),y,cos(angle)};
                float b[3] = {sin(nextAngle),y,cos(nextAngle)};

                normalize(a);
                normalize(b);

                file << stackRadius * sin(angle) << ";" << a[0] << ";" << s0 << endl;
                file << stackHeight << ";" << a[1] << ";" << t << endl;
                file << stackRadius * cos(angle) << ";" << a[2] << endl;

                file << stackRadius * sin(nextAngle) << ";" << b[0] << ";" << s1 << endl;
                file << stackHeight << ";" << b[1] << ";" << t << endl;
                file << stackRadius * cos(nextAngle) << ";" << b[2] << endl;
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

        float textureSliceStep = (float) 1.0f / slices;
        float textureStackStep = (float) 1.0f / stacks;

        for(int i = 0; i < slices; i++){
            file << "GL_TRIANGLE_STRIP" << endl;

            // Vertex at the top
            file << 0.0f << ";" << 0 << ";" << i*textureSliceStep + 0.5*textureSliceStep << endl;
            file << radius << ";" << 1 << ";" << 1 << endl;
            file << 0.0f << ";" << 0 << endl;

            for(int j = 1; j < stacks; j++){

                float angleV = j * angleStepV;
                float angleH = i * angleStepH;
                float nextAngleH = (i+1) * angleStepH;

                float x = radius * sin(angleH) * sin(angleV), nX = radius * sin(nextAngleH) * sin(angleV);
                float y = radius * cos(angleV);
                float z = radius * cos(angleH) * sin(angleV), nZ = radius * cos(nextAngleH) * sin(angleV);

                float a[3] = {x,y,z};
                float b[3] = {nX,y,nZ};

                normalize(a);
                normalize(b);

                file << x << ";" << a[0] << ";" << i*textureSliceStep << endl;
                file << y << ";" << a[1] << ";" << 1.0f - j*textureStackStep << endl;
                file << z << ";" << a[2] << endl;

                file << nX << ";" << b[0] << ";" << (i+1)*textureSliceStep << endl;
                file << y << ";" << b[1] << ";" << 1.0f - j*textureStackStep << endl;
                file << nZ << ";" << b[2] << endl;
            }

            // Vertex at the bottom
            file << 0.0f << ";" << 0 << ";" << i*textureSliceStep + 0.5*textureSliceStep << endl;
            file << -radius << ";" << -1 << ";" << 0 << endl;
            file << 0.0f << ";" << 0 << endl;

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

        float textureStackStep = (float) 1.0f / stacks;
        float textureSliceStep = (float) 2.0f / slices;

        for (int i = 0; i < stacks; i++) {
            file << "GL_TRIANGLE_STRIP" << endl;

            float stackAngle = i * stackStep;
            float nextStackAngle = (i + 1) * stackStep;

            for (int j = 0; j <= slices; j++) {
                float sliceAngle = j * sliceStep;

                float x = (outerRadius + innerRadius * cos(sliceAngle)) * cos(stackAngle), z = (outerRadius + innerRadius * cos(sliceAngle)) * sin(stackAngle);
                float y = -(innerRadius * sin(sliceAngle));
                float nX = (outerRadius + innerRadius * cos(sliceAngle)) * cos(nextStackAngle), nZ = (outerRadius + innerRadius * cos(sliceAngle)) * sin(nextStackAngle);

                float a[3] = {x,y,z}, b[3] = {nX,y,nZ};
                normalize(a);
                normalize(b);

                float t = j * textureSliceStep;

                if (t > 1) t = 2 - t;

                file << x << ";" << a[0] << ";" << t << endl;
                file << y << ";" << a[1] << ";" << 0 << endl;
                file << z << ";" << a[2] << endl;

                file << nX << ";" << b[0] << ";" << t << endl;
                file << y << ";" << b[1] << ";" << 1 << endl;
                file << nZ << ";" << b[2] << endl;
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
            int start = 0;
            int end;
            while ((end = line.find(',', start)) != string::npos) {
                indices.push_back(stoi(line.substr(start, end - start)));
                start = end + 2;
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
            int start = 0;
            int end;
            while ((end = line.find(',', start)) != string::npos) {
                points.push_back(stof(line.substr(start, end - start)));
                start = end + 2;
            }
            points.push_back(stof(line.substr(start)));
            controlPoints.push_back(points);
        }
        bezier.close();

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
                {controlPoints[patchIndex[i][8]][1],controlPoints[patchIndex[i][9]][1],controlPoints[patchIndex[i][10]][1],controlPoints[patchIndex[i][11]][1]},
                {controlPoints[patchIndex[i][12]][1],controlPoints[patchIndex[i][13]][1],controlPoints[patchIndex[i][14]][1],controlPoints[patchIndex[i][15]][1]}
            };

            float pontos_z[4][4] = {
                {controlPoints[patchIndex[i][0]][2],controlPoints[patchIndex[i][1]][2],controlPoints[patchIndex[i][2]][2],controlPoints[patchIndex[i][3]][2]},
                {controlPoints[patchIndex[i][4]][2],controlPoints[patchIndex[i][5]][2],controlPoints[patchIndex[i][6]][2],controlPoints[patchIndex[i][7]][2]},
                {controlPoints[patchIndex[i][8]][2],controlPoints[patchIndex[i][9]][2],controlPoints[patchIndex[i][10]][2],controlPoints[patchIndex[i][11]][2]},
                {controlPoints[patchIndex[i][12]][2],controlPoints[patchIndex[i][13]][2],controlPoints[patchIndex[i][14]][2],controlPoints[patchIndex[i][15]][2]}
            };

            for(int j = 0; j < tesselation; j++){
                file << "GL_TRIANGLE_STRIP" << endl;
                for(int k = 0; k <= tesselation; k++){
                    float point[3], normal[3];
                    generatePoint(tesselationStep,j,k,*pontos_x, *pontos_y, *pontos_z, point);
                    generateNormal(tesselationStep,j,k,*pontos_x, *pontos_y, *pontos_z, normal);
                    file << point[0] << ';' << normal[0] << ';' << tesselationStep*j << endl;
                    file << point[1] << ';' << normal[1] << ';' << tesselationStep*k << endl;
                    file << point[2] << ';' << normal[2] << endl;

                    float nextPoint[3], nextNormal[3];
                    generatePoint(tesselationStep,j+1,k,*pontos_x, *pontos_y, *pontos_z, nextPoint);
                    generateNormal(tesselationStep,j+1,k,*pontos_x, *pontos_y, *pontos_z, nextNormal);
                    file << nextPoint[0] << ';' << nextNormal[0] << ';' << tesselationStep*(j+1) << endl;
                    file << nextPoint[1] << ';' << nextNormal[1] << ';' << tesselationStep*k << endl;
                    file << nextPoint[2] << ';' << nextNormal[2] << endl;
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