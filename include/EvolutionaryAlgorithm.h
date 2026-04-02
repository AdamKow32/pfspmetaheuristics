#ifndef EVOLUTIONARYALGORITHM_H
#define EVOLUTIONARYALGORITHM_H
#include <vector>
#include <string>
#include <random>
#include "Individual.h"
#include "ProblemInstance.h"
#include "Evaluator.h"
class EvolutionaryAlgorithm {
public:
    struct Config{
        int    popSize        = 100;
        int    generations    = 100;
        double px             = 0.7;
        double pm             = 0.1;
        int    tournamentSize = 5;
        int    eliteCount     = 2;
        std::string mutationType  = "inversion";
        std::string crossoverType = "ox";
        std::string initType      = "random";
        int    seed           = -1;
        std::string logFile   = "resultsEA.csv";
    };

    struct GenerationStatistics {
        int    generation;
        int    best;
        double avg;
        int    worst;
    };


    EvolutionaryAlgorithm(const ProblemInstance& inst, Evaluator& evaluator, const Config& config);

    Individual run();

    const std::vector<GenerationStatistics>& history() const;

private:
    const ProblemInstance& instance;
    Evaluator& evaluator_;
    const Config& config_;
    mutable std::mt19937 random;

    std::vector<Individual> population;
    Individual bestEver;
    std::vector<GenerationStatistics> history_;

    // Initialization
    Individual createRandomIndividual() const;
    std::vector<Individual> initializePopulation(int size);
    void evaluateAll(std::vector<Individual>& population);

    // Selection
    const Individual& tournament() const;
    std::vector<Individual> elite(int count) const;

    // Crossover
    static std::pair<int, int> randomSegment(int J, std::mt19937& rng);
    Individual ox(const Individual& p1, const Individual& p2) const;
    Individual pmx(const Individual& p1, const Individual& p2) const;
    Individual applyCrossover(const Individual& p1, const Individual& p2);

    // Mutation
    Individual swapMutation(Individual ind) const;
    Individual inversionMutation(Individual ind) const;
    Individual applyMutation(Individual ind) const;

    // Help functions
    GenerationStatistics computeStatistics(int gen) const;
    void updateBest(const Individual& ind);
    void saveLogs() const;
};

#endif //EVOLUTIONARYALGORITHM_H
