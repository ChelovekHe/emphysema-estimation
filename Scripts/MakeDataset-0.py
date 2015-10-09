#!/usr/bin/python3

import sys, os, os.path, csv, math, subprocess, glob

def main():
    baseDir = '../Data/Dataset-0'
    scanDir = os.path.join(baseDir, 'Scans')
    maskDir = os.path.join(baseDir, 'Lungs')
    tmpDir = os.path.join(baseDir, 'Tmp')
    featureDir = os.path.join(baseDir, 'Features')
    fileListDir = os.path.join(baseDir, 'FileLists')
    scanMaskListPath = os.path.join(fileListDir, 'ScanMaskList.csv')
    featureExtractionScript = './RunFeatureExtraction.py'
    scales = ["%0.2f" % (0.6 * math.sqrt(2.0)**i) for i in range(7)]
    
    # Make the scan-mask list
    print('Creating scan-mask list')
    scanFiles = glob.glob( os.path.join( scanDir, 'vol*.dcm' ) )
    maskFiles = glob.glob( os.path.join( maskDir, 'vol*-D-1-lungs.dcm' ) )
    
    scanMaskList = zip( sorted(scanFiles), sorted(maskFiles) )
    with open(scanMaskListPath, 'w') as out:
        csv.writer( out ).writerows( scanMaskList )

    # Run feature extraction
    print('Running feature extraction')
    args = [
        featureExtractionScript,
        scanMaskListPath,
        featureDir,
        tmpDir,
    ] + scales
        
    if subprocess.call( args ) != 0:
        print( 'Error running feature extraction' )
        return 1
        
    return 0


if __name__ == '__main__':
    sys.exit( main() )



    
