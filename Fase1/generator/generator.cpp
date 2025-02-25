#include <iostream>
#include <fstream>
#include <vector>

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
            for(int j = 0; j < divisions;j++){
                file << length/2 - j*(length/divisions) << endl;
                file << 0 << endl;
                file << length/2 - i*(length/divisions) << endl;
                file << length/2 - j*(length/divisions) << endl;
                file << 0 << endl;
                file << length/2 - (i+1)*(length/divisions) << endl;
            }
        } 
    } else if(primitive.compare("box") == 0) {
        file << "box" << endl;
        float length = stof(argv[2]);
        int divisons = stoi(argv[3]);

        

    } else if(primitive.compare("cone") == 0) {

    } else if(primitive.compare("sphere") == 0) {

    } else {
        printf("Poh caralho socio");
    }
    return 0;
}