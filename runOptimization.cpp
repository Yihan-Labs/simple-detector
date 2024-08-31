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

std::atomic<long> cycles(0);

int inline Roundn(double n) {
    return TMath::Nint(n);
}

double inline RoundtoN(double i, double n) {
    // Divide i by n, round to the nearest integer, and then multiply by n
    return TMath::Nint(i / n) * n;
}

int inline intToBinary(int n, int digit) {
    // Shift the number right by (digit - 1) and then extract the least significant bit
    return (n >> (digit - 1)) & 1;
}

int inline intPow(int x, unsigned int p) {
  if (p == 0) return 1;
  if (p == 1) return x;
  if (p == 2) return x * x;
  
  int tmp = intPow(x, p / 2);
  if (p % 2 == 0) return tmp * tmp;
  else return x * tmp * tmp;
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
        int n_star = Roundn(PolygonUtils::PolygonSides(r, L1[i]));
        if(n_star % 4 != 0) {continue;}
        double r_next = PolygonUtils::CircumscribedRadius(L1[i], n_star);
        double delta = TMath::Abs(r_next - r) / r;
        if (delta <= config.getTolerance()) {
            typenext.push_back(i);
            // We're not setting types and npoly here anymore, as we're collecting all possibilities
        }
    }
    return typenext;
}

void exploreRingConfigurations(EndcapConfiguration& config, std::vector<EndcapConfiguration>& config_list, int ringNumber, double step) {
    auto& L1 = config.getL1();
    auto& L2 = config.getL2();
    auto& npoly = config.getNpoly();
    auto& types = config.getTypes();

    if (ringNumber >= config.getNRings()) {
        if (config.buildRadius(step)) {
            //config.printConfiguration();
            EndcapConfiguration vconfig = config;
            config_list.push_back(std::move(vconfig));
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
        exploreRingConfigurations(config, config_list, ringNumber + 1, step);

        // Restore state for backtracking
        types[ringNumber] = oldType;
        npoly[ringNumber] = oldNpoly;
    }
}

void optimaN(EndcapConfiguration& config, std::vector<EndcapConfiguration>& config_list) {
    const int N_species = config.getNspecies();

    // Define a recursive lambda to handle nested loops
    std::function<void(int, EndcapConfiguration&, std::vector<EndcapConfiguration>&, int, double)> nestedLoops =
    [&](int depth, EndcapConfiguration& cfg, std::vector<EndcapConfiguration>& cfg_list, int thread_id, double step) {
        auto& L1 = cfg.getL1();
        auto& L2 = cfg.getL2();
        double L1offset = 0;
        double L2offset = step / 2;
        
        L2offset += step/2 * intToBinary(thread_id, (depth * 2) - 1);
        L1offset += step/2 * intToBinary(thread_id, (depth * 2));

        // For L2, start from Round(L1) + step
         for (L2[depth - 1] = Roundn(L1[depth-1]) + L2offset; L2[depth - 1] <= config.getLMax(); L2[depth - 1] += step) {
            if (depth == N_species - 1) {
                for (L1[depth] = config.getLMin() + L1offset; L1[depth] <= L2[depth] - step/2; L1[depth] += step) {
                    ++cycles;
                    exploreRingConfigurations(cfg, cfg_list, 1, step / 2);
                }
            } else {
                // For intermediate depths, L1 goes from LMin to LMax
                for (L1[depth] = config.getLMin() + L1offset; L1[depth] <= config.getLMax(); L1[depth] += step) {
                    nestedLoops(depth + 1, cfg, cfg_list, thread_id, step);
                }
            }
        }
    };

    // Function for each thread to execute
    auto threadWorker = [&](int thread_id, EndcapConfiguration thread_config, std::vector<EndcapConfiguration>& thread_config_list) {
        nestedLoops(1, thread_config, thread_config_list, thread_id, thread_config.getStepLength());
    };

    // Create and launch threads
    int num_threads = intPow(2, (N_species - 1) * 2);
    std::vector<std::thread> threads;
    std::vector<std::vector<EndcapConfiguration>> thread_config_lists(num_threads);  // List for each thread

    for (int i = 0; i < num_threads; ++i) {
        EndcapConfiguration thread_config = config; // Copy the configuration for each thread
        double steplength = thread_config.getStepLength();
        thread_config.setStepLength(steplength * 2);
        threads.emplace_back(threadWorker, i, std::move(thread_config), std::ref(thread_config_lists[i]));
    }

    // Join threads
    for (auto& thread : threads) {
        thread.join();
    }

    // Combine all thread-specific config lists into the main config_list
    for (const auto& thread_list : thread_config_lists) {
        config_list.insert(config_list.end(), thread_list.begin(), thread_list.end());
    }
}

// Main function
void runOptimization(EndcapConfiguration config, std::vector<EndcapConfiguration>& config_list) {
    
    if (config.getNspecies() >= 3) {
        optimaN(config , config_list);
    } else {
        std::cerr << "Unsupported number of species: " << config.getNspecies() << std::endl;
    }
}

// Entry point for ROOT
int main(int argc,char**argv) {
    TString config_file;
    if(argc>=2){
        config_file = argv[1];
    } else {
        config_file = "optimize.ini";
    }

    EndcapConfiguration config(config_file);

    // Output the read parameters
    printf("Tolerance: %.2e\n", config.getTolerance());
    printf("Radius min, max: = [%.2f, %.2f]\n", config.getRMin(), config.getRMax());
    printf("Hreal: [%.2f, %.2f]\n", config.getHrealMin(), config.getHrealMax());
    printf("costheta: [%.2f, %.2f]\n", config.getCosthetaMin(), config.getCosthetaMax());
    printf("N_species: %d\n", config.getNspecies());
    printf("N_rings: %d\n", config.getNRings());
    printf("polygon sides: [%d, %d]\n", config.getNMin(), config.getNMax());
    auto& L1 = config.getL1();
    auto& L2 = config.getL2();

    std::cout << L1[0] << " " << L2[config.getNspecies() - 1] << std::endl;

    std::vector<EndcapConfiguration> config_list;
    runOptimization(config, config_list);

    for(auto& cfg : config_list) {
        cfg.printConfiguration();
    }

    std::cout << "Total cycles: " << cycles.load() << std::endl;
    return 0;
}