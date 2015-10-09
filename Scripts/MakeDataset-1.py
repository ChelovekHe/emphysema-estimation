#!/usr/bin/python3

import sys, os, os.path, csv, glob, subprocess

def main():
    dataset0Dir = '../Data/Dataset-0'
    dataset1Dir = '../Data/Dataset-1'
    
    maskDir = os.path.join(dataset0Dir, 'Lungs')
    featureDir = os.path.join(dataset0Dir, 'Features')
    fileListDir = os.path.join(dataset1Dir, 'FileLists')
    roiDir = os.path.join(dataset1Dir, 'ROIs')
    imagesMaskListPath = os.path.join(fileListDir, 'FeaturesMaskList.csv')
    roiExtractionScript = './RunROIExtraction.py'

    numROIs = 1000
    roiSize = 41
    
    # Make the images-mask list
    print('Creating images-mask list')
    intensity = glob.glob( os.path.join( featureDir, '*masked*' ) )
    gradientMagnitude = glob.glob( os.path.join( featureDir, '*GradientMagnitude*' ) )
    eig1 = glob.glob( os.path.join( featureDir, '*eig1*' ) )
    eig2 = glob.glob( os.path.join( featureDir, '*eig2*' ) )
    eig3 = glob.glob( os.path.join( featureDir, '*eig3*' ) )
    LoG = glob.glob( os.path.join( featureDir, '*LoG*' ) )
    Curvature = glob.glob( os.path.join( featureDir, '*Curvature*' ) )
    Frobenius = glob.glob( os.path.join( featureDir, '*Frobenius*' ) )
    mask = glob.glob( os.path.join( maskDir, '*lungs*' ) )

    imagesMaskList = zip( sorted(intensity),
                          sorted(gradientMagnitude),
                          sorted(eig1),
                          sorted(eig2),
                          sorted(eig3),
                          sorted(LoG),
                          sorted(Curvature),
                          sorted(Frobenius),
                          sorted(mask)
                      )
    
    with open(imagesMaskListPath, 'w') as out:
        csv.writer( out ).writerows(imagesMaskList)

    # Run ROI extraction
    print('Running ROI extraction')
    args = [
        roiExtractionScript,
        imagesMaskListPath,
        roiDir,
        "%d" % numROIs,
        "%d" % roiSize,
    ]
        
    if subprocess.call( args ) != 0:
        print( 'Error running ROI extraction' )
        return 1
        
    return 0


if __name__ == '__main__':
    sys.exit( main() )



    
