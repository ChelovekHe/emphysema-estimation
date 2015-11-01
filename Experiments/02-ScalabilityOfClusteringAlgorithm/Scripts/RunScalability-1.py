#!/usr/bin/python3
'''Run Scalability-1.
See README.md for details.
'''

import sys, subprocess, os.path
from Util import intersperse

def main():
    skip = {
        'Measure' : False,
    }
        
    basedir = ''

    dirs = {
        'Instances' : os.path.join(basedir, 'Data', 'Instances'),
        'Statistics' : os.path.join(basedir, 'Data', 'Statistics', 'Scalability-1'),
        'Bin' : '../../Build',
    }
    
    files = {
        'Instances' : [
            os.path.join(dirs['Instances'], 'instances500.csv'),
            os.path.join(dirs['Instances'], 'instances1000.csv'),
            os.path.join(dirs['Instances'], 'instances2500.csv'),
            os.path.join(dirs['Instances'], 'instances5000.csv'),
            os.path.join(dirs['Instances'], 'instances7500.csv'),
            os.path.join(dirs['Instances'], 'instances10000.csv')
        ],
    }
    
    progs = {
        'Scalability' : os.path.join(dirs['Bin'],'Experiments/02-ScalabilityOfClusteringAlgorithm/Scalability'),
    }
    
    params = {
        'clusters' : [4, 8, 16, 32, 64],
        'histograms' : 7*8, # 7 scales * 8 features
        'burnin' : 10,
        'iterations' : 100,
        'branching' : 1,
    }
       
    if skip['Measure']:
        print( 'Skipping: Measure' )
    else:
        print( 'Measuring' )
        for instanceMatrix in files['Instances']:
            args = [
                progs['Scalability'],
                '--input', instanceMatrix,
                '--nHistograms', "%d" % params['histograms'],
                '--output', os.path.join(dirs['Statistics'], 'stats_' + os.path.basename(instanceMatrix)),
                '--burnin', "%d" % params['burnin'],
                '--iterations', "%d" % params['iterations'],
                '--branching', "%d" % params['branching']
            ] + list(intersperse('--clusters', ("%d" % k for k in params['clusters'])))
        
            print(' '.join(args))        
            if subprocess.call( args ) != 0:
                print( 'Error measuring', instanceMatrix )
                return 1
    return 0

if __name__ == '__main__':
    sys.exit( main() )
