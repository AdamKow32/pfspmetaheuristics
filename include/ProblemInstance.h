#ifndef PROBLEMINSTANCE_H
#define PROBLEMINSTANCE_H

#include <vector>
#include <string>

class ProblemInstance {
public:
    // Number of tasks (J) + number of machines (M)
    int numTasks;
    int numMachines;

    // processingTime[M][J], creating matrix using two vectors
    std::vector<std::vector<int>> processingTimes;

    bool loadFromFile(const std::string& filePath);

    // help method to show data
    void print() const;
};
#endif //PROBLEMINSTANCE_H
