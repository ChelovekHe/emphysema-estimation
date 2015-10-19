#!/usr/bin/python3
'''Run Experiment-1: KMeansProfiling
See README.md for details.
'''

import sys, subprocess, os.path

def main(argv=None):
    if argv is None:
        argv = sys.argv

    numFeatures = 8
    numScales = 7
    numHistograms = '%d' % (numFeatures*numScales)
    kMeansProg = '../../Build/Experiments/Experiment-1/KMeansProfiling'

    features = '../../Data/Dataset-1/Instances/instances2500.csv'

    args = [
        'time',
        'valgrind', '--tool=callgrind',
        kMeansProg, '--nHistograms', numHistograms, '--input', features,
    ]
    if subprocess.call( args ) != 0:
        print('Error:', args )
        
    return 0
            

if __name__ == '__main__':
    sys.exit( main() )

