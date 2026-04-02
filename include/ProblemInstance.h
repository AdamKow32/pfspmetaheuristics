#ifndef PROBLEMINSTANCE_H
#define PROBLEMINSTANCE_H

#include <vector>
#include <string>

class ProblemInstance {
public:
    int numTasks;
    int numMachines;

    std::vector<int> processingTimes;

    bool loadFromFile(const std::string& filePath);
    void print() const;
};
#endif //PROBLEMINSTANCE_H
