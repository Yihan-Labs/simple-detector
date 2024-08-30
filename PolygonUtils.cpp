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