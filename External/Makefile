# Make the external dependencies.
# tclap needs to be unpacked
# ITK needs to be unpacked and build.
#  make itk
# will unpack ITK archive and launch ccmake in a build directory. This requires
# manual intervention to continue.
CMAKE = ccmake
CMAKE_FLAGS = -DCMAKE_CXX_FLAGS=-std=c++11

TAR = tar
TAR_EXTRACT_FLAGS = -xzf

TCLAP_SRC_DIR = tclap-1.2.1
TCLAP_ARCHIVE = ${TCLAP_SRC_DIR}.tar.gz

ITK_SRC_DIR = InsightToolkit-4.8.0
ITK_ARCHIVE = ${ITK_SRC_DIR}.tar.gz
ITK_BUILD_DIR = ITK-build

all: tclap itk

tclap: ${TCLAP_ARCHIVE}
	${TAR} ${TAR_EXTRACT_FLAGS} $^ 


itk-src: ${ITK_ARCHIVE}
	${TAR} ${TAR_EXTRACT_FLAGS} $^

itk: itk-src
	mkdir ${ITK_BUILD_DIR}
	cd ${ITK_BUILD_DIR}
	${CMAKE} ${CMAKE_FLAGS} ../${ITK_SRC_DIR}

clean:
	rm -rf ${ITK_BUILD_DIR}
	rm -rf ${ITK_SRC_DIR}
	rm -rf ${TCLAP_SRC_DIR}
