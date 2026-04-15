#ifndef PROBLEMINSTANCE_H
#define PROBLEMINSTANCE_H

#include <vector>
#include <string>

class ProblemInstance {
public:
    int numTasks = 0;
    int numMachines = 0;

    std::vector<int> processingTimes;

    bool loadFromFile(const std::string& filePath);
    void print() const;
};
#endif //PROBLEMINSTANCE_H
