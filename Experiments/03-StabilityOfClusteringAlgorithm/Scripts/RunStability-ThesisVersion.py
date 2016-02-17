#!/usr/bin/python3
'''Run Stability Thesis version
See README.md for details.
'''

import sys, subprocess, os.path
from Util import intersperse

def main():
    skip = [
        'Stability-Thesis-3',
        'Stability-Thesis-4',
    ]
    
    basedir = ''

    dirs = {
        'Instances' : os.path.join(basedir, 'Data', 'Instances'),
        'Distances' : os.path.join(basedir, 'Distances'),
        'Bin' : '../../Build',
    }
    
    files = {
        'Instances' : [
            os.path.join(dirs['Instances'], 'instances1000.csv'),
        ],
    }
    
    progs = {
        'Stability' : os.path.join(dirs['Bin'],'Experiments/03-StabilityOfClusteringAlgorithm/Stability'),
    }
    
    params = {
        'histograms' : 7*8, # 7 scales * 8 features
        'iterations' : 100,

        'Stability-Thesis-1' : {
            'clusters' : [2, 4, 8, 16, 32, 64],
            'branching' : 2 ,
            'kmeans-iterations' : 11,
        },
        'Stability-Thesis-2' : {
            'clusters' : [2, 4, 8, 16, 32, 64],
            'branching' : 1 ,
            'kmeans-iterations' : 11,
        },
        'Stability-Thesis-3' : {
            'clusters' : [2, 4, 8, 16, 32, 64],
            'branching' : 2 ,
            'kmeans-iterations' : -1,
        },
        'Stability-Thesis-4' : {
            'clusters' : [2, 4, 8, 16, 32, 64],
            'branching' : 1 ,
            'kmeans-iterations' : -1,
        },
    }

    experiments =  [
        'Stability-Thesis-1',
        'Stability-Thesis-2',
        'Stability-Thesis-3',
        'Stability-Thesis-4',
    ]

    for ex in experiments:
        if ex in skip:
            print( 'Skipping:', ex )
        else:
            print( 'Running:', ex )
            for instanceMatrix in files['Instances']:
                outname = '%s_%s' % (ex, os.path.basename(instanceMatrix))
                args = [
                    progs['Stability'],
                    '--input', instanceMatrix,
                    '--histograms', "%d" % params['histograms'],
                    '--output', os.path.join(dirs['Distances'], outname),
                    '--iterations', "%d" % params['iterations'],
                    '--kmeans-iterations', "%d" % params[ex]['kmeans-iterations'],
                    '--branching', "%d" % params[ex]['branching']
            ] + list(intersperse('--clusters', ("%d" % k for k in params[ex]['clusters'])))
        
                print(' '.join(args))        
                if subprocess.call( args ) != 0:
                    print( 'Error running', ex, '.Using:', instanceMatrix )
                    return 1
    return 0

if __name__ == '__main__':
    sys.exit( main() )
