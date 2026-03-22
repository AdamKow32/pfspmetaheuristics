#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <vector>

struct Individual {
    std::vector<int> schedule;
    int fitness;

    Individual(const std::vector<int>& s, int f) : schedule(s), fitness(f) {}

    bool operator<(const Individual& other) const {
        return fitness < other.fitness;
    }
};

#endif //INDIVIDUAL_H
