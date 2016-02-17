#!/usr/bin/python3
'''See ../Data/Dataset-3/README.md'''

import sys, os, os.path, subprocess, math, csv
from Util import intersperse 

def main():
    indir = '../Data/Dataset-3'
    outdir = '../Data/Dataset-3/Training'
    bindir = '../Build'
    histDir = os.path.join(outdir, 'Histograms')

    dirs = {
        'Bags' : os.path.join(outdir, 'Bags'),
        'FileLists' : os.path.join(outdir, 'FileLists'),
    }
    
    files = {
        'ScanMaskList' : os.path.join(dirs['FileLists'], 'ScanLungList.txt'),
        'HistogramSpec' : os.path.join(histDir, 'HistogramSpec.txt'),
    }
    
    progs = {
        'HistBinEdges' : os.path.join(bindir, 'Statistics/DetermineHistogramBinEdges'),
        'MakeBag' : os.path.join(bindir,'FeatureExtraction/MakeBag'),
    }

    params = {
        'bins' : 41,
        'samples' : 10000,
        'scales' : ["%0.2f" % (0.6 * math.sqrt(2.0)**i) for i in range(7)],
        'num-rois' : 50,
        'roi-size-x' : 41,
        'roi-size-y' : 41,
        'roi-size-z' : 41,
    }
    
    # Determine the bin edges
    print( 'Estimating bin edges for equalized histograms' )
    args = [
        progs['HistBinEdges'],
        '--infile', files['ScanMaskList'],
        '--outfile', files['HistogramSpec'],
        '--bins', '%d' % params['bins'],
        '--samples', '%d' % params['samples'],
        ] + list(intersperse('--scale', (s for s in params['scales'])))

    print(' '.join(args))        
    if subprocess.call( args ) != 0:
        print( 'Error running histogram bin edge estimation' )
        return 1

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



    
