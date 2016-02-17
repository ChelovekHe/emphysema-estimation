#!/usr/bin/python3
'''Run Scalability Thesis version.
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
        'Statistics' : os.path.join(basedir, 'Data', 'Statistics', 'Scalability-Thesis'),
        'Bin' : '../../Build',
    }
    
    files = {
        'Instances' : [
            # os.path.join(dirs['Instances'], 'instances500.csv'),
            # os.path.join(dirs['Instances'], 'instances1000.csv'),
            # os.path.join(dirs['Instances'], 'instances2500.csv'),
            # os.path.join(dirs['Instances'], 'instances5000.csv'),
            #os.path.join(dirs['Instances'], 'instances7500.csv'),
            os.path.join(dirs['Instances'], 'instances10000.csv')
        ],
    }
    
    progs = {
        'Scalability' : os.path.join(dirs['Bin'],'Experiments/02-ScalabilityOfClusteringAlgorithm/Scalability'),
    }
    
    params = {
        'histograms' : 7*8, # 7 scales * 8 features
        'burnin' : 10,
        'iterations' : 100,
        
        'Scalability-Thesis-1': {
            'clusters' : [2, 4, 8, 16, 32, 64],
            'branching' : 2,
            'kmeans-iterations' : 11,
        },
        'Scalability-Thesis-2': {
            'clusters' : [2, 4, 8, 16, 32, 64],
            'branching' : 1,
            'kmeans-iterations' : 11,
        },
        'Scalability-Thesis-3': {
            'clusters' : [2, 4, 8, 16, 32, 64],
            'branching' : 2,
            'kmeans-iterations' : 100,
        },

        'Scalability-Thesis-4': {
            'clusters' : [2, 4, 8, 16, 32, 64],
            'branching' : 1,
            'kmeans-iterations' : 100,
        },
    }

    experiments = [
#        'Scalability-Thesis-1',
#        'Scalability-Thesis-2',
#        'Scalability-Thesis-3',
        'Scalability-Thesis-4',
    ]

    for ex in experiments:
        if ex in skip:
            print( 'Skipping:', ex )
        else:
            print( 'Running', ex )
            for instanceMatrix in files['Instances']:
                outname = '%s_%s' % (ex, os.path.basename(instanceMatrix))
                args = [
                    progs['Scalability'],
                    '--input', instanceMatrix,
                    '--nHistograms', "%d" % params['histograms'],
                    '--output', os.path.join(dirs['Statistics'], outname), 
                    '--burnin', "%d" % params['burnin'],
                    '--iterations', "%d" % params['iterations'],
                    '--branching', "%d" % params[ex]['branching'],
                    '--kmeans-iterations', "%d" % params[ex]['kmeans-iterations'],
                ] + list(intersperse('--clusters', ("%d" % k for k in params[ex]['clusters'])))
            
                print(' '.join(args))        
                if subprocess.call( args ) != 0:
                    print( 'Error measuring', instanceMatrix )
                    return 1
    return 0

if __name__ == '__main__':
    sys.exit( main() )
