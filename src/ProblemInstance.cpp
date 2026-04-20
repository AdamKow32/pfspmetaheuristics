#include "ProblemInstance.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <sstream>

bool ProblemInstance::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        return false;
    }
    std::string line;

    while (std::getline(file, line)) {
        if (line.find("number of jobs") != std::string::npos) {
            if (!std::getline(file, line)) return false;
            std::istringstream ss(line);
            if (!(ss >> numTasks >> numMachines)) return false;
            break;
        }
    }

    if (numTasks == 0 || numMachines == 0) {
        return false;
    }

    while (std::getline(file, line)) {
        if (line.find("processing times") != std::string::npos) break;
    }

    processingTimes.resize(numMachines * numTasks);
    for (int m = 0; m < numMachines; ++m) {
        for (int j = 0; j < numTasks; ++j) {
            if (!(file >> processingTimes[m * numTasks + j])) {
                return false;
            }
        }
    }
    return true;
}

void ProblemInstance::print() const {
    std::cout << "Instance: " << numTasks << " tasks, " << numMachines <<  " machines" << std::endl;

    for (int m = 0; m < numMachines; ++m) {
        std::cout << "M" << m+1 << ": ";
        for (int j = 0; j < numTasks; ++j)
            std::cout << std::setw(4) << processingTimes[m * numTasks + j] << " ";
        std::cout << std::endl;
    }
}