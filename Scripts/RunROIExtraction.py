#!/usr/bin/python3

# Read a list of images and masks. There can be multiple images for each mask.
# For each mask
#   GenerateROIs
#   SampleROIs on the mask
#   For each image corresponding to the mask
#     SampleROIs
# 

import os, os.path, subprocess, glob, re, sys, csv


def main(argv=None):
    if argv is None:
        argv = sys.argv

    if len(argv) < 5:
        print( "Usage:", argv[0],
               "<images-mask-list> <outdir> <num-rois> <roi-size>" )
        return 1

    progs = {
        'GenerateROIs' : '../Build/ROIExtraction/GenerateROIs',
        'SampleROIs' : '../Build/ROIExtraction/SampleROIs',
    }
    
    imagesMaskListPath = argv[1]
    outDir = argv[2]
    try:
        numROIs = int(argv[3])
        if numROIs < 1: raise ValueError()
    except ValueError as e:
        print( "<num-rois> argument must be a positive integer" )
        return 1

    try:
        roiSize = int(argv[4])
        if roiSize < 1: raise ValueError()
    except ValueError as e:
        print( "<roi-size> argument must be a positive integer" )
        return 1    
    
    try:
        with open(imagesMaskListPath) as f:
            imagesMaskList = [(row[:-1], row[-1]) for row in csv.reader(f)]
    except FileNotFoundError as e:
        print( "<images-mask-list> is not a valid path" )
        return 1
    except IndexError as e:
        print( "<image-mask-list> must contain at least one column" )
        return 2

    dcmMatcher = re.compile('(.*).dcm')
    for images, mask in imagesMaskList:
        # Generate the ROIS
        maskBaseName = dcmMatcher.match(os.path.basename( mask ) ).group(1) 
        print( maskBaseName )
        args = [
            progs['GenerateROIs'],
            "--mask", mask,
            "--outdir", outDir,
            "--prefix", maskBaseName + "_",
            "--num-rois", "%d" % numROIs,
            "--roi-size", "%d" % roiSize,
        ]
        if subprocess.call(args) != 0:
            print( 'Failed to generate ROIs:', mask )
            return 2

        # Sample the ROIs
        # We need to sample the mask so we can easily keep track of the part of
        # the ROI that are outside the mask
        roiInfoFile = os.path.join( outDir, maskBaseName + '_ROIInfo.txt' )

        args = [
            progs['SampleROIs'],
            "--image", mask,
            "--roifile", roiInfoFile,
            "--outdir", outDir,
            "--prefix", maskBaseName + '_',
        ]
        if subprocess.call( args ) != 0:
            print( 'Failed to sample ROIs from mask:', mask )
            return 3
        
        
        niigzMatcher = re.compile('(.*).nii.gz')
        for image in images:
            baseName = niigzMatcher.match( os.path.basename(image) ).group(1)
            print( baseName )
            
            args = [
                progs['SampleROIs'],
                "--image", image,
                "--roifile", roiInfoFile,
                "--outdir", outDir,
                "--prefix", baseName + '_',
            ]
            if subprocess.call( args ) != 0:
                print( 'Failed to sample ROIs from image:', image )
                continue                        
        
if __name__ == '__main__':
    sys.exit( main() )
