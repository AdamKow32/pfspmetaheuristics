#include "Individual.h"

Individual::Individual() : schedule({}), fitness(-1) {}

Individual::Individual(const std::vector<int>& s, int f) : schedule(s), fitness(f) {}

// Porównuje dwóch osobników po fitness
bool Individual::operator<(const Individual &other) const {
    return fitness < other.fitness;
}

// zwraca true jeśli fitness > 0, żeby nie liczyć f(x) dwa razy
bool Individual::hasFitness() const {
    return fitness >= 0;
}

// metoda do debugowania
void Individual::print(std::ostream &os) const {
    os << "[";
    for (int i = 0; i < static_cast<int>(schedule.size()); ++i) {
        os << schedule[i] + 1;
        if (i + 1 < static_cast<int>(schedule.size())) os << ", ";
    }
    os << "] f=" << fitness;
}
