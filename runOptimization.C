// runOptimization.C

#include "EndcapConfiguration.h"
#include "PolygonUtils.h"
#include <TEnv.h>
#include <TMath.h>
#include <iostream>
#include <functional>
#include <thread>
#include <vector>
#include <atomic>
#include <array>

int inline Roundn(double n) {
    return TMath::Nint(n);
}

// find the n and type of the next ring using current l2 and n.
std::vector<int> nextCircles(int currentRing, EndcapConfiguration& config) {
    auto& npoly = config.getNpoly();
    auto& L1 = config.getL1();
    auto& L2 = config.getL2();
    auto& types = config.getTypes();
    std::vector<int> typenext;

    if (currentRing < 1) {
        std::cerr << "current ring cannot be lower than 1!";
        return typenext;
    }

    double r = PolygonUtils::InscribedRadius(L2[types[currentRing - 1]], npoly[currentRing - 1]);

    // check if the next ring is the outer ring
    if (currentRing + 1 == config.getNRings()) {
        int i = types[currentRing];
        auto r_next = PolygonUtils::CircumscribedRadius(L1[i], npoly[currentRing]);
        auto delta = TMath::Abs(r_next - r) / r;
        if (delta <= config.getTolerance()) {
            typenext.push_back(i);
        }
        return typenext;
    }

    for (int i = 0; i < config.getNspecies(); i++) {
        double n_star = PolygonUtils::PolygonSides(r, L1[i]);
        double r_next = PolygonUtils::CircumscribedRadius(L1[i], Roundn(n_star));
        double delta = TMath::Abs(r_next - r) / r;
        if (delta <= config.getTolerance()) {
            typenext.push_back(i);
            // We're not setting types and npoly here anymore, as we're collecting all possibilities
        }
    }
    return typenext;
}

void exploreRingConfigurations(EndcapConfiguration& config, int ringNumber) {
    auto& L1 = config.getL1();
    auto& L2 = config.getL2();
    auto& npoly = config.getNpoly();
    auto& types = config.getTypes();

    if (ringNumber >= config.getNRings()) {
        if (config.buildRadius()) {
            config.printConfiguration();
        }
        return;
    }

    std::vector<int> typenext = nextCircles(ringNumber, config);
    for (int type : typenext) {
        // Save current state
        int oldType = types[ringNumber];
        int oldNpoly = npoly[ringNumber];

        // Set new type and npoly
        types[ringNumber] = type;
        double r = PolygonUtils::InscribedRadius(L2[types[ringNumber - 1]], npoly[ringNumber - 1]);
        double n_star = PolygonUtils::PolygonSides(r, L1[type]);
        npoly[ringNumber] = Roundn(n_star);

        // Recurse to next ring
        exploreRingConfigurations(config, ringNumber + 1);

        // Restore state for backtracking
        types[ringNumber] = oldType;
        npoly[ringNumber] = oldNpoly;
    }
}

void optimaN(EndcapConfiguration& config) {
    const int N_species = config.getNspecies();
    const double step = config.getStepLength();
    std::atomic<long> cycles(0);
    // const int num_threads = 16;

    // Define a recursive lambda to handle nested loops
    std::function<void(int, double, EndcapConfiguration&)> nestedLoops = [&](int depth, double step, EndcapConfiguration& cfg) {
        auto& L1 = cfg.getL1();
        auto& L2 = cfg.getL2();
        // For L2, start from Round(L1) + step
        for (L2[depth - 1] = Roundn(L1[depth-1]) + step; L2[depth - 1] <= config.getLMax(); L2[depth - 1] += step) {
            if (depth == N_species - 1) {
                for (L1[depth] = config.getLMin(); L1[depth] <= L2[depth] - step; L1[depth] += step) {
                    long current_cycles = ++cycles;
                    //if (current_cycles % 100000 == 0) {
                    //    std::cout << current_cycles << std::endl;
                    //}
                    exploreRingConfigurations(cfg, 1);
                }
            } else {
                // For intermediate depths, L1 goes from LMin to LMax
                for (L1[depth] = config.getLMin(); L1[depth] <= config.getLMax(); L1[depth] += step) {
                    nestedLoops(depth + 1, step, cfg);
                }
            }
        }
    };

    nestedLoops(1, step, config);

    std::cout << "Total cycles: " << cycles.load() << std::endl;
}

// Main function
void runOptimization(EndcapConfiguration config) {
    auto& L1 = config.getL1();
    auto& L2 = config.getL2();

    std::cout << L1[0] << " " << L2[config.getNspecies() - 1] << std::endl;
    
    if (config.getNspecies() >= 3) {
        optimaN(config);
    } else {
        std::cerr << "Unsupported number of species: " << config.getNspecies() << std::endl;
    }
}

// Entry point for ROOT
int main() {
    EndcapConfiguration config("optimize.ini");

    // Output the read parameters
    printf("Tolerance: %.2e\n", config.getTolerance());
    printf("Radius min, max: = [%.2f, %.2f]\n", config.getRMin(), config.getRMax());
    printf("Hreal: [%.2f, %.2f]\n", config.getHrealMin(), config.getHrealMax());
    printf("costheta: [%.2f, %.2f]\n", config.getCosthetaMin(), config.getCosthetaMax());
    printf("N_species: %d\n", config.getNspecies());
    printf("N_rings: %d\n", config.getNRings());
    printf("polygon sides: [%d, %d]\n", config.getNMin(), config.getNMax());

    runOptimization(config);
    return 0;
}