#include "GreedyAlgorithm.h"
#include <limits>
#include <algorithm>
#include <numeric>

GreedyAlgorithm::GreedyAlgorithm(const ProblemInstance &instance, Evaluator &evaluator) : instance_(instance), evaluator_(evaluator) {}

int GreedyAlgorithm::appendJob(std::vector<int> &c, int job, bool empty) const {
    const int M = instance_.numMachines;
    std::vector<int> cn(M);

    for (int m = 0; m < M; ++m) {
        int pt = instance_.processingTimes[m * instance_.numTasks + job];
        if (empty && m == 0) cn[m] = pt;
        else if (empty)      cn[m] = cn[m-1] + pt;
        else if (m==0)       cn[m] = cn[m] + pt;
        else                 cn[m] = std::max(c[m], cn[m-1])+ pt;
    }
    c=cn;
    return c[M-1];
}

Individual GreedyAlgorithm::solve(int startJob) const {
    const int J = instance_.numTasks;
    std::vector used (J, false);
    std::vector<int> sched;
    sched.reserve(J);
    std::vector c(instance_.numMachines, 0);
    bool empty = true;

    if (startJob >= 0 && startJob < J) {
        appendJob(c, startJob, empty);
        sched.push_back(startJob);
        used[startJob] = true;
        empty = false;
    }

    while (static_cast<int>(sched.size()) < J) {
        int bestJob = -1;
        int bestC = std::numeric_limits<int>::max();
        for (int j = 0; j < J; ++j) {
            if (used[j]) continue;
            std::vector<int> cTmp = c;
            int cj = appendJob(cTmp, j, empty);
            if (cj < bestC) { bestC = cj; bestJob = j; }
        }
        appendJob(c, bestJob, empty);
        sched.push_back(bestJob);
        used[bestJob] = true;
        empty = false;
    }

    Individual ind(sched, -1);
    evaluator_.evaluate(ind);
    return ind;
}

Individual GreedyAlgorithm::solveBest() const {
    Individual best;
    for (int s = 0; s < instance_.numTasks; ++s) {
        Individual cand = solve(s);
        if (!best.hasFitness() || cand.fitness < best.fitness)
            best = cand;
    }
    return best;
}
