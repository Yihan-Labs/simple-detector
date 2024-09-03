// EndcapConfiguration.C

#include "EndcapConfiguration.h"
#include <TEnv.h>
#include <TMath.h>
#include <iostream>

EndcapConfiguration::EndcapConfiguration(TEnv& config) {
    loadConfiguration(config);
}

void EndcapConfiguration::loadConfiguration(TEnv& config) {
    R_min = config.GetValue("R_min", 414.0);
    R_max = config.GetValue("R_max", 690.0);
    L_min = config.GetValue("L_min", 25.0);
    L_max = config.GetValue("L_max", 85.0);
    Hreal_min = config.GetValue("Hreal_min", 75.0);
    Hreal_max = config.GetValue("Hreal_max", 145.0);
    costheta_min = config.GetValue("costheta_min", 0.7);
    costheta_max = config.GetValue("costheta_max", 1.0);
    Gap_tolerance = config.GetValue("Gap_tolerance", 1e-3);
    Overlap_max_mm = config.GetValue("Overlap_max_mm", 2);
    N_species = config.GetValue("N_species", 3);
    N_rings = config.GetValue("N_rings", 3);
    N_min = config.GetValue("N_min", 48);
    N_max = config.GetValue("N_max", 64);

    L1.resize(N_species, 0.0);
    L2.resize(N_species, 0.0);
    Hr.resize(N_species, 0.0);
    npoly.resize(N_rings, 0);
    types.resize(N_rings, 0);
    radius.resize(N_rings, std::array<double, 2>{0.0, 0.0});

    R = 101.5;

    initializeDefaultValues();
}

// Copy constructor
EndcapConfiguration::EndcapConfiguration(const EndcapConfiguration& other) {
    R_max = other.R_max;
    R_min = other.R_min;
    L_min = other.L_min;
    L_max = other.L_max;
    Hreal_max = other.Hreal_max;
    Hreal_min = other.Hreal_min;
    costheta_max = other.costheta_max;
    costheta_min = other.costheta_min;
    Gap_tolerance = other.Gap_tolerance;
    Overlap_max_mm = other.Overlap_max_mm;
    N_species = other.N_species;
    N_min = other.N_min;
    N_max = other.N_max;
    N_rings = other.N_rings;
    R = other.R;

    // Copy the vectors (deep copy)
    L1 = other.L1;
    L2 = other.L2;
    Hr = other.Hr;
    npoly = other.npoly;
    types = other.types;
    radius = other.radius;
}

void EndcapConfiguration::initializeDefaultValues() {
    L1[0] = 2 * R_min * TMath::Sin(TMath::Pi() / N_min);
    L2[N_species - 1] = 2 * R_max * TMath::Tan(TMath::Pi() / N_max);
    npoly[0] = N_min;
    npoly[N_rings - 1] = N_max;
    types[0] = 0;
    types[N_rings - 1] = N_species - 1;
}

double EndcapConfiguration::getInnerRadius(int ringn) {
    return CircumscribedRadius(L1[types[ringn]], npoly[ringn]);
}

double EndcapConfiguration::getOuterRadius(int ringn) {
    return InscribedRadius(L2[types[ringn]], npoly[ringn]);
}

// buildRadius returns 1 if build success.
int EndcapConfiguration::buildRadius(double step) {
    // First, calculate all radii
    for (int i = 0; i < N_rings; i++) {
        radius[i][0] = CircumscribedRadius(L1[types[i]], npoly[i]);
        radius[i][1] = InscribedRadius(L2[types[i]], npoly[i]);
    }

    // Now, determine Hr for each sensor type
    for (int type = 0; type < N_species; type++) {
        bool foundValidHr = false;
        for (double h = Hreal_min; h <= Hreal_max; h += step) {
            bool validForAllRings = true;
            
            // Check this Hr value against all rings of this sensor type
            for (int i = 0; i < N_rings; i++) {
                if (types[i] == type) {
                    double ringHeight = radius[i][1] - radius[i][0];
                    if (!(ringHeight > h * costheta_min && ringHeight < h * costheta_max)) {
                        validForAllRings = false;
                        break;
                    }
                }
            }

            if (validForAllRings) {
                Hr[type] = h;
                foundValidHr = true;
                break;
            }
        }

        if (!foundValidHr) {
            return 0; // Build failed
        }
    }

    return 1; // Build succeeded
}

template <typename T>
void printVector(const std::vector<T>& vec, const char* label, const char* format) {
    printf("%s: [", label);
    for (std::size_t i = 0; i < vec.size(); ++i) {
        printf(format, vec[i]);
        if (i < vec.size() - 1) printf(" ");
    }
    printf("]\n");
}

void EndcapConfiguration::printConfiguration() const {
    // Print L1 and L2 vectors
    printVector(L1, "L1", "%.2f");
    printVector(L2, "L2", "%.2f");

    // Print npoly and types vectors
    printVector(npoly, "npoly", "%d");
    printVector(types, "types", "%d");

    // Print Hr vector
    printVector(Hr, "sensor Height", "%.2f");

    // Print radius, Hr, and tilt for each ring
    for (auto i = 0; i < getNRings(); ++i) {
        double ringHeight = radius[i][1] - radius[i][0];
        double sensorHeight = Hr[types[i]];
        double tiltAngle = (ringHeight / sensorHeight);

        printf("Ring %d", i + 1);
        printf("  Radius: [%.3f %.3f]", radius[i][0], radius[i][1]);
        printf("\tring height: %.3f", ringHeight);
        printf("\tcostheta: %.5f \n", tiltAngle);
    }
}

// Inscribed radius of a regular polygon
double EndcapConfiguration::InscribedRadius(double L, int n) {
    if (n <= 2) {
        std::cerr << "Error: A polygon must have at least 3 sides." << std::endl;
        return -9999.5;
    }
    return L / (2 * TMath::Tan(TMath::Pi() / n));
}

// Circumscribed radius of a regular polygon
double EndcapConfiguration::CircumscribedRadius(double L, int n) {
    if (n <= 2) {
        std::cerr << "Error: A polygon must have at least 3 sides." << std::endl;
        return -9999.5;
    }
    return L / (2 * TMath::Sin(TMath::Pi() / n));
}

// Number of sides of a regular polygon
float EndcapConfiguration::PolygonSides(double r, double L) {
    if (L >= 2 * r) {
        std::cerr << "Error: The side length must be smaller than the diameter (2 * r)." << std::endl;
        return -9999.5;
    }
    float sin_term = L / (2 * r);
    return TMath::Pi() / TMath::ASin(sin_term);
}
