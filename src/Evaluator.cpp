#include "Evaluator.h"
#include <algorithm>
#include <stdexcept>

Evaluator::Evaluator(const ProblemInstance& instance) : inst(instance){
    if (instance.numTasks == 0 || instance.numMachines == 0)
        throw std::invalid_argument("Problem Instance is empty (Evaluator)");
}

int Evaluator::evaluate(const std::vector<int>& schedule) const {
    ++evalCount_;

    const int J = inst.numTasks;
    const int M = inst.numMachines;

    std::vector c(J, std::vector(M,0));

    for (int j = 0; j < J; ++j) {
        for (int m = 0; m < M; ++m) {
            int pt = inst.processingTimes[m][schedule[j]];
            if (j == 0 && m == 0) c[j][m] = pt;
            else if (j == 0) c[j][m] = c[j][m-1] + pt;
            else if (m == 0) c[j][m] = c[j-1][m] + pt;
            else c[j][m] = std::max(c[j-1][m],c[j][m-1]) + pt;
        }
    }

    int total = 0;
    for (int j = 0; j < J; ++j)
        total += c[j][M-1];
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




