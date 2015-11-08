# Make the external dependencies.
# tclap needs to be unpacked
# ITK needs to be unpacked and build.
#  make itk
# will unpack ITK archive and launch ccmake in a build directory. This requires
# manual intervention to continue.
CMAKE = ccmake
CMAKE_FLAGS = -DCMAKE_CXX_FLAGS=-std=c++11

TAR = tar
TAR_GUNZIP_FLAGS = -xzf
TAR_BUNZIP_FLAGS = -xjf

UNZIP = unzip

TCLAP_SRC_DIR = tclap-1.2.1
TCLAP_ARCHIVE = ${TCLAP_SRC_DIR}.tar.gz

EIGEN_SRC_DIR = eigen-eigen-c58038c56923
EIGEN_ARCHIVE = ${EIGEN_SRC_DIR}.tar.bz2

GLOG_SRC_DIR = glog-0.3.4
GLOG_ARCHIVE = ${GLOG_SRC_DIR}.tar.gz


CERES_SRC_DIR = ceres-solver-1.11.0
CERES_ARCHIVE = ${CERES_SRC_DIR}.tar.gz
CERES_BUILD_DIR = CERES-build
CERES_CMAKE_FLAGS = -DEIGEN_INCLUDE_DIR=../eigen-eigen-c58038c56923

ITK_SRC_DIR = InsightToolkit-4.8.0
ITK_ARCHIVE = ${ITK_SRC_DIR}.tar.gz
ITK_BUILD_DIR = ITK-build

FLANN_SRC_DIR = flann-1.8.4-src
FLANN_ARCHIVE = ${FLANN_SRC_DIR}.zip
FLANN_BUILD_DIR = FLANN-build

GTEST_SRC_DIR = googletest-release-1.7.0
GTEST_ARCHIVE = ${GTEST_SRC_DIR}.tar.gz
GTEST_BUILD_DIR = GTEST-build


all: tclap itk flann eigen ceres

tclap: ${TCLAP_ARCHIVE}
	${TAR} ${TAR_GUNZIP_FLAGS} $^ 


eigen: ${EIGEN_ARCHIVE}
	${TAR} ${TAR_BUNZIP_FLAGS} $^

glog-src : ${GLOG_ARCHIVE}
	${TAR} ${TAR_GUNZIP_FLAGS} $^ 

ceres-src: ${CERES_ARCHIVE}
	${TAR} ${TAR_GUNZIP_FLAGS} $^ 

ceres: ceres-src
	mkdir -p ${CERES_BUILD_DIR}
	cd ${CERES_BUILD_DIR};\
	${CMAKE} ${CMAKE_FLAGS} ${CERES_CMAKE_FLAGS} ../${CERES_SRC_DIR}
	${MAKE} -C ${CERES_BUILD_DIR}


itk-src: ${ITK_ARCHIVE}
	${TAR} ${TAR_GUNZIP_FLAGS} $^

itk: itk-src
	mkdir -p ${ITK_BUILD_DIR}
	cd ${ITK_BUILD_DIR}; \
	${CMAKE} ${CMAKE_FLAGS} ../${ITK_SRC_DIR}


flann-src: ${FLANN_ARCHIVE}
	${UNZIP} $^

flann: flann-src
	mkdir -p ${FLANN_BUILD_DIR}
	cd ${FLANN_BUILD_DIR}; \
	${CMAKE} ${CMAKE_FLAGS} ../${FLANN_SRC_DIR}
	${MAKE} -C ${FLANN_BUILD_DIR}


gtest-src: ${GTEST_ARCHIVE}
	${TAR} ${TAR_GUNZIP_FLAGS} $^

gtest: gtest-src
	mkdir -p ${GTEST_BUILD_DIR}
	cd ${GTEST_BUILD_DIR}; \
	${CMAKE} ${CMAKE_FLAGS} ../${GTEST_SRC_DIR}
	${MAKE} -C ${GTEST_BUILD_DIR}


clean:
	rm -rf ${ITK_BUILD_DIR}
	rm -rf ${ITK_SRC_DIR}
	rm -rf ${TCLAP_SRC_DIR}
