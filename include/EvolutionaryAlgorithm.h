#ifndef EVOLUTIONARYALGORITHM_H
#define EVOLUTIONARYALGORITHM_H
#include <vector>
#include <random>
#include "Individual.h"
#include "ProblemInstance.h"
class EvolutionaryAlgorithm {
private:
    const ProblemInstance& instance;
    std::mt19937 rng;

public:
    EvolutionaryAlgorithm(const ProblemInstance& inst);

    Individual createRandomIndividual;

    std::vector<Individual> initializePopulation(int size);

};
#endif //EVOLUTIONARYALGORITHM_H
