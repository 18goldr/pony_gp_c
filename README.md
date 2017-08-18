`pony_gp_c` is an implementation of Genetic Programming(GP), see e.g. 
<http://geneticprogramming.com>. The purpose of `pony_gp_c` is to describe how 
the GP algorithm works and to compatible with emscripten. The intended use is for 
teaching. The aim is to allow the developer to quickly start using and developing. 
The design is supposed to be simple, self contained and use core C libraries. The original
project, written in python, can be found here: https://github.com/flexgp/pony_gp

# Run

Find an equation for the input given an output.
Example output:
```
Reading: fitness_cases.csv headers: ['a', 'b', 'y'] exemplars:121
GP settings:
{'arities': {'+': 2, '*': 2, '/': 2, '-': 2, 'a': 0, 'b': 0, '0.0': 0, '1.0': 0}, 'constants': [0.0, 1.0], 'population_size': 4, 'max_depth': 5, 'elite_size': 2, 'generations': 2, 'tournament_size': 3, 'seed': 0, 'crossover_probability': 0.8, 'mutation_probability': 0.2, 'fitness_cases': [[-3.0, 4.0], [-2.0, 3.0], [-4.0, 3.0], [-5.0, -3.0], [5.0, -3.0], [0.0, -1.0], [2.0, 0.0], [-1.0, 0.0], [-2.0, -3.0], [-4.0, -2.0], [-1.0, -2.0], [5.0, 1.0], [-5.0, -1.0], [-1.0, 3.0], [4.0, 5.0], [-2.0, 1.0], [3.0, 1.0], [-3.0, 0.0], [-1.0, -4.0], [0.0, 3.0], [3.0, -3.0], [0.0, 1.0], [5.0, -2.0], [2.0, 1.0], [1.0, 3.0], [4.0, 4.0], [0.0, -4.0], [-1.0, 1.0], [-4.0, 4.0], [-5.0, 4.0], [-2.0, 0.0], [-4.0, 1.0], [-3.0, 3.0], [2.0, 5.0], [-2.0, -4.0], [2.0, -2.0], [0.0, 4.0], [0.0, -5.0], [1.0, 4.0], [5.0, 0.0], [-5.0, 5.0], [4.0, 3.0], [5.0, 2.0], [3.0, 2.0], [2.0, -1.0], [-5.0, 2.0], [-3.0, -2.0], [2.0, 2.0], [4.0, -5.0], [3.0, 4.0], [-1.0, 2.0], [-4.0, -5.0], [-5.0, -4.0], [3.0, 0.0], [-2.0, -5.0], [-3.0, -1.0], [5.0, 5.0], [-2.0, 2.0], [4.0, 1.0], [-5.0, -5.0], [4.0, -2.0], [-3.0, -4.0], [-4.0, -1.0], [1.0, 2.0], [-3.0, 2.0], [-5.0, 3.0], [4.0, 0.0], [3.0, -1.0], [-3.0, 1.0], [-3.0, 5.0], [1.0, -4.0], [2.0, 3.0], [2.0, -3.0], [1.0, -3.0], [5.0, -4.0], [1.0, 5.0], [-2.0, 4.0], [5.0, -5.0], [-5.0, 0.0], [2.0, -5.0], [1.0, -2.0], [1.0, 1.0], [4.0, -4.0], [-1.0, -5.0]], 'test_train_split': 0.7, 'config': 'configs.ini', 'verbose': None, 'symbols': {'arities': {'+': 2, '*': 2, '/': 2, '-': 2, 'a': 0, 'b': 0, '0.0': 0, '1.0': 0}, 'terminals': ['a', 'b', '0.0', '1.0'], 'functions': ['+', '*', '/', '-']}, 'targets': [25.0, 13.0, 25.0, 34.0, 34.0, 1.0, 4.0, 1.0, 13.0, 20.0, 5.0, 26.0, 26.0, 10.0, 41.0, 5.0, 10.0, 9.0, 17.0, 9.0, 18.0, 1.0, 29.0, 5.0, 10.0, 32.0, 16.0, 2.0, 32.0, 41.0, 4.0, 17.0, 18.0, 29.0, 20.0, 8.0, 16.0, 25.0, 17.0, 25.0, 50.0, 25.0, 29.0, 13.0, 5.0, 29.0, 13.0, 8.0, 41.0, 25.0, 5.0, 41.0, 41.0, 9.0, 29.0, 10.0, 50.0, 8.0, 17.0, 50.0, 20.0, 25.0, 17.0, 5.0, 13.0, 34.0, 16.0, 10.0, 10.0, 34.0, 17.0, 13.0, 13.0, 10.0, 41.0, 26.0, 20.0, 50.0, 25.0, 29.0, 5.0, 2.0, 32.0, 26.0]}
Generation:0 Duration: 0.0016 fit_ave:-572.76+-25.137 size_ave:2.00+-1.000 depth_ave:0.50+-0.500 max_size:3 max_depth:1 max_fit:-530.166667 best_solution:{'genome': ['1.0'], 'fitness': -530.1666666666666}
Generation:1 Duration: 0.0035 fit_ave:-530.17+-0.000 size_ave:1.00+-0.000 depth_ave:0.00+-0.000 max_size:1 max_depth:0 max_fit:-530.166667 best_solution:{'genome': ['1.0'], 'fitness': -530.1666666666666}
Best solution on train data:{'genome': ['1.0'], 'fitness': -530.1666666666666}
Best solution on test data:{'genome': ['1.0'], 'fitness': -487.1081081081081}
```

If you wish to,
change the paramaters from the `configs.ini` file to your desired
paramaters or allow it to remain at its default values.

The input with their respective output is in the file `data/fitness_case.csv`. The
exemplars are generated from `y = a^2 + b^2` from range `[-5,5]`


## Requirements

ANSI C and CMake


## Parameters
```
Parameters:
          POPULATION_SIZE: Population size is the number of individual solutions
                MAX_DEPTH: Max depth of tree. Partly determines the search space
			   of the solutions
	       ELITE_SIZE: Elite size is the number of best individual solutions
			   that are preserved between generations
	      GENERATIONS: Number of generations. The number of iterations of the
			   search loop
          TOURNAMENT_SIZE: Tournament size. The number of individual solutions
			   that are compared when determining which solutions are
			   inserted into the next generation(iteration) of the
			   search loop
		     SEED: Random seed. For replication of runs of the EA. The
			   search is stochastic and and replication of the
			   results are guaranteed the random seed
    CROSSOVER_PROBABILITY: Crossover probability, [0.0,1.0]. The probability of
                           two individual solutions to be varied by the crossover
                           operator
     MUTATION_PROBABILITY: Mutation probability, [0.0, 1.0]. The probability of
                           an individual solutions to be varied by the mutation
                           operator
	 TEST_TRAIN_SPLIT: Test-train data split, [0.0,1.0]. The ratio of fitness
                           cases used for training individual solutions
	          VERBOSE: Verbose printing.
```

## Output
Runs for `generations`

## Output
Runs for `generations`

### Individual Statistics

`Initial individual nr`:individual number nodes: amount of nodes or
different symbols in the individual, `max_depth`: max depth of
individual(refer to usage): individual generated

### Generation Statistics
`Generation`:generation number, `duration`:evaluation time, `fit_ave`:average fitness of the generation, `size_ave`:average number of nodes in the genearation amongst all solutions, `depth_ave`:average max_tree depth, max_size`: maximum number of nodes, `max_depth`: maximum depth, `max_fit`: maximum fitnessm `best_solution`:{`'genome'`: individual formula/tree, `'fitness'`: fitness of genome}

### Best Solution Statistics
```
Best solution on train data:{'genome': individual formula/tree, 'fitness': fitness of genome}
Best solution on test data:{'genome':individual formula/tree, 'fitness':fitness of genome}
```
