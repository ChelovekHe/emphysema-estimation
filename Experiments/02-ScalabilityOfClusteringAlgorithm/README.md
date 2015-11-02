# Scalability of clustering algorithm#
## Purpose ##
Investigate how k-means clustering performs on the image patches. Questions that should be sought answered by the experiment are

* How does the clustering algorithm scale?
  - Influence of k
  - Influence of number of samples


## Method ##
Run the `KMeansClusterer` several times with increasing k and increasing number of instances from Dataset-1. Use both maximal branching factor and fixed branching factor. Measure computation time.

### Common parameters ###
These are used unless specified otherwise.

* Number of samples = [500, 1000, 2500, 5000, 7500, 10000]
* Number of clusters = [4, 8, 16, 32, 64]
* Number of burnin iterations  = 10
* Nuber of measurement iterations = 100

`KMeansClusterer` implementation parameters

* `iterations` is set to default value (= 11)
* `centers_init` is set to `CENTERS_KMEANSPP`

### Experiments ###
* Scalability-1: Set branching equal to number of clusters. 
* Scalability-2: Fix branching at 16. Set Number of clusters = [16, 31, 61].
* Scalability-3: Set branching = number of clusters = 32. Set number of clusters = 2500. Run through cachegrind to profile program execution.

## Output ##
Graphs showing computation time as function of number of clusters, number of samples and branching factor. Cachegrind profile.

## Data ##
Dataset-1. 
