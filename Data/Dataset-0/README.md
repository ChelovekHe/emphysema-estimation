# Dataset-0 #
10 scans selected at random from the 20 first visit scans in `copd_label_data`
Selection is based on the ordering in Order.txt

## Features ##
All features are calculated at scales {0.6 sqrt(2)^i}_{i=0,1,...6} mm.

* Gaussian blur
* Gradient magnitude
* Eigenvalues of the Hessian ordered by magnitude
* Laplacian of Gauss (Sum of eigenvalues)
* Gaussian curvature (Product of eigenvalues)
* Frobenius norm of the Hessian ( Euclidean 2-norm of eigenvalue vector )

