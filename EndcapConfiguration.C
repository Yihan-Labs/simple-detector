// EndcapConfiguration.C

#include "EndcapConfiguration.h"
#include "PolygonUtils.h"
#include <TEnv.h>
#include <TMath.h>
#include <iostream>

EndcapConfiguration::EndcapConfiguration(const char* iniFile) {
    loadConfiguration(iniFile);
}

void EndcapConfiguration::loadConfiguration(const char* iniFile) {
    TEnv config(iniFile);
    R_min = config.GetValue("R_min", 414.0);
    R_max = config.GetValue("R_max", 690.0);
    L_min = config.GetValue("L_min", 25.0);
    L_max = config.GetValue("L_max", 85.0);
    Hreal_min = config.GetValue("Hreal_min", 75.0);
    Hreal_max = config.GetValue("Hreal_max", 145.0);
    costheta_min = config.GetValue("costheta_min", 0.7);
    costheta_max = config.GetValue("costheta_max", 1.0);
    step_length = config.GetValue("step_length", 0.5);
    Tolerance = config.GetValue("Tolerance", 1e-3);
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

    R = 105.0;

    initializeDefaultValues();
}

void EndcapConfiguration::initializeDefaultValues() {
    L1[0] = 2 * R_min * TMath::Sin(TMath::Pi() / N_min);
    L2[N_species - 1] = 2 * R_max * TMath::Tan(TMath::Pi() / N_max);
    npoly[0] = N_min;
    npoly[N_rings - 1] = N_max;
    types[0] = 0;
    types[N_rings - 1] = N_species - 1;
}

// buildRadius returns 1 if build success.
int EndcapConfiguration::buildRadius(double step) {
    // First, calculate all radii
    for (int i = 0; i < N_rings; i++) {
        radius[i][0] = PolygonUtils::CircumscribedRadius(L1[types[i]], npoly[i]);
        radius[i][1] = PolygonUtils::InscribedRadius(L2[types[i]], npoly[i]);
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
        printf("  Radius: [%.2f %.2f]", radius[i][0], radius[i][1]);
        printf("  ring height: %.2f", ringHeight);
        printf("\tcostheta: %.3f \n", tiltAngle);
    }
}
