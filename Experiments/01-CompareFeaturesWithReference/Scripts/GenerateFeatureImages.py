#!/usr/bin/python3
import sys, os.path, subprocess, csv
from Util import intersperse 

def main():
    skip = {
        'Make features' : False,
    }
        
    basedir = ''

    dirs = {
        'FileLists' : os.path.join(basedir, 'FileLists'),
        'Features' : os.path.join(basedir, 'Data', 'Features'),
        'Bin' : '../../Build',
    }
    
    files = {
        'ImageMaskList' : os.path.join(dirs['FileLists'], 'ImageMaskList.csv'),
    }
    
    progs = {
        'ExtractFeatures' : os.path.join(dirs['Bin'],'FeatureExtraction/ExtractFeatures'),
    }
    
    params = {
        'scales' : ["0.6", "1.2"]
    }


    # Get the list of image/reference pairs
    with open(files['ImageMaskList']) as infile:
        imageMaskList = [
            (row[0].strip(),
             row[1].strip())
            for row in csv.reader(infile)]
        
    if skip['Make features']:
        print( 'Skipping: Make features' )
    else:
        print( 'Making features' )
        for image, mask in imageMaskList:
            print( "Using image '%s'" % image )
            print( "Using mask '%s'" % mask )
            args = [
                progs['ExtractFeatures'],
                '--image', image,
                '--mask', mask,
                '--out', os.path.join(dirs['Features'], os.path.basename(mask))
            ] + list(intersperse('--scale', (s for s in params['scales'])))
        
            print(' '.join(args))        
            if subprocess.call( args ) != 0:
                print( 'Error extracting features' )
                return 1
    return 0

if __name__ == '__main__':
    sys.exit( main() )
