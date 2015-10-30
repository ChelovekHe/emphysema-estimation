#!/usr/bin/python3
'''See ../Data/Dataset-1/README.md'''

import sys, os, os.path, subprocess, math, csv, random
from Util import intersperse 

def main():
    # Set to False to actually do something
    skip = {
        'Make bags' : True,
        'Make instance matrices' : True,
    }
    
    basedir = './'

    dirs = {
        'Bags' : os.path.join(basedir, 'Bags'),
        'Histograms' : os.path.join(basedir, 'Histograms'),
        'FileLists' : os.path.join(basedir, 'FileLists'),
        'Instances' : os.path.join(basedir, 'Instances'),
        'Bin' : '../../Build'
    }
    
    files = {
        'ScanMaskList' : os.path.join(dirs['FileLists'], 'ScanMaskList.csv'),
        'HistogramSpec' : os.path.join(dirs['Histograms'], 'Dataset-3-HistogramSpec.txt'),
        'InstanceMatrixBaseName' : os.path.join(dirs['Instances'], 'instances%d.csv'),
    }
    
    progs = {
        'MakeBag' : os.path.join(dirs['Bin'],'FeatureExtraction/MakeBag'),
    }

    params = {
        'scales' : ["%0.2f" % (0.6 * math.sqrt(2.0)**i) for i in range(7)],
        'num-rois' : 2000,
        'roi-size-x' : 41,
        'roi-size-y' : 41,
        'roi-size-z' : 41,
        'instance-matrix-sizes' : [500, 1000, 2500, 5000, 7500, 10000],
    }

    # We need the list for both bag and matric production
    with open(files['ScanMaskList']) as infile:
        scanMaskList = [(row[0].strip(), row[1].strip()) for row in csv.reader(infile)]

    
    if skip['Make bags']:
        print( 'Skipping: Make bags' )
    else:
        print( 'Making bags' )                
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

    
    if skip['Make instance matrices']:
        print( 'Skipping: Make instance matrices' )
    else:
        print( 'Making instance matrices' )
        bags = [ os.path.join(dirs['Bags'],os.path.basename(scan) + 'bag.txt') for scan,_ in scanMaskList]
        samples = []
        for bag in bags:
            with open(bag) as f:
                samples += [row for row in csv.reader(f)]

        for size in params['instance-matrix-sizes']:
            outpath = files['InstanceMatrixBaseName'] % size
            with open(outpath, 'w') as out:
                csv.writer( out ).writerows( random.sample(samples, size) )
    
    return 0

if __name__ == '__main__':
    sys.exit( main() )
