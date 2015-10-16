# emphysema-estimation
Tools for analysing the extent of emphysema in CT scans of the lungs.

## Build instructions
Clone the repository.
First build the external dependencies.

      cd External
      make all

This will launch ccmake in a build directory for ITK. Configure, generate, make and wait.

Now build the project

    mkdir ../Build
    cd ../Build
    ccmake -DCMAKE_CXX_FLAGS=-std=c++11 ../

Configure, generate, make and wait a little bit.
