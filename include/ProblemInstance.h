#ifndef PROBLEMINSTANCE_H
#define PROBLEMINSTANCE_H

#include <vector>
#include <string>

class ProblemInstance {
public:
    int numTasks;
    int numMachines;

    std::vector<int> processingTimes;

    int p(int machine, int job) const;

    bool loadFromFile(const std::string& filePath);
    void print() const;
};
#endif //PROBLEMINSTANCE_H
