#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H
#include <vector>
#include <ostream>
struct Individual{
    std::vector<int> schedule;

    int fitness;

    Individual();
    Individual(const std::vector<int>& s, int f) : schedule(s), fitness(f) {}

    bool operator<(const Individual& other) const {
        return fitness < other.fitness;
    }

    bool hasFitness() const;
    void print(std::ostream& os) const;
};
#endif //INDIVIDUAL_H
