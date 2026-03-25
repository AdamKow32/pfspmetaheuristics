## PFSP metaheuristics project

The PFSP (Permutation Flowshop Scheduling Problem) consists of determining the optimal order of jobs processed on a set of machines in order to minimize the total completion time.

## Problem

PSFP consists of finding a job sequence that minimizes the total completion time of all jobs on the last machine:

f(x) = sum C(x_j, M)  for j = 1..J

where C(x_j, m) is the completion time of job x_j on machine m.

## Progress
So far I've made:
- load .fsp file
- Individual representation
- Objective function f(x)

I still need to implement random search, greedy algorithm, evolutionary algorithm and CSV logging.

## License

MIT License
