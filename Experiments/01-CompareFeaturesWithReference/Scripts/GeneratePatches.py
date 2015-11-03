#!/usr/bin/python3
import sys, os.path, subprocess, csv
from Util import intersperse 

def main():
    skip = {
        'Make patches' : False,
    }
        
    basedir = ''

    dirs = {
        'FileLists' : os.path.join(basedir, 'FileLists'),
        'Out' : os.path.join(basedir, 'Data', 'ROIs'),
        'Bin' : '../../Build',
    }
    
    files = {
        'ImageROIList' : os.path.join(dirs['FileLists'], 'ImageROIList.csv'),
    }
    
    progs = {
        'SampleROIs' : os.path.join(dirs['Bin'],'ROIExtraction/SampleROIs'),
    }    

    # Get the list of image/reference pairs
    with open(files['ImageROIList']) as infile:
        imageROIList = [
            (row[0].strip(),
             row[1].strip())
            for row in csv.reader(infile)]
        
    if skip['Make patches']:
        print( 'Skipping: Make patches' )
    else:
        print( 'Making patches' )
        for image, roiInfo in imageROIList:
            print( "Using image '%s'" % image )
            print( "Using ROIinfo '%s'" % roiInfo )
            args = [
                progs['SampleROIs'],
                '--image', image,
                '--roifile', roiInfo,
                '--outdir', dirs['Out'],
                '--prefix', os.path.basename(image),
            ]
        
            print(' '.join(args))        
            if subprocess.call( args ) != 0:
                print( 'Error extracting features' )
                return 1
    return 0

if __name__ == '__main__':
    sys.exit( main() )
