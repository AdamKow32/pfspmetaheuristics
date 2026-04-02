#ifndef SIMULATEDANNEALING_H
#define SIMULATEDANNEALING_H
#include <vector>
#include <string>
#include <random>
#include "Individual.h"
#include "ProblemInstance.h"
#include "Evaluator.h"

class SimulatedAnnealing {
public:
    struct Config {
        double initialTemperature = 1000.0;
        double coolingRate = 0.995;
        double minTemperature = 0.1;
        int seed = -1;
        std::string logFile = "results_SA.csv";
    };

    SimulatedAnnealing(const ProblemInstance& instance,Evaluator& evaluator, const Config& config);
    Individual run(int budget);

private:
    const ProblemInstance& instance_;
    Evaluator& evaluator_;
    const Config& config_;
    mutable std::mt19937 randomGen_;

    struct IterationStats{int iterator; int current; int best;};
    std::vector<IterationStats> history_;

    Individual neighbour(const Individual& ind) const;
    void saveLog() const;
};
#endif //SIMULATEDANNEALING_H
