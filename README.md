# Numerical Simulation of the Ising Model with Monte Carlo
## Introduction
In the repository, Ising model on a D dimensional lattice is simulated. The main challenge of the problem was the exponential scaling of the number of configurations: There are two spin values -1  and 1 associated to each lattice point. For N lattice points, we have 2^N total configurations, which gives approximately 10^30 for 10 * 10 lattice.

 The observation that not every configuration is equally important leads us to just include the configurations which gives the biggest contribution. This is done by the Metropolis Algorithm. We began the Markov Chain with a random initial configuration and each iteration in the chain refers to a Metropolis Iteration. Since the algorithm is probabilistic in nature, we use number of different Markov Chains (replicas) to calculate the statistical error.
 
 This project was done by Burak Varol, Lyding Brumm and Max Heimann. The results and physical discussion can be found in the report pdf.

## Usage
 The code is purely written in C and can be compiled with a Makefile. Be aware that the RANLUX library is present in the ‘lib‘ subdirectory and has to be compiled for the architecture of your machine. The main part of the code can be build with ‘make’. After a successful build the binary ‘main’ and ‘analysation’ will be present in the subdirectory ‘app’. The executable ‘main’ implements the Metropolis algorithm and produces two files storing the energy and magnetisation of each iteration in a new-line seperated format, ready for gnuplot’s plot function. Its parameters are:

    Dimension of the grid (integer),
    Number of Gridpoints per Dimension (integer), 
    β (float),
    b (float),
    Number of iterations (integer),
    seed (integer)
It therefor calculates one replica. Running the main program therefor might look something like 
    
    ‘./app/main 2 100 0.43 0.01 1000 12345’ 
The second executable takes two arguments, the path to a directory containing replica files and the thermalisation time. It calculates the expectation value of the energy and magnetisation as well as their error. Running the analysation program might look something like 
    
    ’./app/analysation ./dataset2/ 300’.

Please be aware that we changed the ranlux library a bit, since ranlux we got the following error (just in macOS):
  
    ./ranlux.h:37:14: error: typedef redefinition with different types ('long' vs 'signed char')
    typedef long int8_t; 

The solution was redefinition of int8_ to another word. We tested the code multiple times in linux and macOS and the errors were solved.





