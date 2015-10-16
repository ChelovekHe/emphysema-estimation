#!/usr/bin/python3

# Read a file list and a set of scales
# For each file 
#   Run MaskedNormalizedConvoution without masking output
#   For each of the filtered images
#     Run FD_HessianFeatures
#     Run FD_GradientFeatures
#     Run MaskedFilter
#     Delete the filtered image
import os, os.path, subprocess, glob, re, sys, csv
from Util import intersperse


def main(argv=None):
    if argv is None:
        argv = sys.argv

    if len(argv) < 5:
        print( "Usage:", argv[0],
               "<image-mask-list> <outdir> <tmpdir> <scale>+" )
        return 1

    progs = {
        'MaskedNormalizedConvolution' : '../Build/FeatureExtraction/MaskedNormalizedConvolution',
        'MaskedImageFilter' : '../Build/FeatureExtraction/MaskedImageFilter',
        'GradientFeatures' : '../Build/FeatureExtraction/FiniteDifference_GradientFeatures',
        'HessianFeatures' : '../Build/FeatureExtraction/FiniteDifference_HessianFeatures',
    }
    
    imageMaskListPath = argv[1]
    outDir = argv[2]
    tmpDir = argv[3]
    try:
        scales = [ float(x) for x in argv[4:] ]
    except ValueError as e:
        print( "<scale> arguments must be numbers" )
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

    scaleMatcher = re.compile('.*(scale_[0-9]*\.[0-9]*)')
    
    for image, mask in imageMaskList:
        # Run MaskedNormalizedConvolution
        baseName = os.path.basename( image )[:-4]
        print( baseName )
        args = [
            progs['MaskedNormalizedConvolution'],
            "--image", image,
            "--certainty", mask,
            "--outdir", tmpDir,
            "--prefix", "unmasked" + baseName + "_",
            "--maskoutput", "0",
        ] + list(intersperse("--scale", ("%0.2f" % s for s in scales)))
        if subprocess.call(args) != 0:
            print( 'Failed to process:', image )
            continue

        # Now we extract features from the unmasked images
        # We want to name the images according to scale
        paths = glob.glob( os.path.join( tmpDir, 'unmasked*' ) )
        for path in paths:
            scale = scaleMatcher.match(path).group(1)
            prefix = baseName + "_" + scale + "_"
            print( scale )
            
            # Intensity features
            args = [
                progs['MaskedImageFilter'],
                "--image", path,
                "--mask", mask,
                "--outdir", outDir,
                "--prefix", prefix,
            ]
            print( ' '.join(args) )
            if subprocess.call( args ) != 0:
                print( 'Failed to extract gradient features' )
                continue

            # Gradient features
            args = [
                progs['GradientFeatures'],
                "--image", path,
                "--mask", mask,
                "--outdir", outDir,
                "--prefix", prefix,
            ]
            print( ' '.join(args) )
            if subprocess.call( args ) != 0:
                print( 'Failed to extract gradient features' )
                continue

            # Hessian features
            args = [
                progs['HessianFeatures'],
                "--image", path,
                "--mask", mask,
                "--outdir", outDir,
                "--prefix", prefix,
            ]
            print( ' '.join(args) )
            if subprocess.call( args ) != 0:
                print( 'Failed to extract Hessian features' )
                continue

            # Delete the unmasked image
            os.remove( path )
            
            
        
if __name__ == '__main__':
    sys.exit( main() )
