#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <random>
#include <string>
#include "ProblemInstance.h"
#include "Evaluator.h"
#include "GreedyAlgorithm.h"
#include "EvolutionaryAlgorithm.h"
#include "SimulatedAnnealing.h"

struct RunStats {
    int    best;
    int    worst;
    double avg;
    double std;
};

RunStats computeRunStats(const std::vector<int>& results) {
    RunStats s;
    s.best  = *std::min_element(results.begin(), results.end());
    s.worst = *std::max_element(results.begin(), results.end());
    s.avg   = std::accumulate(results.begin(), results.end(), 0.0)
              / static_cast<double>(results.size());
    double var = 0.0;
    for (int r : results) var += (r - s.avg) * (r - s.avg);
    s.std = std::sqrt(var / static_cast<double>(results.size()));
    return s;
}

void printRunStats(const std::string& label, const RunStats& s) {
    std::cout << std::left  << std::setw(12) << label
              << "best="    << std::setw(8)  << s.best
              << "worst="   << std::setw(8)  << s.worst
              << "avg="     << std::fixed    << std::setprecision(1)
                            << std::setw(10) << s.avg
              << "std="     << std::setprecision(1) << s.std
              << std::endl;
}

std::string instanceName(const std::string& path) {
    size_t slash = path.find_last_of("/\\");
    std::string name = (slash == std::string::npos) ? path : path.substr(slash + 1);
    size_t dot = name.find_last_of('.');
    return (dot == std::string::npos) ? name : name.substr(0, dot);
}


void saveSummaryCSV(const std::string& instance,
                    const RunStats& random,
                    const RunStats& greedy,
                    const RunStats& ea,
                    const RunStats& sa)
{
    std::ofstream file("results.csv", std::ios::app);
    if (!file.is_open()) return;

    // Write header only if file is empty
    if (file.tellp() == 0)
        file << "instance;method;best;worst;avg;std\n";

    auto write = [&](const std::string& method, const RunStats& s) {
        file << instance << ";" << method << ";"
             << s.best   << ";" << s.worst << ";"
             << std::fixed << std::setprecision(2)
             << s.avg    << ";" << s.std   << "\n";
    };

    write("Random", random);
    write("Greedy", greedy);
    write("EA",     ea);
    write("SA",     sa);

    std::cout << "Results saved -> results.csv" << std::endl;
}

Individual randomSearch(const ProblemInstance& inst, Evaluator& eval,
                        int budget, std::mt19937& rng,
                        std::ofstream& log)
{
    Individual best;
    std::vector<int> sched(inst.numTasks);
    std::iota(sched.begin(), sched.end(), 0);

    while (eval.evalCount() < budget) {
        std::shuffle(sched.begin(), sched.end(), rng);
        Individual ind(sched, -1);
        eval.evaluate(ind);
        if (!best.hasFitness() || ind.fitness < best.fitness)
            best = ind;
        if (log.is_open() && eval.evalCount() % 100 == 0)
            log << eval.evalCount() << ";" << best.fitness << "\n";
    }
    return best;
}

RunStats runRandomSearch(const ProblemInstance& inst, Evaluator& eval,
                         int budget, int runs, std::mt19937& rng,
                         const std::string& instance)
{
    std::cout << "Random Search" << std::endl;
    std::vector<int> results;
    std::ofstream log;

    for (int r = 0; r < runs; ++r) {
        eval.resetCount();
        if (r == runs - 1) {
            log.open("convergence_random_" + instance + ".csv");
            log << "evals;best\n";
        }
        Individual best = randomSearch(inst, eval, budget, rng, log);
        results.push_back(best.fitness);
    }

    auto stats = computeRunStats(results);
    printRunStats("Random:", stats);
    std::cout << std::endl;
    return stats;
}

RunStats runGreedy(const ProblemInstance& inst, Evaluator& eval,
                   int budget, int runs, std::mt19937& rng,
                   const std::string& instance)
{
    std::cout << "Greedy" << std::endl;
    std::vector<int> results;
    std::uniform_int_distribution<int> startDist(0, inst.numTasks - 1);

    for (int r = 0; r < runs; ++r) {
        eval.resetCount();
        GreedyAlgorithm greedy(inst, eval);
        Individual best;

        std::ofstream log;
        if (r == runs - 1) {
            log.open("convergence_greedy_" + instance + ".csv");
            log << "evals;best\n";
        }

        while (eval.evalCount() < budget) {
            int startJob = startDist(rng);
            Individual cand = greedy.solve(startJob);
            if (!best.hasFitness() || cand.fitness < best.fitness)
                best = cand;
            if (log.is_open())
                log << eval.evalCount() << ";" << best.fitness << "\n";
        }

        results.push_back(best.fitness);
        std::cout << "  Run " << std::setw(2) << (r + 1)
                  << ": " << best.fitness
                  << "  (evals=" << eval.evalCount() << ")" << std::endl;
    }

    auto stats = computeRunStats(results);
    printRunStats("Greedy:", stats);
    std::cout << std::endl;
    return stats;
}

RunStats runEA(const ProblemInstance& inst, Evaluator& eval,
               EvolutionaryAlgorithm::Config& cfg,
               int runs, const std::string& instance)
{
    std::cout << "Evolutionary Algorithm" << std::endl;
    std::vector<int> results;
    int bestRunIdx = 0;

    for (int r = 0; r < runs; ++r) {
        eval.resetCount();
        cfg.seed    = r * 1000;
        cfg.logFile = "convergence_ea_" + instance + "_tmp" + std::to_string(r) + ".csv";
        EvolutionaryAlgorithm ea(inst, eval, cfg);
        Individual best = ea.run();
        results.push_back(best.fitness);
        if (best.fitness < results[bestRunIdx]) bestRunIdx = r;
        std::cout << "  Run " << std::setw(2) << (r + 1)
                  << ": " << best.fitness
                  << "  (evals=" << eval.evalCount() << ")" << std::endl;
    }
    cfg.logFile = "";

    for (int r = 0; r < runs; ++r) {
        std::string f = "convergence_ea_" + instance + "_tmp" + std::to_string(r) + ".csv";
        if (r == bestRunIdx)
            std::rename(f.c_str(), ("convergence_ea_" + instance + ".csv").c_str());
        else
            std::remove(f.c_str());
    }

    auto stats = computeRunStats(results);
    printRunStats("EA:", stats);
    std::cout << std::endl;
    return stats;
}

RunStats runSA(const ProblemInstance& inst, Evaluator& eval,
               SimulatedAnnealing::Config& cfg,
               int budget, int runs, const std::string& instance)
{
    std::cout << "Simulated Annealing" << std::endl;
    std::vector<int> results;

    for (int r = 0; r < runs; ++r) {
        eval.resetCount();
        cfg.seed = r * 1000;
        SimulatedAnnealing sa(inst, eval, cfg);
        Individual best = sa.run(budget);
        results.push_back(best.fitness);
        std::cout << "  Run " << std::setw(2) << (r + 1)
                  << ": " << best.fitness
                  << "  (evals=" << eval.evalCount() << ")" << std::endl;
    }

    cfg.seed    = 42;
    cfg.logFile = "convergence_sa_" + instance + ".csv";
    eval.resetCount();
    SimulatedAnnealing(inst, eval, cfg).run(budget);
    cfg.logFile = "";

    auto stats = computeRunStats(results);
    printRunStats("SA:", stats);
    std::cout << std::endl;
    return stats;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: ./pfsp <path_to_instance.fsp>" << std::endl;
        std::cerr << "Example: ./pfsp data/tai20_5_0.fsp"   << std::endl;
        return 1;
    }

    ProblemInstance inst;
    if (!inst.loadFromFile(argv[1])) return 1;
    inst.print();
    std::cout << std::endl;

    const std::string instance = instanceName(argv[1]);

    Evaluator eval(inst);
    std::mt19937 rng(std::random_device{}());

    EvolutionaryAlgorithm::Config eaCfg;
    eaCfg.popSize        = 100;
    eaCfg.generations    = 100;
    eaCfg.px             = 0.7;
    eaCfg.pm             = 0.1;
    eaCfg.tournamentSize = 10;
    eaCfg.eliteCount     = 2;
    eaCfg.mutationType   = "inversion";
    eaCfg.crossoverType  = "ox";
    eaCfg.initType       = "random";
    eaCfg.logFile        = "";

    SimulatedAnnealing::Config saCfg;
    saCfg.initialTemperature = 10000.0;
    saCfg.coolingRate        = 0.995;
    saCfg.minTemperature     = 0.1;
    saCfg.logFile            = "";

    const int budget = eaCfg.popSize * eaCfg.generations;
    const int runs   = 10;

    std::cout << "Budget: " << budget << " evaluations  |  Runs: " << runs
              << std::endl << std::endl;

    auto randomStats = runRandomSearch(inst, eval, budget, runs, rng, instance);
    auto greedyStats = runGreedy      (inst, eval, budget, 1, rng, instance);
    auto eaStats     = runEA          (inst, eval, eaCfg,  runs, instance);
    auto saStats     = runSA          (inst, eval, saCfg,  budget, runs, instance);

    printRunStats("Random:", randomStats);
    printRunStats("Greedy:", greedyStats);
    printRunStats("EA:",     eaStats);
    printRunStats("SA:",     saStats);

    saveSummaryCSV(instance, randomStats, greedyStats, eaStats, saStats);


    return 0;
}