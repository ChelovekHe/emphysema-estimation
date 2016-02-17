# Stability of clustering algorithm #
## Purpose ##
Investigate how k-means clustering performs on the image patches. Questions that should be sought answered by the experiment are

* How does the branching and k parameters influence stability?


## Method ##
Flann has support for automatically choosing the optimal parameters based on a user-defined weighting of build-time, memory-consumption and search-time and a desired search precision, see [Muja and Lowe (2009)][Muja2009]. Taking advantage of the automatic parameter tuning in this work is complicated because of different objectives: The automatic parameter tuning considers several algorithms, the full parameter space and optimizes for precision under time/memory constraints. We need to restrict the parameter space so we can get the number of clusters we are interested in, and we want to optimize for precision and reproducability of cluster centers.


Instead we estimate the stability of the clustering for varying numbers of branching factors and clusters. The Hasudorff distance, with earth movers distance defining the metric space, is used as a measure of stability.


As shown in 02-ScalabilityOfClusteringAlgorithm runtime increases with the branching factor. Experiments in [Muja and Lowe (2009)][Muja2009] indicates that a higher branching factor gives a higher search precision, but little is gained in [Muja and Lowe (2012)][Muja2012] from brancing factors higher than 16 or 32.


One issue to consider is that branching factor restricts the feasible number of clusters.

### Common parameters ###
These are used unless specified otherwise.
* Number of iterations = 100
* Number of samples = 1000
* Nuber of measurement iterations = 100

`KMeansClusterer` implementation parameters
* `iterations` is set to default value (= 11)
* `centers_init` is set to `CENTERS_KMEANSPP`

TODO: Try iterating until convergence.

### Experiments ###
* Stability-1: Branching =  2. Clusters = [2, 4, 8, 16, 32, 64]
* Stability-2: Branching =  4. Clusters = [   4, 7, 16, 31, 64]
* Stability-3: Branching =  8. Clusters = [      8, 15, 29, 64]
* Stability-4: Branching = 16. Clusters = [         16, 31, 61]
* Stability-4: Branching = 32. Clusters = [             32, 63]


## Output ##
One boxplot for eac branching factor, showing the distribution of Hausdorff distances for each k.

## Data ##
Dataset-1: `instances1000.csv`


## References ##
[Muja2009]: http://www.cs.ubc.ca/research/flann/uploads/FLANN/flann_visapp09.pdf "Fast approximate nearest neighbors with automatic algorithm configuration"
[Muja2012]: http://www.cs.ubc.ca/research/flann/uploads/FLANN/binary_matching_crv2012.pdf "Fast Matching of Binary Features"

