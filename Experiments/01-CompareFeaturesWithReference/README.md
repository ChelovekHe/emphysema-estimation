# Compare features with reference #
## Purpose ##
Check correctness of feature extraction implementation.

## Method ##
Calculate features at scales {0.6, 1.2} for a single scan. Calculate joint histograms between the calculated feature images and reference images.

## Output ##
One joint histogram image for each pair of feature and reference image.

## How to run it ##
Requires python3 and Rscript.
* Generate feature images with the script `Scripts/GenerateFeatureImages.py`.
* Generate histogram plots with the script `Scripts/GenerateJointHistogramPlots.py`
* Run everything with `Scripts/Run.py`


## Data ##
Note that access to the data is restricted and it should be manually copied from the separate data storage.

### Scans ###
In directory Data/Scans
* `vol1004_20050208-D-1.dcm`

### Lung segmentations ###
In directory Data/Lungs
* `vol1004_20050208-D-1-smooth.dcm`
* `vol1004_20050208-D-1-normal.dcm`

### Reference features ###
In directory Data/Reference
* `scale_0_6_eig1.nii.gz`
* `scale_0_6_eig2.nii.gz`
* `scale_0_6_eig3.nii.gz`
* `scale_0_6_eigen_magnitude.nii.gz`
* `scale_0_6_gaussian_curvature.nii.gz`
* `scale_0_6_laplacian_of_gaussian.nii.gz`
* `scale_0_6_gradient_magnitude.nii.gz`
* `scale_0_6_gaussian.nii.gz`
* `scale_1_2_eig1.nii.gz`
* `scale_1_2_eig2.nii.gz`
* `scale_1_2_eig3.nii.gz`
* `scale_1_2_eigen_magnitude.nii.gz`
* `scale_1_2_gaussian_curvature.nii.gz`
* `scale_1_2_laplacian_of_gaussian.nii.gz`
* `scale_1_2_gradient_magnitude.nii.gz`
* `scale_1_2_gaussian.nii.gz`

### Calculated features ###
In directory `Data/Features`.
