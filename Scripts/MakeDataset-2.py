#!/usr/bin/python3
'''See ../Data/Dataset-2/README.md'''

import sys, os, os.path, csv, glob, subprocess, re

def main():
    dataset0Dir = '../Data/Dataset-0'
    dataset2Dir = '../Data/Dataset-2'

    instancesFile = os.path.join( dataset2Dir, 'instances.csv' )
    
    maskDir = os.path.join(dataset0Dir, 'Lungs')
    featureDir = os.path.join(dataset0Dir, 'Features')
    fileListDir = os.path.join(dataset2Dir, 'FileLists')
    roiDir = os.path.join(dataset2Dir, 'ROIs')
    histogramDir = os.path.join(dataset2Dir, 'Histograms')

    imagesMaskListPath = os.path.join(fileListDir, 'FeaturesMaskList.csv')
    roiMaskListPath = os.path.join(fileListDir, 'ROIMaskList.csv')

    roiExtractionScript = './RunROIExtraction.py'
    maskedHistogramScript = './RunMaskedHistograms.py'

    numROIs = 50
    roiSize = 41
    numBins = 41
    numSamples = 10000
    scales = [(0.6 * math.sqrt(2.0)**i) for i in range(7)]
    
    # Make the images-mask list
    print('Creating images-mask list')
    intensity = sorted(glob.glob( os.path.join( featureDir, '*masked*' ) ))
    gradientMagnitude = sorted(glob.glob( os.path.join( featureDir, '*GradientMagnitude*' ) ))
    eig1 = sorted(glob.glob( os.path.join( featureDir, '*eig1*' ) ))
    eig2 = sorted(glob.glob( os.path.join( featureDir, '*eig2*' ) ))
    eig3 = sorted(glob.glob( os.path.join( featureDir, '*eig3*' ) ))
    LoG = sorted(glob.glob( os.path.join( featureDir, '*LoG*' ) )) 
    Curvature = sorted(glob.glob( os.path.join( featureDir, '*Curvature*' ) ))
    Frobenius = sorted(glob.glob( os.path.join( featureDir, '*Frobenius*' ) ))

    # We have features at multiple scales, but the mask is only at one scale
    # so we need to repeat it
    mask = glob.glob( os.path.join( maskDir, '*lungs*' ) )
    scanNames = [os.path.basename(s)[:-14] for s in mask]
    repeatMask = max(1, len(intensity)/len(mask))
    mask = sorted( mask * repeatMask )
    
    imagesMaskList = zip( intensity,
                          gradientMagnitude,
                          eig1,
                          eig2,
                          eig3,
                          LoG,
                          Curvature,
                          Frobenius,
                          mask
                      )
    
    with open(imagesMaskListPath, 'w') as out:
        csv.writer( out ).writerows(imagesMaskList)

    # Run ROI extraction
    # This creates a lot of ROIs that need to be grouped according to which
    # image they belong to.
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

    # Make the ROI/mask list
    print( "Creating ROI/mask list for each feature and calculating histograms." )
    featureNames = [
        'masked',
        'GradientMagnitude',
        'eig1',
        'eig2',
        'eig3',
        'LoG',
        'Curvature',
        'Frobenius'
    ]

    maskPattern = os.path.join( roiDir, '*lungs*nii.gz' )
    maskPaths = sorted(glob.glob( maskPattern ))
    for featureName in featureNames:
        pattern = os.path.join( roiDir, '*' + featureName + '*' )
        paths = sorted(glob.glob( pattern ) )
        if len(paths) > 0:
            fileListPath = os.path.join(fileListDir, 'ROI' + featureName + '.csv')
            try:
                with open( fileListPath, 'w' ) as out:
                    csv.writer( out ).writerows( zip(paths, maskPaths) )
            except Exception as e:
                print(e)
            args = [
                maskedHistogramScript,
                fileListPath,
                histogramDir,
                "%d" % numBins,
                "%d" % numSamples
            ]
            if subprocess.call(args) != 0:
                print( 'Error creating histograms' )

    # Now we need to concatenate the histogram responses into a matrix of
    # ROIs and histograms
    # A ROI is identified by the scan name and the ROI number.
    print( 'Collating histogram into feature matrix' )
    instances = []
    for scanName in scanNames:
        for roi in range(numROIs):
            instance = []
            for featureName in featureNames:
                for scale in scales:
                    path = '%s_scale_%06f_%s_ROI_%05d_hist.txt' % (scanName, scale, featureName, roi)
                    with open(os.path.join(histogramDir, path)) as f:
                        for row in csv.reader( f ):
                            for elem in row:
                                instance.append( float(elem) )
            instances.append(instance)
            
    with open( instancesFile, 'w' ) as out:
        csv.writer( out ).writerows( instances )    
    
    return 0


if __name__ == '__main__':
    sys.exit( main() )



    
