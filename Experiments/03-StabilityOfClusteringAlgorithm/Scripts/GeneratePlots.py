#!/usr/bin/python3
import sys, os, os.path, subprocess, math, csv, random
from Util import intersperse 

def main():
    skip = {
        'Make plots' : False,
    }
        
    basedir = './'

    dirs = {
        'Distances' : os.path.join(basedir, 'Distances'),
        'Scripts' : os.path.join(basedir, 'Scripts'),
        'Plots' : os.path.join(basedir, 'Plots'),
    }

    experiments =  [
        'Stability-Thesis-1',
        'Stability-Thesis-2',
        'Stability-Thesis-3',
        'Stability-Thesis-4',
    ]
        
    infilePattern = '%s_instances1000.csv'
    outfilePattern = '%s_DistanceDistribution.ps'

    files = {}
    for ex in experiments:
        files[ex] = {
            'infile' : os.path.join(dirs['Distances'], infilePattern % ex),
            'outfile' : os.path.join(dirs['Plots'], outfilePattern % ex)
        }
    
    progs = {
        'Plot' : os.path.join(dirs['Scripts'], 'StabilityPlots.R'),
    }

    for ex in experiments:
        print( 'Making', ex, 'plot' )
        args = [
            progs['Plot'],
            files[ex]['infile'],
            files[ex]['outfile'],
            ex
        ]
        print(' '.join(args))        
        if subprocess.call( args ) != 0:
            print( 'Error plotting', ex )
            return 1
        
    return 0

if __name__ == '__main__':
    sys.exit( main() )

