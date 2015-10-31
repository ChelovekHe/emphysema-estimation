#!/usr/bin/python3
import sys, os, os.path, subprocess, math, csv, random, re
from Util import intersperse 

def main():
    skip = {
        'Make histograms' : False,
        'Make plots' : False,
    }
        
    basedir = './'

    dirs = {
        'FileLists' : os.path.join(basedir, 'FileLists'),
        'Histograms' : os.path.join(basedir, 'Histograms'),
        'Bin' : '../../Build',
        'Scripts' : os.path.join(basedir, 'Scripts'),
    }
    
    files = {
        'ImageReferenceMaskList' : os.path.join(dirs['FileLists'], 'ImageReferenceMaskList.csv'),
    }
    
    progs = {
        'ComputeJointHistogram' : os.path.join(dirs['Bin'],'Statistics/ComputeJointHistogram'),
        'PlotJointHistogram' : os.path.join(dirs['Scripts'],'PlotJointHistogram.R'),
    }


    # Get the list of image/reference pairs
    with open(files['ImageReferenceMaskList']) as infile:
        imageReferenceMaskHistList = [
            (row[0].strip(),
             row[1].strip(),
             row[2].strip(),
             os.path.join( dirs['Histograms'], os.path.basename(row[0].strip()) + '_histogram'))
            for row in csv.reader(infile)]
        
    if skip['Make histograms']:
        print( 'Skipping: Make histograms' )
    else:
        print( 'Making histograms' )                
        for image, reference, mask, hist in imageReferenceMaskHistList:
            print( "Using image '%s'" % image )
            print( "Using reference '%s'" % reference )
            print( "Using mask '%s'" % mask )
            print( "Using hist '%s'" % hist )
            args = [
                progs['ComputeJointHistogram'],
                '--image-1', image,
                '--image-2', reference,
                '--mask', mask,
                '--out', hist + '.txt',
                ]
        
            print(' '.join(args))        
            if subprocess.call( args ) != 0:
                print( 'Error making histograms' )
                return 1
    
    if skip['Make plots']:
        print( 'Skipping: Make plots' )
    else:
        featurePattern = re.compile('.*[0-9]{6}(.*)\.nii\.gz.*')
        lungStylePattern = re.compile('.*lungs-(.*)\.dcm.*')
        scalePattern = re.compile('.*scale_([01]\.[26]).*')
        print( 'Making plots' )
        for _,_,_,hist in imageReferenceMaskHistList:
            featureName = featurePattern.match(hist).group(1)
            lungStyle = lungStylePattern.match(hist).group(1)
            scale = scalePattern.match(hist).group(1)
            args = [
                progs['PlotJointHistogram'],
                hist + '.txt',
                hist + '.png',
                "%s: %s @ %s" % (lungStyle, featureName, scale),
            ]
            print(' '.join(args))        
            if subprocess.call( args ) != 0:
                print( 'Error plotting histograms' )
                return 1
    return 0

if __name__ == '__main__':
    sys.exit( main() )
