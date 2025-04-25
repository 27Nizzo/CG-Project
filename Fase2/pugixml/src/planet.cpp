#include "planet.h"
#include "orbits.h"
#include <vector>

std::vector<Planet> criarPlanetas() {
    return {
        {"Sol", 1.0f, 0.0f, velocidade_orbita.at(0), "sun.3d"},
        {"Mercúrio", 0.38f, 5.0f, velocidade_orbita.at(1), "mercury.3d"},
        {"Vénus", 0.95f, 8.0f, velocidade_orbita.at(2), "venus.3d"},
        {"Terra", 1.0f, 12.0f, velocidade_orbita.at(3), "earth.3d"},
        {"Marte", 0.53f, 16.0f, velocidade_orbita.at(4), "mars.3d"},
        {"Júpiter", 11.2f, 52.0f, velocidade_orbita.at(5), "jupiter.3d"},
        {"Saturno", 9.45f, 95.0f, velocidade_orbita.at(6), "saturn.3d"},
        {"Urano", 4.0f, 192.0f, velocidade_orbita.at(7), "uranus.3d"},
        {"Neptuno", 3.88f, 300.0f, velocidade_orbita.at(8), "neptune.3d"}
    };
}

// Logica para o movimento dos planetas 