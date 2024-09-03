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
    EndcapConfiguration(const char* iniFile);
    EndcapConfiguration(const EndcapConfiguration& other);
    void loadConfiguration(const char* iniFile);
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
    double getStepLength() const { return step_length; }
    double getGapTolerance() const { return Gap_tolerance; }
    double getOverlapMax() const { return Overlap_max_mm; }
    int getNspecies() const { return N_species; }
    int getNMin() const { return N_min; }
    int getNMax() const { return N_max; }
    int getNRings() const { return N_rings; }
    double getR() const { return R; }

    // Setter method for step_length
    void setStepLength(double step) { step_length = step; }

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
    double step_length, Gap_tolerance, Overlap_max_mm;
    int N_species, N_min, N_max, N_rings;
    double R;

    std::vector<double> L1, L2, Hr;
    std::vector<int> npoly, types;
    std::vector<std::array<double, 2>> radius;
};

#endif // ENDCAP_CONFIGURATION_H