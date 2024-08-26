#ifndef POLYGON_UTILS_H
#define POLYGON_UTILS_H

#include "TMath.h"
#include <iostream>

class PolygonUtils {
public:
    static double InscribedRadius(double L, int n);
    static double CircumscribedRadius(double L, int n);
    static double PolygonSides(double r, double L);
    static int Roundn(double n);
    static double H_Max(double L1, double L2, double R);
};

#endif // POLYGON_UTILS_H
