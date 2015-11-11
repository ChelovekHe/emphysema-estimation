#!/usr/bin/python3
'''See ../Data/Dataset-3/README.md'''

import sys, os, os.path, subprocess, math, csv
from Util import intersperse 

def main():
    basedir = '../Data/Dataset-3/Test'

    dirs = {
        'Bags' : os.path.join(basedir, 'Bags'),
        'FileLists' : os.path.join(basedir, 'FileLists'),
        'Histograms' : os.path.join(basedir, 'Histograms'),
        'Bin' : '../Build'
    }
    
    files = {
        'ScanMaskList' : os.path.join(dirs['FileLists'], 'ScanLungList.txt'),
        'HistogramSpec' : os.path.join(dirs['Histograms'], 'HistogramSpecFromTraining.txt'),
    }
    
    progs = {
        'MakeBag' : os.path.join(dirs['Bin'],'FeatureExtraction/MakeBag'),
    }

    params = {
        'scales' : ["%0.2f" % (0.6 * math.sqrt(2.0)**i) for i in range(7)],
        'num-rois' : 50,
        'roi-size-x' : 41,
        'roi-size-y' : 41,
        'roi-size-z' : 41,
    }
    
    # Make the bags
    print( 'Making bags' )
    with open(files['ScanMaskList']) as infile:
        scanMaskList = [(row[0].strip(), row[1].strip()) for row in csv.reader(infile)]
        
    for scan, mask in scanMaskList:
        print( "Using scan '%s'" % scan )
        print( "Using mask '%s'" % mask )
        args = [
            progs['MakeBag'],
            '--image', scan,
            '--mask', mask,
            '--histogram-spec', files['HistogramSpec'],
            '--outdir', dirs['Bags'],
            '--prefix', os.path.basename(scan),
            '--num-rois', '%d' % params['num-rois'],
            '--roi-size-x', '%d' % params['roi-size-x'],
            '--roi-size-y', '%d' % params['roi-size-y'],
            '--roi-size-z', '%d' % params['roi-size-z'],
        ] + list(intersperse('--scale', (s for s in params['scales'])))
        
        print(' '.join(args))        
        if subprocess.call( args ) != 0:
            print( 'Error making bag' )
            return 1
         
    
    return 0

if __name__ == '__main__':
    sys.exit( main() )



    
