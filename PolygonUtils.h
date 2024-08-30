#ifndef POLYGON_UTILS_H
#define POLYGON_UTILS_H

#include "TMath.h"
#include <iostream>

class PolygonUtils {
public:
    static double InscribedRadius(double L, int n);
    static double CircumscribedRadius(double L, int n);
    
    static float PolygonSides(double r, double L);
    static inline int Roundn(double n) {return TMath::Nint(n);};
    static inline double H_Max(double L1, double L2, double R);
};

#endif // POLYGON_UTILS_H
