
`pony_gp` is an implementation of Genetic Programming (GP -- see <http://geneticprogramming.com>).
The purpose of `pony_gp` is to describe how the GP algorithm works and to be compatible with
emscripten. The intended use is for teaching. The aim is to allow the developer to quickly
start using and developing. The design is supposed to be simple, self contained and use core
C libraries. The original project, written in python, can be found here:
https://github.com/flexgp/pony_gp

# Run

Find an equation that produces the given outputs from the given inputs.
Example output:
```
Reading: ../data/fitness_cases.csv, Headers: {a, b, y}, Number of Exemplars: 121
GP Settings:
[[Population Size: 500, Max Depth: 4, Elite Size: 4, Generations: 2, Tournament Size: 7, Seed: 0.000000, Crossover Probability: 0.800000, Mutation Probability: 0.200000, Verbose: 0, Config: ../data/configs.ini, Functions: {+, *, /, -}, Terminals: {0, 1, a, b}, Arities: {+: 2.000000, *: 2.000000, /: 2.000000, -: 2.000000, 0: 0.000000, 1: 0.000000, a: 0.000000, b: 0.000000}, Fitness Cases: {[-5.000000, -5.000000], [-5.000000, -4.000000], [-5.000000, -3.000000], [-5.000000, -2.000000], [-5.000000, -1.000000], [-5.000000, 0.000000], [-5.000000, 1.000000], [-5.000000, 2.000000], [-5.000000, 3.000000], [-5.000000, 4.000000], [-5.000000, 5.000000], [-4.000000, -5.000000], [-4.000000, -4.000000], [-4.000000, -3.000000], [-4.000000, -2.000000], [-4.000000, -1.000000], [-4.000000, 0.000000], [-4.000000, 1.000000], [-4.000000, 2.000000], [-4.000000, 3.000000], [-4.000000, 4.000000], [-4.000000, 5.000000], [-3.000000, -5.000000], [-3.000000, -4.000000], [-3.000000, -3.000000], [-3.000000, -2.000000], [-3.000000, -1.000000], [-3.000000, 0.000000], [-3.000000, 1.000000], [-3.000000, 2.000000], [-3.000000, 3.000000], [-3.000000, 4.000000], [-3.000000, 5.000000], [-2.000000, -5.000000], [-2.000000, -4.000000], [-2.000000, -3.000000], [-2.000000, -2.000000], [-2.000000, -1.000000], [-2.000000, 0.000000], [-2.000000, 1.000000], [-2.000000, 2.000000], [-2.000000, 3.000000], [-2.000000, 4.000000], [-2.000000, 5.000000], [-1.000000, -5.000000], [-1.000000, -4.000000], [-1.000000, -3.000000], [-1.000000, -2.000000], [-1.000000, -1.000000], [-1.000000, 0.000000], [-1.000000, 1.000000], [-1.000000, 2.000000], [-1.000000, 3.000000], [-1.000000, 4.000000], [-1.000000, 5.000000], [0.000000, -5.000000], [0.000000, -4.000000], [0.000000, -3.000000], [0.000000, -2.000000], [0.000000, -1.000000], [0.000000, 0.000000], [0.000000, 1.000000], [0.000000, 2.000000], [0.000000, 3.000000], [0.000000, 4.000000], [0.000000, 5.000000], [1.000000, -5.000000], [1.000000, -4.000000], [1.000000, -3.000000], [1.000000, -2.000000], [1.000000, -1.000000], [1.000000, 0.000000], [1.000000, 1.000000], [1.000000, 2.000000], [1.000000, 3.000000], [1.000000, 4.000000], [1.000000, 5.000000], [2.000000, -5.000000], [2.000000, -4.000000], [2.000000, -3.000000], [2.000000, -2.000000], [2.000000, -1.000000], [2.000000, 0.000000], [2.000000, 1.000000], [2.000000, 2.000000], [2.000000, 3.000000], [2.000000, 4.000000], [2.000000, 5.000000], [3.000000, -5.000000], [3.000000, -4.000000], [3.000000, -3.000000], [3.000000, -2.000000], [3.000000, -1.000000], [3.000000, 0.000000], [3.000000, 1.000000], [3.000000, 2.000000], [3.000000, 3.000000], [3.000000, 4.000000], [3.000000, 5.000000], [4.000000, -5.000000], [4.000000, -4.000000], [4.000000, -3.000000], [4.000000, -2.000000], [4.000000, -1.000000], [4.000000, 0.000000], [4.000000, 1.000000], [4.000000, 2.000000], [4.000000, 3.000000], [4.000000, 4.000000], [4.000000, 5.000000], [5.000000, -5.000000], [5.000000, -4.000000], [5.000000, -3.000000], [5.000000, -2.000000], [5.000000, -1.000000], [5.000000, 0.000000], [5.000000, 1.000000], [5.000000, 2.000000], [5.000000, 3.000000], [5.000000, 4.000000], [5.000000, 5.000000]}, Targets: {50.000000, 41.000000, 34.000000, 29.000000, 26.000000, 25.000000, 26.000000, 29.000000, 34.000000, 41.000000, 50.000000, 41.000000, 32.000000, 25.000000, 20.000000, 17.000000, 16.000000, 17.000000, 20.000000, 25.000000, 32.000000, 41.000000, 34.000000, 25.000000, 18.000000, 13.000000, 10.000000, 9.000000, 10.000000, 13.000000, 18.000000, 25.000000, 34.000000, 29.000000, 20.000000, 13.000000, 8.000000, 5.000000, 4.000000, 5.000000, 8.000000, 13.000000, 20.000000, 29.000000, 26.000000, 17.000000, 10.000000, 5.000000, 2.000000, 1.000000, 2.000000, 5.000000, 10.000000, 17.000000, 26.000000, 25.000000, 16.000000, 9.000000, 4.000000, 1.000000, 0.000000, 1.000000, 4.000000, 9.000000, 16.000000, 25.000000, 26.000000, 17.000000, 10.000000, 5.000000, 2.000000, 1.000000, 2.000000, 5.000000, 10.000000, 17.000000, 26.000000, 29.000000, 20.000000, 13.000000, 8.000000, 5.000000, 4.000000, 5.000000, 8.000000, 13.000000, 20.000000, 29.000000, 34.000000, 25.000000, 18.000000, 13.000000, 10.000000, 9.000000, 10.000000, 13.000000, 18.000000, 25.000000, 34.000000, 41.000000, 32.000000, 25.000000, 20.000000, 17.000000, 16.000000, 17.000000, 20.000000, 25.000000, 32.000000, 41.000000, 50.000000, 41.000000, 34.000000, 29.000000, 26.000000, 25.000000, 26.000000, 29.000000, 34.000000, 41.000000, 50.000000}]]
Generation: 0, Duration: ~0.0000, fit ave: -1004.11+/-31987.366, size ave: 7.95+/-67.019 depth ave: 1.52+/-10.912, max size: 31, max depth: 4, max fit: -102.904762, best solution: Genome: {'-', '-', '+', '+', 'a', '0', '*', 'b', 'b', '/', '*', 'b', 'a', '/', '0', '0', '*', '-', '-', '0', 'b', '+', 'b', 'a', '-', '/', 'a', '1', '*', 'b', '0'}, Fitness: -102.9048
Generation: 1, Duration: ~2.0000, fit ave: -446.22+/-1379.142, size ave: 9.98+/-72.319 depth ave: 1.98+/-11.031, max size: 31, max depth: 4, max fit: -0.000000, best solution: Genome: {'-', '-', '+', '+', 'a', '0', '*', 'b', 'b', '/', '*', 'b', 'a', '/', '0', '0', '*', '-', '1', '+', 'b', 'a', '-', '/', 'a', '1', '*', 'b', '0'}, Fitness: -0.0000

Best solution on the training data: Genome: {'-', '-', '+', '+', 'a', '0', '*', 'b', 'b', '/', '*', 'b', 'a', '/', '0', '0', '*', '-', '1', '+', 'b', 'a', '-', '/', 'a', '1', '*', 'b', '0'}, Fitness: -0.0000
Best solution on the test data: Genome: {'-', '-', '+', '+', 'a', '0', '*', 'b', 'b', '/', '*', 'b', 'a', '/', '0', '0', '*', '-', '1', '+', 'b', 'a', '-', '/', 'a', '1', '*', 'b', '0'}, Fitness: -0.0000
```

Change the parameters from the `configs.ini` file to your desired
parameters if you wish.

The input(s) with their respective output is in the file `data/fitness_case.csv`. The
exemplars are generated from `y = a^2 + b^2` from range `[-5, 5]`

To implement a system-dependant time function, modify the function `get_time` in `misc_util.c`.

## Requirements

C99 and CMake 3.5+

## Usage
```
usage: ./pony_gp    [-p POPULATION_SIZE] [-m MAX_DEPTH] [-e ELITE_SIZE]
                    [-g GENERATIONS] [-ts TOURNAMENT_SIZE] [-s SEED]
                    [-cp CROSSOVER_PROBABILITY] [-mp MUTATION_PROBABILITY]
                    [-tts TEST_TRAIN_SPLIT] [-v VERBOSE] [-config CONFIG]
                    [-fc FITNESS_CASES]


Required arguments:
  -config CONFIG            Config path (INI format). Overridden by CLI-arguments.
  -fc FITNESS_CASES         Fitness cases path. The exemplars of input and the
                            corresponding output used to train and test individual
                            solutions. Inputs must be in alphabetical order (not case
                            sensitive).

Optional arguments:
  -p POPULATION_SIZE        Population size is the number of individual solutions
  -m MAX_DEPTH              Max depth of tree. Partly determines the search space
                            of the solutions.
  -e ELITE_SIZE             Elite size is the number of best individual solutions
                            that are preserved between generations.
  -g GENERATIONS            Number of generations. The number of iterations of the
                            search loop.
  -ts TOURNAMENT_SIZE       Tournament size. The number of individual solutions
                            that are compared when determining which solutions are
                            inserted into the next generation (iteration) of the
                            search loop.
  -s SEED                   Random seed. For replication of runs of the EA. The
                            search is stochastic and and replication of the
                            results are guaranteed the random seed.
  -cp CROSSOVER_PROBABILITY Crossover probability, [0.0, 1.0]. The probability of
                            two individual solutions to be varied by the crossover
                            operator.
  -mp MUTATION_PROBABILITY  Mutation probability, [0.0, 1.0]. The probability of
                            an individual solutions to be varied by the mutation
                            operator.
  -tts TEST_TRAIN_SPLIT     Test-train data split, [0.0, 1.0]. The ratio of fitness
                            cases used for training individual solutions.
  -v VERBOSE                Set to 1 for verbose printing. Otherwise, 0.
```

## Output
Runs for `generations`

### Individual Statistics

`Initial tree nr`: number `nodes`: number of nodes in tree `max_depth`: max tree depth `tree`: symbols in tree

### Generation Statistics
`Generation`: generation number, `duration`: evaluation time, `fit_ave`: average fitness of the generation, `size_ave`: average number of nodes in the generation amongst all solutions, `depth_ave`: average max_tree depth,`max_size`: maximum number of nodes, `max_depth`: maximum depth, `max_fit`: maximum fitness `best_solution`: `'genome'`: {individual formula/tree}, `'fitness'`: fitness of genome


### Best Solution Statistics
```
Best solution on train data: 'genome': {individual formula/tree}, 'fitness': fitness of genome
Best solution on test data: 'genome': {individual formula/tree}, 'fitness':fitness of genome
```

# Test
Run. Generator type can be anything. Config and fitness case file paths are required.
```
cmake -G <Generator type>
make
./pony_gp -config <path/to/config/file> -fc <path/to/fitness/cases>
```
For information on the types of generators, run `cmake --help`

