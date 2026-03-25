#ifndef EVALUATOR_H
#define EVALUATOR_H
#include "ProblemInstance.h"
#include "Individual.h"

class Evaluator {
private:
    const ProblemInstance& inst;
    mutable int evalCount_{};

public:
    explicit Evaluator(const ProblemInstance& instance);

    int evaluate(const std::vector<int>& schedule) const;
    void evaluate(Individual& ind) const;

    int evalCount() const;
    void resetCount() const;
};
#endif //EVALUATOR_H
