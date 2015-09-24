#ifndef __Eigenvalues_h
#define __Eigenvalues_h

#include "itkFixedArray.h"
#include <cmath>

/* Calculates eigenvalues of 3x3 symmetric matrix. 
   source: https://en.wikipedia.org/wiki/Eigenvalue_algorithm#3.C3.973_matrices
   Returns them in order such that |eig3| <= |eig2| <= |eig1|
 */
template< typename RealType >
itk::FixedArray<RealType, 3>
eigenvalues_symmetric3x3(itk::FixedArray<RealType, 6> const &A) {
  itk::FixedArray<RealType, 3> eigenvalues;
  
  RealType A11 = A[0];
  RealType A12 = A[1]; // = A21
  RealType A13 = A[2]; // = A31
  RealType A22 = A[3]; 
  RealType A23 = A[4]; // = A32
  RealType A33 = A[5];

  RealType p = A12*A12 + A13*A13 + A23*A23; 
  if (p == 0) {
    // A is diagonal.
    eigenvalues[0] = A11;
    eigenvalues[1] = A22;
    eigenvalues[2] = A33;
  }
  else {
    RealType q = (A11 + A22 + A33) / 3;
    p = (A11 - q) * (A11 - q) + (A22 - q) * (A22 - q) +
        (A33 - q) * (A33 - q) + 2 * p;
    p = sqrt(p / 6);
    // We need to compute
    // B = (A - Identity * q) / p
    // r = det(B)/2
    RealType B11 = (A11 - q) / p;
    RealType B12 = A12 / p;       // = B21
    RealType B13 = A13 / p;       // = B31
    RealType B22 = (A22 - q) / p;
    RealType B23 = A23 / p;       // = B32
    RealType B33 = (A33 - q) / p;
    RealType r = (B11 * B22 * B33
		+ 2 * B12 * B13 * B23
		- B23*B23 * B11
		- B13*B13 * B22
		- B12*B12 * B33
		) / 2.0;

    // Inexact arithmetic for a symmetric matrix  -1 <= r <= 1
    // but computation error can leave it slightly outside this range.
    RealType phi;
    if (r <= -1) {
      phi = M_PI / 3;
    }
    else if (r >= 1) {
      phi = 0;
    }
    else {
      phi = acos(r) / 3;
    }
    
    // The eigenvalues satisfy eig3 <= eig2 <= eig1
    eigenvalues[0] = q + 2 * p * cos(phi);
    eigenvalues[2] = q + 2 * p * cos(phi + M_PI * (2.0/3.0));
    eigenvalues[1] = 3 * q - eigenvalues[0] - eigenvalues[2];   //  since trace(A) = eig1 + eig2 + eig3
    // Ensure that have |eig3| <= |eig2| <= |eig1|
    if ( std::abs( eigenvalues[0] ) < std::abs( eigenvalues[2] ) ) {
      std::swap( eigenvalues[0], eigenvalues[2] );
    }
  }
  return eigenvalues;
}

#endif
