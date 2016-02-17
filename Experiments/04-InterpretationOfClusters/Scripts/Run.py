#!/usr/bin/python3

import sys, subprocess, os.path

def main():
    skip = []
    
    basedir = ''

    dirs = {
        'Scripts' : os.path.join(basedir, 'Scripts'),
    }
    
    steps = [
        os.path.join(dirs['Scripts'], 'GenerateLabels.py'),
        os.path.join(dirs['Scripts'], 'CollateLabels.py'),
        os.path.join(dirs['Scripts'], 'GenerateStatistics.py'),
        os.path.join(dirs['Scripts'], 'GenerateEntropyPlots.py'),
    ]

    for step in steps:
        if step in skip:
            print('Skipping', step)
        else:
            print('Running', step)
            if subprocess.call(step) != 0:
                print ( 'Error calling', step )
                return 1


if __name__ == '__main__':
    sys.exit(main())
