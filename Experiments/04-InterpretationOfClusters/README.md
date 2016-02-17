# Interpretation of clusters #

## Purpose ##
Investigate if clusters have a simple interpretation. We look at three scenarios

* Will two clusters give us interior/border clusters?
* Will six clusters give us lung region clusters?
* Will ten clusters give us scan clusters?

## Method ##
The instances are labelled with

* Interior/Border
* Left lung/Right lung
* Lung region
* Scan id

The instances are clustered using number of clusters according to the scenarios.


For each clustering we calculate the entropy for each of the label dimensions.


Each instance is labelled according to the scenarios. The instances are the and entropy is used as a measure of how much the clustering is correlated with the scenarios.


The clustering is iterated several times.

Make some plots that show the distribution of labels in each cluster.


## Common parameters ##
* Iterations = 100

`KMeansClusterer` implementation parameters
* `iterations` is set to default value (= 11)
* `centers_init` is set to `CENTERS_KMEANSPP`


## Output ##
?

## Data ##
?
