
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
Generation: 0, Duration: ~2.0000, fit ave: -9060975.07+/-1982762271.224, size ave: 13.91+/-192.000 depth ave: 1.93+/-18.739, max size: 63, max depth: 5, max fit: -111.446281, best solution: Genome: {'+', '-', '-', '+', '1', '0', '-', 'a', '0', '/', '-', '0', 'a', '/', '1', 'a', '*', '+', '/', '0', '0', '*', 'b', 'a', '*', '-', 'a', '1', '/', '0', '0'}, Fitness: -111.4463
Generation: 1, Duration: ~3.0000, fit ave: -358.91+/-2593.833, size ave: 13.21+/-163.385 depth ave: 2.21+/-18.181, max size: 63, max depth: 5, max fit: -0.694215, best solution: Genome: {'+', '-', '*', '+', '1', 'a', '*', '1', 'a', '+', '+', 'a', '0', '-', '0', 'a', '-', '+', '-', 'a', '1', '*', 'b', 'b', '+', '+', 'a', 'a', '-', 'b', 'b'}, Fitness: -0.6942

Best solution on the training data: Genome: {'+', '-', '*', '+', '1', 'a', '*', '1', 'a', '+', '+', 'a', '0', '-', '0', 'a', '-', '+', '-', 'a', '1', '*', 'b', 'b', '+', '+', 'a', 'a', '-', 'b', 'b'}, Fitness: -0.6942
Best solution on the test data: Genome: {'+', '-', '*', '+', '1', 'a', '*', '1', 'a', '+', '+', 'a', '0', '-', '0', 'a', '-', '+', '-', 'a', '1', '*', 'b', 'b', '+', '+', 'a', 'a', '-', 'b', 'b'}, Fitness: -0.6942
```

If you wish to,
change the parameters from the `configs.ini` file to your desired
parameters or allow it to remain at its default values.

The input with their respective output is in the file `data/fitness_case.csv`. The
exemplars are generated from `y = a^2 + b^2` from range `[-5,5]`

To implement a system-dependant time function, modify the function `get_time` in `misc_util.c`.

## Requirements

C99 and CMake

## Usage
```
usage: ./pony_gp    [-p POPULATION_SIZE] [-m MAX_DEPTH] [-e ELITE_SIZE]
                    [-g GENERATIONS] [-ts TOURNAMENT_SIZE] [-s SEED]
                    [-cp CROSSOVER_PROBABILITY] [-mp MUTATION_PROBABILITY]
                    [-tts TEST_TRAIN_SPLIT] [-v VERBOSE] [-config CONFIG]
                    [-fc FITNESS_CASES]


Required arguments:
  -config CONFIG            Config filename (INI format). Overridden by CLI-arguments.
  -fc FITNESS_CASES         Fitness cases filename. The exemplars of input and the
                            corresponding output used to train and test individual
                            solutions.

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
`Generation`: generation number, `duration`: evaluation time, `fit_ave`: average fitness of the generation, `size_ave`: average number of nodes in the generation amongst all solutions, `depth_ave`: average max_tree depth,`max_size`: maximum number of nodes, `max_depth`: maximum depth, `max_fit`: maximum fitness `best_solution`: {`'genome'`: individual formula/tree, `'fitness'`: fitness of genome}


### Best Solution Statistics
```
Best solution on train data:{'genome': individual formula/tree, 'fitness': fitness of genome}
Best solution on test data:{'genome':individual formula/tree, 'fitness':fitness of genome}
```

# Test
Run. Generator type can be anything.
```
cmake -G <Generator type>
make
./pony_gp
```
For information on the types of generators, run `cmake --help`

