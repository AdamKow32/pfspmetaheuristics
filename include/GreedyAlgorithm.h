#ifndef GREEDYALGORITHM_H
#define GREEDYALGORITHM_H
#include "Evaluator.h"
#include "ProblemInstance.h"
#include "Individual.h"
class GreedyAlgorithm {
private:
    const ProblemInstance& instance_;
    Evaluator& evaluator_;

    int appendJob(std::vector<int>& c, int job, bool empty) const;

public:
    GreedyAlgorithm(const ProblemInstance& instance, Evaluator& evaluator);

    Individual solve(int startJob = -1) const;

    Individual solveBest() const;
};
#endif //GREEDYALGORITHM_H
