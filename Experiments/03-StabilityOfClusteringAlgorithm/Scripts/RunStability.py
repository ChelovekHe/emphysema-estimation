#!/usr/bin/python3
'''Run Stability
See README.md for details.
'''

import sys, subprocess, os.path
from Util import intersperse

def main():
    skip = {
        'Stability-1' : False,
        'Stability-2' : False,
        'Stability-3' : False,
        'Stability-4' : False,
    }
        
    basedir = ''

    dirs = {
        'Instances' : os.path.join(basedir, 'Data', 'Instances'),
        'Distances' : os.path.join(basedir, 'Distances'),
        'Bin' : '../../Build',
    }
    
    files = {
        'Instances' : [
            os.path.join(dirs['Instances'], 'instances500.csv'),
        ],
    }
    
    progs = {
        'Stability' : os.path.join(dirs['Bin'],'Experiments/03-StabilityOfClusteringAlgorithm/Stability'),
    }
    
    params = {
        'histograms' : 7*8, # 7 scales * 8 features
        'iterations' : 100,

        'Stability-1' : {
            'clusters' : [2, 4, 8, 16, 32, 64],
            'branching' : 2,
        },
        'Stability-2' : {
            'clusters' : [4, 7, 16, 31, 64],
            'branching' : 4,
        },
        'Stability-3' : {
            'clusters' : [8, 15, 29, 64],
            'branching' : 8,
        },
        'Stability-4' : {
            'clusters' : [16, 31, 61],
            'branching' : 16,
        },
        'Stability-5' : {
            'clusters' : [32, 63],
            'branching' : 32,
        },        
    }

    experiments =  [
        'Stability-1',
        'Stability-2',
        'Stability-3',
        'Stability-4',
        'Stability-5'
    ]

    for ex in experiments:
        if skip[ex]:
            print( 'Skipping:', ex )
        else:
            print( 'Running:', ex )
            for instanceMatrix in files['Instances']:
                outname = '%s_%s' % (ex,
                                        os.path.basename(instanceMatrix))
                args = [
                    progs['Stability'],
                    '--input', instanceMatrix,
                    '--histograms', "%d" % params['histograms'],
                    '--output', os.path.join(dirs['Distances'], outname),
                    '--iterations', "%d" % params['iterations'],
                    '--branching', "%d" % params[ex]['branching']
            ] + list(intersperse('--clusters', ("%d" % k for k in params[ex]['clusters'])))
        
                print(' '.join(args))        
                if subprocess.call( args ) != 0:
                    print( 'Error running', ex, '.Using:', instanceMatrix )
                return 1
    return 0

if __name__ == '__main__':
    sys.exit( main() )
