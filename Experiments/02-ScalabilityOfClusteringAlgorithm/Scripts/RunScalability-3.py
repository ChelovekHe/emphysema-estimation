#!/usr/bin/python3
'''Run Scalability-3.
See README.md for details.
'''

import sys, subprocess, os.path
from Util import intersperse

def main():
    skip = {
        'Profile' : False,
    }
        
    basedir = ''

    dirs = {
        'Instances' : os.path.join(basedir, 'Data', 'Instances'),
        'Profiling' : os.path.join(basedir, 'Data', 'Profiling'),
        'Bin' : '../../Build',
    }
    
    files = {
        'Instances' : os.path.join(dirs['Instances'], 'instances2500.csv'),
        'Profile' : os.path.join(dirs['Profiling'], 'instances2500_k32_%p.out'),
    }
    
    progs = {
        'Scalability' : os.path.join(dirs['Bin'],'Experiments/02-ScalabilityOfClusteringAlgorithm/Scalability'),
        'cachegrind' : ['valgrind', '--tool=cachegrind', '--cachegrind-out-file=' + files['Profile']],
    }
    
    params = {
        'clusters' : 32,
        'histograms' : 7*8, # 7 scales * 8 features
        'burnin' : 1,
        'iterations' : 0,
        'branching' : 32,
    }
       
    if skip['Profile']:
        print( 'Skipping: Profile' )
    else:
        print( 'Profiling' )
        args = progs['cachegrind'] + [
            progs['Scalability'],
            '--input', files['instances'],
            '--nHistograms', "%d" % params['histograms'],
            '--output', files['Profile'],
            '--burnin', "%d" % params['burnin'],
            '--iterations', "%d" % params['iterations'],
            '--branching', "%d" % params['branching'],
            '--clusters', "%d" % params['clusters']
        ]
        
        print(' '.join(args))        
        if subprocess.call( args ) != 0:
            print( 'Error profiling', files['instances'] )
            return 1
    return 0

if __name__ == '__main__':
    sys.exit( main() )
