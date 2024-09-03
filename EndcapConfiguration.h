// EndcapConfiguration.h

#ifndef ENDCAP_CONFIGURATION_H
#define ENDCAP_CONFIGURATION_H

#include <TEnv.h>
#include <TMath.h>
#include <iostream>
#include <vector>
#include <array>

class EndcapConfiguration {

void inline initializeDefaultValues();

public:
    EndcapConfiguration(TEnv& config);
    EndcapConfiguration(const EndcapConfiguration& other);
    void loadConfiguration(TEnv& config);
    int buildRadius(double step);
    void printConfiguration() const;

    // Getter methods
    double getRMin() const { return R_min; }
    double getRMax() const { return R_max; }
    double getLMin() { return L_min; }
    double getLMax() { return L_max; }
    double getHrealMin() const { return Hreal_min; }
    double getHrealMax() const { return Hreal_max; }
    double getCosthetaMin() const { return costheta_min; }
    double getCosthetaMax() const { return costheta_max; }
    double getGapTolerance() const { return Gap_tolerance; }
    double getOverlapMax() const { return Overlap_max_mm; }
    int getNspecies() const { return N_species; }
    int getNMin() const { return N_min; }
    int getNMax() const { return N_max; }
    int getNRings() const { return N_rings; }
    double getR() const { return R; }
    double getInnerRadius(int ring_number);
    double getOuterRadius(int ring_number);

    // Setter method for step_length
    void setRMin(double min) { R_min = min; }
    void setRMax(double max) { R_max = max; }
    void setLMin(double min) { L_min = min; }
    void setLMax(double max) { L_max = max; }
    void setHrealMin(double min) { Hreal_min = min; }
    void setHrealMax(double max) { Hreal_max = max; }
    void setCosthetaMin(double min) { costheta_min = min; }
    void setCosthetaMax(double max) { costheta_max = max; }
    void setGapTolerance(double tol) { Gap_tolerance = tol; }
    void setOverlapMax(double max) { Overlap_max_mm = max; }

    double static InscribedRadius(double L, int n);
    double static CircumscribedRadius(double L, int n);
    float static PolygonSides(double r, double L);

    std::vector<double>& getL1() { return L1; }
    std::vector<double>& getL2() { return L2; }
    std::vector<double>& getHr() { return Hr; }
    std::vector<int>& getNpoly() { return npoly; }
    std::vector<int>& getTypes() { return types; }
    std::vector<std::array<double, 2>>& getRadius() { return radius; }

private:
    double R_max, R_min, L_min, L_max;
    double Hreal_max, Hreal_min;
    double costheta_max, costheta_min;
    double Gap_tolerance, Overlap_max_mm;
    int N_species, N_min, N_max, N_rings;
    double R;

    std::vector<double> L1, L2, Hr;
    std::vector<int> npoly, types;
    std::vector<std::array<double, 2>> radius;
};

#endif // ENDCAP_CONFIGURATION_H