#include "SimulatedAnnealing.h"
#include <cmath>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>

SimulatedAnnealing::SimulatedAnnealing(const ProblemInstance& instance, Evaluator& evaluator, const Config& config) : instance_(instance), evaluator_(evaluator), config_(config) {
    if (config.seed >= 0)
        randomGen_.seed(static_cast<unsigned>(config_.seed));
    else {
        std::random_device rd;
        randomGen_.seed(rd());
    }
}

Individual SimulatedAnnealing::run(int budget) {
    history_.clear();
    std::vector<int> sched(instance_.numTasks);
    std::iota(sched.begin(), sched.end(), 0);
    std::shuffle(sched.begin(), sched.end(), randomGen_);

    Individual current(sched,-1);
    evaluator_.evaluate(current);

    Individual best = current;
    double temp = config_.initialTemperature;
    int iter = 0;

    std::uniform_real_distribution coin(0.0,1.0);

    while (evaluator_.evalCount() < budget) {

        Individual next = neighbour(current);
        evaluator_.evaluate(next);

        int delta = next.fitness - current.fitness;

        if (delta < 0 || coin(randomGen_) < std::exp(-static_cast<double>(delta) / temp))
            current = next;

        if (current.fitness < best.fitness)
            best = current;

        if (temp > config_.minTemperature)
            temp *= config_.coolingRate;

        if (iter % 100 == 0)
            history_.push_back({iter, current.fitness, best.fitness});

        ++iter;
    }

    if (!config_.logFile.empty())
        saveLog();

    return best;
}

Individual SimulatedAnnealing::neighbour(const Individual& ind) const {
    Individual next = ind;
    const int J =static_cast<int>(next.schedule.size());

    std::uniform_int_distribution dist(0, J-1);
    int i = dist(randomGen_);
    int j;
    do{ j = dist(randomGen_); } while ( j==i );

    std::swap(next.schedule[i], next.schedule[j]);
    next.fitness = -1;
    return next;
}

void SimulatedAnnealing::saveLog() const {
    std::ofstream file(config_.logFile);
    if (!file.is_open()) return;
    file << "iteration;current;best\n";
    for (const auto& s : history_)
        file << s.iterator << ";"
             << s.current   << ";"
             << s.best     << "\n";
    std::cout << "CSV saved here " << config_.logFile << "\n";
}