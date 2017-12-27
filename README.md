`pony_gp_c` is an implementation of Genetic Programming(GP), 
(see <http://geneticprogramming.com>). The purpose of `pony_gp_c` is to describe how 
the GP algorithm works and to be compatible with emscripten. The intended use is for 
teaching. The aim is to allow the developer to quickly start using and developing. 
The design is supposed to be simple, self contained and use core C libraries. The original
project, written in python, can be found here: https://github.com/flexgp/pony_gp

# Run

Find an equation for the input given an output.
Example output:
```
Generation: 0, Duration: 0.0000, fit ave: -559.71+/-138.623, size ave: 11.60+/-10.846 depth ave: 2.10+/-1.513, max size: 31, max depth: 4, max fit:-164.976190 best solution: Genome: *++*11*11+*bb*10///a0/a0/*a0-ba, Fitness: -164.9762
Generation: 1, Duration: 0.0000, fit ave: -760.20+/-731.329, size ave: 6.80+/-10.750 depth ave: 1.00+/-1.612, max size: 31, max depth: 4, max fit:-164.976190 best solution: Genome: *++*11*11+*bb*10///a0/a01, Fitness: -164.9762
Best solution on train data: Genome: *++*11*11+*bb*10///a0/a01, Fitness: -164.9762

Best solution of test data: Genome: *++*11*11+*bb*10///a0/a01, Fitness: -164.9762
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

### Individual Statistics

`Initial tree nr`: number `nodes`: number of nodes in tree `max_depth`: max tree depth `tree`: symbols in tree

### Generation Statistics
<<<<<<< HEAD
`Generation`: generation number, `duration`: evaluation time, `fit_ave`: average fitness of the generation, `size_ave`: average number of nodes in the generation amongst all solutions, `depth_ave`: average max_tree depth,`max_size`: maximum number of nodes, `max_depth`: maximum depth, `max_fit`: maximum fitness `best_solution`: {`'genome'`: individual formula/tree, `'fitness'`: fitness of genome}
=======
`Generation`: generation number, `duration`: evaluation time, `fit_ave`: average fitness of the generation, `size_ave`: average number of nodes in the generation amongst all solutions, `depth_ave`: average max_tree depth,`max_size`: maximum number of nodes, `max_depth`: maximum depth, `max_fit`: maximum fitnessm `best_solution`: {`'genome'`: individual formula/tree, `'fitness'`: fitness of genome}
>>>>>>> 9c56a56480caebc127ca9217bba103eb2a5a1d8f

### Best Solution Statistics
```
Best solution on train data:{'genome': individual formula/tree, 'fitness': fitness of genome}
Best solution on test data:{'genome':individual formula/tree, 'fitness':fitness of genome}
```

# Test
Run. Generator type can be anything.
```
cmake -G <Generator type> 
./pony_gpExe
```
