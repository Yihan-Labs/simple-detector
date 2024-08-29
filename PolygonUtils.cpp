#include "PolygonUtils.h"

// Inscribed radius of a regular polygon
double PolygonUtils::InscribedRadius(double L, int n) {
    if (n <= 2) {
        std::cerr << "Error: A polygon must have at least 3 sides." << std::endl;
        return -9999.5;
    }
    return L / (2 * TMath::Tan(M_PI / n));
}

// Circumscribed radius of a regular polygon
double PolygonUtils::CircumscribedRadius(double L, int n) {
    if (n <= 2) {
        std::cerr << "Error: A polygon must have at least 3 sides." << std::endl;
        return -9999.5;
    }
    return L / (2 * TMath::Sin(M_PI / n));
}

// Vectorized version of Inscribed Radius
void PolygonUtils::InscribedRadiusV(const std::vector<double>& L, const std::vector<int>& n, std::vector<double>& results) {
    size_t size = L.size();
    if (size != n.size() || size != results.size()) {
        std::cerr << "Error: Input vectors must have the same size." << std::endl;
        return;
    }

    // Allocate memory for intermediate Pi/n values
    std::vector<double> angles(size);
    for (size_t i = 0; i < size; ++i) {
        if (n[i] <= 2) {
            std::cerr << "Error: A polygon must have at least 3 sides." << std::endl;
            results[i] = -9999.5;
        } else {
            angles[i] = M_PI / n[i];  // Store Pi/n in angles array
        }
    }

    // Perform the vectorized tangent operation
    vdt::fast_tanv(static_cast<uint32_t>(size), angles.data(), angles.data());

    // Calculate the inscribed radii
    for (size_t i = 0; i < size; ++i) {
        if (n[i] > 2) {
            results[i] = L[i] / (2 * angles[i]);  // L / (2 * tan(Pi/n))
        }
    }
}

// Vectorized version of Circumscribed Radius
void PolygonUtils::CircumscribedRadiusV(const std::vector<double>& L, const std::vector<int>& n, std::vector<double>& results) {
    size_t size = L.size();
    if (size != n.size() || size != results.size()) {
        std::cerr << "Error: Input vectors must have the same size." << std::endl;
        return;
    }

    // Allocate memory for intermediate Pi/n values
    std::vector<double> angles(size);
    for (size_t i = 0; i < size; ++i) {
        if (n[i] <= 2) {
            std::cerr << "Error: A polygon must have at least 3 sides." << std::endl;
            results[i] = -9999.5;
        } else {
            angles[i] = M_PI / n[i];  // Store Pi/n in angles array
        }
    }

    // Perform the vectorized sine operation
    vdt::fast_sinv(static_cast<uint32_t>(size), angles.data(), angles.data());

    // Calculate the circumscribed radii
    for (size_t i = 0; i < size; ++i) {
        if (n[i] > 2) {
            results[i] = L[i] / (2 * angles[i]);  // L / (2 * sin(Pi/n))
        }
    }
}

// Number of sides of a regular polygon
float PolygonUtils::PolygonSides(double r, double L) {
    if (L >= 2 * r) {
        std::cerr << "Error: The side length must be smaller than the diameter (2 * r)." << std::endl;
        return -9999.5;
    }
    float sin_term = L / (2 * r);
    return M_PI / TMath::ASin(sin_term);
}

double PolygonUtils::H_Max(double L1, double L2, double R) {
    double h1 = sqrt(R*R - L1*L1/4.0);
    double h2 = sqrt(R*R - L2*L2/4.0);
    return h1 + h2;
}