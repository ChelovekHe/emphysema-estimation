#!/usr/bin/python3
'''Run Experiment-1: Scalability.
See README.md for details.
'''

import sys, subprocess, os.path

def main(argv=None):
    if argv is None:
        argv = sys.argv

    numFeatures = 8
    numScales = 7
    numHistograms = '%d' % (numFeatures*numScales)
    scalabilityProg = '../../Build/Experiments/Experiment-1/Scalability-3'

    outDir = 'Scalability-3'
    
    features = [
        ('../../Data/Dataset-1/Instances/instances500.csv', 'stats500'),
        ('../../Data/Dataset-1/Instances/instances1000.csv', 'stats1000'),
        ('../../Data/Dataset-1/Instances/instances2500.csv', 'stats2500'),
        ('../../Data/Dataset-1/Instances/instances5000.csv', 'stats5000'),
        ]

    for feature,stat in features:
        args = [
            scalabilityProg,
            '--nHistograms', numHistograms,
            '--input', feature,
            '--output', os.path.join(outDir, stat)
        ]
        if subprocess.call( args ) != 0:
            print('Error:', args )
            
    return 0
            

if __name__ == '__main__':
    sys.exit( main() )
