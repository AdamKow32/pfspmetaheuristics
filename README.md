## PFSP Metaheuristics Solver

A C++17 implementation of metaheuristic algorithms for the Permutation Flowshop Scheduling Problem (PFSP), a classic NP-hard combinatorial optimization problem widely used as a benchmark in scheduling research.

## Problem Definition

Given J jobs and M machines, find permutation (`x = (x₁, ..., xⱼ)`) that minimizes the total flowtime (sum of completion times of all jobs on last machine):

```
f(x) = Σ C(xⱼ, M)   for j = 1..J
```

The completion time `C(xⱼ, m)` is computed recursively:
 
```
C(x₁, m₁) = p(x₁, m₁)
C(x₁, m)  = C(x₁, m-1) + p(x₁, m)
C(xⱼ, m₁) = C(xⱼ₋₁, m₁) + p(xⱼ, m₁)
C(xⱼ, m)  = max{ C(xⱼ₋₁, m), C(xⱼ, m-1) } + p(xⱼ, m)
```

where `p(j, m)` is the processing time of job `j` on machine `m`.

## Algorithms
 
| Algorithm | Type | Description |
|-----------|------|-------------|
| **Random Search** | Baseline | Randomly generates and evaluates permutations |
| **Greedy** | Constructive heuristic | Builds solution step-by-step, always choosing job with minimum `C(job, M)` |
| **Evolutionary Algorithm** | Metaheuristic | Population-based search with tournament selection, OX/PMX crossover, swap/inversion mutation and elitism |
| **Simulated Annealing** | Metaheuristic | Single-solution local search with probabilistic acceptance of worse solutions |

## Getting started

### 1. Clone the repository
 
```bash
git clone https://github.com/AdamKow32/pfspmetaheuristics
cd pfspmetaheuristics
```

### 2. Download benchmark instances
 
Download `.fsp` files from:
> https://github.com/chneau/gotaillard/tree/master/pfsp/instances

Click on each file → click **Raw** → save to the `data/` folder in the project.

### 3. Build
 
**Linux / macOS:**
```bash
make
```
 
**Windows (CLion):**
 
Open the project folder in CLion, it will detect `CMakeLists.txt` automatically and load the project.

 ### 4. Run
 
**Linux / macOS:**
```bash
./pfsp data/tai20_5_0.fsp
```
 
**Windows (CLion):**
 
1. Click the run configuration dropdown (top right) and then **Edit Configurations...**
2. Set **Program arguments** to the instance you want to run, e.g.:
   ```
   data/tai20_5_0.fsp
   ```
3. Set **Working directory** to the project root folder, e.g.:
   ```
   C:\Users\YourName\CLionProjects\pfsp
   ```
4. Click **OK** and then click **Run**

**Requirements:** C++17 or later, g++ or clang++

## Benchmark Instances
 
Taillard PFSP benchmark instances, available at:
> https://github.com/chneau/gotaillard/tree/master/pfsp/instances
> License: MIT

## Configuration
 
All parameters are set in `main.cpp`

## License

MIT License
