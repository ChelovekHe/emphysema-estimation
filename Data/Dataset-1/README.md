# Dataset-1 #
Derived from Dataset-0 using all features at all scales. Uses 10 scans and samples 2000 ROIs from each scan to generate 10000 instances.

Uses the histogram spec from Dataset-3.



## Files ##
* `FileLists/ScanMaskList.csv` : list of scan,mask
* `Histograms/Dataset-3-HistogramSpec.txt` : Histogram specification used to calculate feature histograms
* `ROIs/<scan-name>ROIInfo.txt` : Specification of the extracted ROIs for `<scan-name>`
* `Bags/<scan-name>bag.txt` : The extracted features for `<scan-name>`. One line contains histograms for one ROI.
* `Instances/instances<size>.csv.gz` : Instance matrix with `<size>` instances randomly sampled from all the bags.

## Scripts ##
* `Make-DataSet-1`: Create the bags and instance matrices. 

