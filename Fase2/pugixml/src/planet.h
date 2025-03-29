#ifndef PLANET_H
#define PLANET_H

#include <string>

struct Planet {
    std::string name;
    float radius;
    float distance;
    float orbit_speed;
    std::string model_file;

    Planet(std::string n, float r, float d, float s, std::string file)
        : name(n), radius(r), distance(d), orbit_speed(s), model_file(file) {}
};

#endif 

// Define a estrutura dos planetas, para guardar a info dos mesmos