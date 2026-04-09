#include "EvolutionaryAlgorithm.h"
#include "GreedyAlgorithm.h"
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <limits>
#include <fstream>
#include <iomanip>

EvolutionaryAlgorithm::EvolutionaryAlgorithm(const ProblemInstance& inst, Evaluator& evaluator, const Config& config)
    : instance(inst), evaluator_(evaluator), config_(config) {
    if (config_.seed >= 0)
        random.seed(static_cast<unsigned>(config_.seed));
    else {
        std::random_device rd;
        random.seed(rd());
    }
    bestEver = Individual({}, std::numeric_limits<int>::max());
}

Individual EvolutionaryAlgorithm::run() {
    history_.clear();
    population = initializePopulation(config_.popSize);
    evaluateAll(population);
    for (const auto& individual : population) updateBest(individual);
    history_.push_back(computeStatistics(0));

    std::cout << "Generation 0 | best = " << history_[0].best
              << " Average = "             << history_[0].avg
              << " Worst = "              << history_[0].worst << std::endl;

    for (int generation = 1; generation <= config_.generations; ++generation) {
        std::vector<Individual> newPopulation = elite(config_.eliteCount);

        while (static_cast<int>(newPopulation.size()) < config_.popSize+config_.eliteCount) {
            const Individual& p1 = tournament();
            const Individual& p2 = tournament();
            Individual offspring = applyCrossover(p1, p2);
            offspring = applyMutation(std::move(offspring));
            evaluator_.evaluate(offspring);
            updateBest(offspring);
            newPopulation.push_back(std::move(offspring));
        }

        std::sort(newPopulation.begin(), newPopulation.end());
        newPopulation.resize(config_.popSize);
        population = std::move(newPopulation);

        auto stats = computeStatistics(generation);
        history_.push_back(stats);

        if (generation % 10 == 0 || generation == config_.generations) {
            std::cout << "Generation "           << generation
                      << " | Best = "            << stats.best
                      << " Average = "           << stats.avg
                      << " Worst = "             << stats.worst
                      << " Global best = "       << bestEver.fitness << std::endl;
        }
    }
    saveLogs();
    return bestEver;
}

const std::vector<EvolutionaryAlgorithm::GenerationStatistics>& EvolutionaryAlgorithm::history() const {return history_;}

Individual EvolutionaryAlgorithm::createRandomIndividual() const {
    std::vector<int> schedule(instance.numTasks);
    std::iota(schedule.begin(), schedule.end(), 0);
    std::shuffle(schedule.begin(), schedule.end(), random);
    return Individual(schedule, -1);
}

std::vector<Individual> EvolutionaryAlgorithm::initializePopulation(int size) {
    std::vector<Individual> pop;
    pop.reserve(size);

    if (config_.initType == "greedy") {
        GreedyAlgorithm ga(instance,evaluator_);
        for (int i = 0; i < instance.numTasks && static_cast<int>(pop.size()) < size; ++i) {
            pop.push_back(ga.solve(i));
        }
    }

    while (static_cast<int>(pop.size()) < size)
        pop.push_back(createRandomIndividual());

    return pop;
}

void EvolutionaryAlgorithm::evaluateAll(std::vector<Individual>& pop) {
    for (auto& individual : pop) {
        if (!individual.hasFitness())
            evaluator_.evaluate(individual);
    }
}

const Individual& EvolutionaryAlgorithm::tournament() const {
    std::uniform_int_distribution<int> distribution(0, static_cast<int>(population.size() - 1));
    int best = distribution(random);
    for (int i = 1; i < config_.tournamentSize; ++i) {
        int index =distribution(random);
        if (population[index].fitness < population[best].fitness) best = index;
    }
    return population[best];
}

std::vector<Individual> EvolutionaryAlgorithm::elite(int count) const {
    if (count <= 0) return {};
    count = std::min(count, static_cast<int>(population.size()));
    std::vector<Individual> sorted = population;
    std::sort(sorted.begin(), sorted.end());
    return {sorted.begin(), sorted.begin() + count};
}

std::pair<int,int> EvolutionaryAlgorithm::randomSegment(int J, std::mt19937& rng) {
    std::uniform_int_distribution distribution(0, J-1);
    int i = distribution(rng), j = distribution(rng);
    if (i > j) std::swap(i, j);
    if (i == j) {
        if (j < J-1)
            ++j;
        else
            --i;
    }
    return {i,j};
}

Individual EvolutionaryAlgorithm::ox(const Individual& p1, const Individual& p2) const {
    const int J =static_cast<int>(p1.schedule.size());
    auto[i,j] = randomSegment(J, random);

    std::vector child(J,-1);
    std::vector used(J, false);

    for (int k = i; k <= j; ++k) {
    child[k] =p1.schedule[k];
    used[p1.schedule[k]] = true;
    }

    int position = (j+1) % J;
    int source =(j+1) % J;
    int filled = 0;
    int toFill =J - (j-i + 1);

    while (filled < toFill) {
        int gene = p2.schedule[source];
        if (!used[gene]) {
            while (child[position] != -1) position = (position+1) % J;
            child[position] = gene;
            used[gene] = true;
            ++filled;
        }
        source = (source + 1) % J;
    }
    return Individual(child, -1);
}

Individual EvolutionaryAlgorithm::pmx(const Individual& p1, const Individual& p2) const {
    const int J = static_cast<int>(p1.schedule.size());
    auto[i,j] = randomSegment(J, random);

    std::vector child(J,-1);
    for (int k = i; k <= j; ++k)
        child[k] = p1.schedule[k];


    std::unordered_map<int,int> mapping;
    for (int k = i; k <= j; ++k)
        mapping[p2.schedule[k]] = p1.schedule[k];

    for (int k = 0; k < J; ++k) {
        if (k >= i && k <= j) continue;
        int val = p2.schedule[k];
        while (mapping.count(val)) val = mapping[val];
        child[k] = val;
    }
    return Individual(child, -1);
}

Individual EvolutionaryAlgorithm::applyCrossover(const Individual& p1, const Individual& p2) {
    std::uniform_real_distribution coin(0.0, 1.0);
    if (coin(random) >= config_.px) {
        return (coin(random) < 0.5) ? p1 : p2;
    }
    if (config_.crossoverType == "pmx") return pmx(p1, p2);
    return ox(p1, p2);
}

Individual EvolutionaryAlgorithm::swapMutation(Individual ind) const {
    const int J = static_cast<int>(ind.schedule.size());
    std::uniform_int_distribution dist(0, J-1);
    int i = dist(random);
    int j;
    do { j = dist(random); } while (j == i);
    std::swap(ind.schedule[i], ind.schedule[j]);
    ind.fitness = -1;
    return ind;
}

Individual EvolutionaryAlgorithm::inversionMutation(Individual ind) const {
    const int J = static_cast<int>(ind.schedule.size());
    std::uniform_int_distribution dist(0, J-1);
    int i = dist(random);
    int j = dist(random);
    if (i > j) std::swap(i, j);
    if (i == j) { if (j < J-1) ++j; else --i; }
    std::reverse(ind.schedule.begin() + i, ind.schedule.begin() + j + 1);
    ind.fitness = -1;
    return ind;
}


Individual EvolutionaryAlgorithm::applyMutation(Individual ind) const {
    std::uniform_real_distribution coin(0.0, 1.0);
    if (coin(random) >= config_.pm) return ind;
    if (config_.mutationType == "swap") return swapMutation(std::move(ind));
    return inversionMutation(std::move(ind));
}


EvolutionaryAlgorithm::GenerationStatistics
EvolutionaryAlgorithm::computeStatistics(int gen) const {
    int    best  = std::numeric_limits<int>::max();
    int    worst = std::numeric_limits<int>::min();
    double sum   = 0.0;
    for (const auto& ind : population) {
        if (ind.fitness < best)  best  = ind.fitness;
        if (ind.fitness > worst) worst = ind.fitness;
        sum += ind.fitness;
    }
    return { gen, best, sum / static_cast<double>(population.size()), worst };
}

void EvolutionaryAlgorithm::updateBest(const Individual& ind) {
    if (ind.fitness < bestEver.fitness)
        bestEver = ind;
}

void EvolutionaryAlgorithm::saveLogs() const {
    if (config_.logFile.empty()) return;
    std::ofstream logFile(config_.logFile);

    if (!logFile.is_open()) return;
    logFile << "generation;best;avg;worse\n";
    for (const auto& s : history_)
        logFile << s.generation << ";"
                << s.best       << ";"
                << std::fixed   << std::setprecision(2) << s.avg << ";"
                << s.worst      << std::endl;
    std::cout << "CSV saved here " << config_.logFile << std::endl;
}
