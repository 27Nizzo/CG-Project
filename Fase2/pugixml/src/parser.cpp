#include "parser.h"
#include "pugixml.hpp"
#include <iostream>
#include "orbits.h"

std::vector<Planet> LoadPlanets(const std::string& filename) {
    std::vector<Planet> planetas;
    pugi::xml_document doc;
    
    if (!doc.load_file(filename.c_str())) {
        std::cerr << "Erro ao carregar XML: " << filename << std::endl;
        return planetas;
    }

    for (pugi::xml_node planeta : doc.child("world").child("planets").children("planet")) {
        std::string name = planeta.attribute("name").as_string();
        float radius = planeta.attribute("radius").as_float();
        float distance = planeta.attribute("distance").as_float();
        int index = planeta.attribute("index").as_int();
        std::string model_file = planeta.attribute("model").as_string();

        planetas.emplace_back(name, radius, distance, velocidade_orbita.at(index), model_file);
    }

    return planetas;
}
