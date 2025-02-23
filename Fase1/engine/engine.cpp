#include "rapidxml.hpp" 
#include <fstream>
#include <vector>
#include <string>

using namespace rapidxml;
using namespace std;


struct CamConf {
    string resolution;
    int frameRate;
};

CamConf loadCamConf(xml_node<>* cameraNode) {
    CamConf config;
    if(cameraNode) {
        xml_node<>* resolutionNode = cameraNode->first_node("resolution");
        xml_node<>* frameRateNode = cameraNode->first_node("frameRate");

        if(resolutionNode) config.resolution = resolutionNode->value();
        if(frameRateNode) config.frameRate = stoi(frameRateNode->value());
    }
    return config;
}


vector<string> loadFiles(xml_node<>* filesNode) {
    vector<string> files;
    if(filesNode) {
        for (xml_node<>* fileNode = filesNode->first_node("file"); fileNode; 
            fileNode = fileNode->next_sibling("file")) {
                files.push_back(fileNode->value());
    }
    return files;
}


int main() {
    ifstream file("config.xml");
    if(!file.is_open()) {
        ofstream errorFile("output.txt");
        errorFile << "Erro ao abrir ficheiro!" << endl;
        errorFile.close();
        return 1;
    }

    string xmlContent((istreambuf_iterator<char>(file)), istreambuf_iterato<char>());
    file.close();

    xml_document<> doc;
    doc.parse<0>(&xmlContent[0]);

    xml_node<>* root = doc.first_node("configuration");
    if(!root) {
        ofstream errorFile("output.txt");
        errorFile << "Ficheiro XML inválido!" << endl;
        errorFile.close();
        return 1;
    }

    ofstream outputFile("output.txt");

    // Carregar configurações da câmara
    CameraConfig cameraConfig = loadCameraConfig(root->first_node("camera"));
    outputFile << "Resolução: " << cameraConfig.resolution << endl;
    outputFile << "Frame Rate: " << cameraConfig.frameRate << " FPS" << endl;

    // Carregar ficheiros a abrir
    vector<string> filesToLoad = loadFilesToLoad(root->first_node("filesToLoad"));
    outputFile << "Ficheiros a carregar:" << endl;
    for (const auto& file : filesToLoad) {
        outputFile << "- " << file << endl;
    }

    // Fechar ficheiro de saída
    outputFile.close();
    return 0;



}