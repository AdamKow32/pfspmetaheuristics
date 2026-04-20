#include "Evaluator.h"
#include <algorithm>

Evaluator::Evaluator(const ProblemInstance& instance) : inst(instance) {}

int Evaluator::evaluate(const std::vector<int>& schedule) const {
    ++evalCount_;

    const int J = inst.numTasks;
    const int M = inst.numMachines;

    std::vector c(J * M, 0);

    for (int j = 0; j < J; ++j) {
        for (int m = 0; m < M; ++m) {
            int pt = inst.processingTimes[m * J + schedule[j]];
            if      (j == 0 && m == 0) c[j*M + m] = pt;
            else if (j == 0)           c[j*M + m] = c[j*M + m-1] + pt;
            else if (m == 0)           c[j*M + m] = c[(j-1)*M + m] + pt;
            else                       c[j*M + m] = std::max(c[(j-1)*M + m], c[j*M + m-1]) + pt;
        }
    }

    int total = 0;
    for (int j = 0; j < J; ++j)
        total += c[j*M + (M-1)];
    return total;
}


void Evaluator::evaluate(Individual &ind) const {
    ind.fitness = evaluate(ind.schedule);
}

int Evaluator::evalCount() const {
    return evalCount_;
}

void Evaluator::resetCount() const {
    evalCount_ = 0;
}




