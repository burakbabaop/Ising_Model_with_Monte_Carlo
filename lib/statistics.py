import numpy as np
from typing import Callable
from multiprocessing import Pool
from functools import partial
import matplotlib.pyplot as plt


def calculate_magnetization(replica: np.ndarray) -> np.ndarray:
    """Calculates the magnetization for each configuration in a replica

    Args:
        replica (np.ndarray): markov chain of spin grids

    Returns:
        np.ndarray: magnetization for each step in the chain
    """
    magnetizations = np.sum(replica, 1)
    return 2 * magnetizations - len(replica[0])*np.ones_like(magnetizations)


def calculate_energy(replica: np.ndarray, n: int, d: int, b: float,
                     beta: float) -> Callable[[np.ndarray], np.ndarray]:
    """Calculate energy of each configuration in one markov chain

    Args:
        replica (np.ndarray): markov cahin of spin grids
        n (int): number of points at each dimension
        d (int): number of dimensions
        b (float): b parameter in hamiltonian
        beta (float): beta parameter of the hamiltonian

    Returns:
        Callable[[np.ndarray], np.ndarray]: inner 
        function which takes a replica and calculates energy for each step in the chain
    """
    r = len(replica)
    grids = 2*np.reshape(replica, (r, ) + (n, )*d) - np.ones((r, ) + (n, )*d)
    h_b = b*calculate_magnetization(replica)
    neighbour = np.zeros_like(grids)
    for k in range(d):
        neighbour += np.roll(grids, 1, axis=k+1)
    interaction = np.sum(grids*neighbour, tuple(range(1, d+1)))
    h_0 = beta*interaction
    return (-h_0-h_b)


def predicted_observable_replica(N_thermal: int, replica: np.ndarray,
                                 calculate_observable: Callable[[np.ndarray], np.ndarray]) -> float:
    """Calculates the mean observable in a for a single replica

    Args:
        N_thermal (int): _description_
        replica (np.ndarray): _description_
        calculate_observable (Callable[[np.ndarray], np.ndarray]): _description_

    Returns:
        float: _description_
    """
    observables = np.array(calculate_observable(replica[N_thermal:]))
    mean_observable = np.mean(observables)
    return mean_observable


def statistics_observable(replicas: np.ndarray, N_thermal: int,
                          calculate_observable: Callable[[np.ndarray], np.ndarray]) -> float:
    """Calculates the mean and error of the mean of the observable of all replicas


    Args:
        replicas (np.ndarray): Array (with length R) containing Arrays (length N_config)
                               containing the configuration grids
        N_thermal (int): Thermalization time
        calculate_observable (Callable[[np.ndarray], np.ndarray]): 

    Returns:
        float: _description_
    """
    with Pool() as p:
        observables = np.array(p.map(calculate_observable, replicas[:, N_thermal:]))
        mean_observables = np.array(p.map(np.mean, observables))
    mean_mean = np.mean(mean_observables)
    err_mean = np.std(mean_observables)/(len(replicas)-1)
    return mean_mean, err_mean


if __name__ == "__main__":
    replica_number = 5
    configuration_number = 5
    N = 3
    D = 2
    B = 1
    BETA = 1
    total_grid_points = N**D
    rng = np.random.default_rng(12345)
    replicas = rng.integers(low=0, high=2, size=(replica_number, configuration_number, total_grid_points))
    _replica = replicas[0]

    test_energy = calculate_energy(_replica, N, D, B, BETA)
    test_magnetization = calculate_magnetization(_replica)

    print("first configuration in first markov chain: \n", 2*np.reshape(_replica[0], (N, )*D) - np.ones((N, )*D))
    print("energies of first markov chain:", test_energy)
    print("magnetizations of first markov chain: ", test_magnetization)

    mean_magnetization = predicted_observable_replica(0, _replica, calculate_magnetization)
    mean_energy = predicted_observable_replica(0, _replica, partial(calculate_energy, n=N, d=D, b=B, beta=BETA))

    print("mean magnetization in replica 0:", mean_magnetization)
    print("mean energy in replica 0:", mean_energy)

    with Pool() as p:
        energies = np.array(p.map(partial(calculate_energy, n=N, d=D, b=B, beta=BETA), replicas))
        print("mean energies in each iteration of each chain: ")
        print(energies)

    for energy in energies:
        plt.plot(energy)
    plt.show()

    exp_m, err_m = statistics_observable(replicas, 0, calculate_magnetization)
    print(rf"Calaculated Magnetization: {exp_m} \pm {err_m}")

    exp_e, err_e = statistics_observable(replicas, 0, partial(calculate_energy, n=N, d=D, b=B, beta=BETA))
    print(rf"Calculated Energy: {exp_e} \pm {err_e}")
