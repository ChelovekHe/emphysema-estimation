#!/usr/bin/python3

# Read a list of (image, mask) pairs.
# DetermineBinWidths for all images
# Use bin widths and (image,mask) pairs to generate histograms for each image

import os, os.path, subprocess, glob, re, sys, csv
from Util import intersperse


def main(argv=None):
    if argv is None:
        argv = sys.argv

    if len(argv) < 5:
        print( "Usage:", argv[0],
               "<image-mask-list> <outdir> <num-bins> <num-samples>" )
        return 1

    progs = {
        'DetermineBinWidths' : '../Build/Statistics/DetermineHistogramBinWidtsFromPopulation',
        'MaskedImageHistogram' : '../Build/Statistics/MaskedImageHistogram',
    }
    
    imageMaskListPath = argv[1]
    outDir = argv[2]
    try:
        numBins = int(argv[3])
        if numBins < 2: raise ValueError()
    except ValueError as e:
        print( "<num-bins> argument must be a positive integer > 1" )
        return 1

    try:
        numSamples = int(argv[4])
        if numSamples < 1: raise ValueError()
    except ValueError as e:
        print( "<num-samples> argument must be a positive integer" )
        return 1
    
    try:
        with open(imageMaskListPath) as f:
            imageMaskList = [(row[0], row[1]) for row in csv.reader(f)]
    except FileNotFoundError as e:
        print( "<image-mask-list> is not a valid path" )
        return 1
    except IndexError as e:
        print( "<image-mask-list> must contain two columns" )
        return 2

    histBaseName = os.path.basename(imageMaskListPath)
    args = [
        progs['DetermineBinWidths'],
        "--infile", imageMaskListPath,
        "--outdir", outDir,
        "--prefix", histBaseName + '_',
        "--bins", "%d" % numBins,
        "--samples", "%d" % numSamples,
    ]

    if subprocess.call( args ) != 0:
        print( 'Error determining bin widths' )
        return 3
    
    pattern = os.path.join(outDir, histBaseName + '_bins*.txt')
    histSpecFile = glob.glob(pattern)[0]
    niigzMatcher = re.compile('(.*).nii.gz')
    for image, mask in imageMaskList:
        baseName = niigzMatcher.match( os.path.basename(image) ).group(1)
        args = [
            progs['MaskedImageHistogram'],
            "--image", image,
            "--mask", mask,
            "--histogram", histSpecFile,
            "--outdir", outDir,
            "--prefix", baseName + '_',
        ]
        if subprocess.call( args ) != 0:
            print( 'Failed to calculate histogram for image :', image )
            continue                        
        
if __name__ == '__main__':
    sys.exit( main() )
