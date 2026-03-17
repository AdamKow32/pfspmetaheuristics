#include "ProblemInstance.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <filesystem>

bool ProblemInstance::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open file "
          << (std::filesystem::current_path() / filePath) << std::endl;
        return false;
    }

    if (!(file >> numTasks >> numMachines)) return false;

    processingTimes.assign(numMachines, std::vector<int>(numTasks));

    for (int i = 0; i < numMachines; ++i) {
        for (int j = 0; j < numTasks; ++j) {
            if (!(file >> processingTimes[i][j])) {
                return false;
            }
        }
    }

    file.close();
    return true;
}

void ProblemInstance::print() const {
    std::cout << "Instance: " << numTasks << " tasks, " << numMachines <<  " machines" << std::endl;

    for (int i = 0; i < numMachines; ++i) {
        std::cout << "M" << i+1 << ": ";
        for (int j = 0; j < numTasks; ++j)
            std::cout << std::setw(3) << processingTimes[i][j] << " ";
    }
    std::cout << std::endl;
}