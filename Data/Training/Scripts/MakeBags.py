#!/usr/bin/python3
'''See ../README.md'''

import sys, os.path, subprocess, csv
from Util import intersperse 

def main():
    scanLungListPath = 'FileLists/ScansLungs.csv'
    outdir = 'Bags'
    prog = '../../Build/FeatureExtraction/MakeBag'
    
    params = {
        'histogram-spec' : 'HistogramEdges_1456312478.268112.txt',
        'scales' : ['1.2', '2.4', '4.8'],
        'num-rois' : '50',
        'roi-size-x' : '53',
        'roi-size-y' : '53',
        'roi-size-z' : '41',
    }
    
    # Make the bags
    print( 'Making bags' )
    with open(scanLungListPath) as infile:
        scanLungList = [(row[0].strip(), row[1].strip())
                               for row in csv.reader(infile)]
        
    for scan, lung in scanLungList:
        print( "Using scan '%s'" % scan )
        print( "Using lung mask '%s'" % lung )
        args = [
            prog,
            '--image', scan,
            '--mask', lung,
            '--histogram-spec', params['histogram-spec'],
            '--outdir', outdir,
            '--prefix', os.path.basename(scan),
            '--num-rois', params['num-rois'],
            '--roi-size-x', params['roi-size-x'],
            '--roi-size-y', params['roi-size-y'],
            '--roi-size-z', params['roi-size-z'],
        ] + list(intersperse('--scale', (s for s in params['scales'])))
        
        print(' '.join(args))        
        if subprocess.call( args ) != 0:
            print( 'Error making bag' )
            return 1
    
    return 0

if __name__ == '__main__':
    sys.exit( main() )



    
