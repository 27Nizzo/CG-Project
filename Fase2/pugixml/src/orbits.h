#ifndef ORBITS_H
#define ORBITS_H

#include <map>

const std::map<int, float> velocidade_orbita = {
    {0, 0.0},   // Sol
    {1, 0.05},  // Mercúrio
    {2, 0.03},  // Vénus
    {3, 0.02},  // Terra
    {4, 0.015}, // Marte
    {5, 0.008}, // Júpiter
    {6, 0.005}, // Saturno
    {7, 0.003}, // Urano
    {8, 0.001}  // Neptuno
};

#endif 

// Guarda as orbitais dos planetas 