#ifndef POLYGON_UTILS_H
#define POLYGON_UTILS_H

#include "TMath.h"
#include <vdt/sin.h>
#include <vdt/asin.h>
#include <vdt/tan.h>
#include <iostream>

class PolygonUtils {
public:
    static double InscribedRadius(double L, int n);
    static double CircumscribedRadius(double L, int n);
    static void InscribedRadiusV(const std::vector<double>& L, const std::vector<int>& n, std::vector<double>& results);
    static void CircumscribedRadiusV(const std::vector<double>& L, const std::vector<int>& n, std::vector<double>& results);
    static float PolygonSides(double r, double L);
    static inline int Roundn(double n) {return TMath::Nint(n);};
    static inline double H_Max(double L1, double L2, double R);
};

#endif // POLYGON_UTILS_H
