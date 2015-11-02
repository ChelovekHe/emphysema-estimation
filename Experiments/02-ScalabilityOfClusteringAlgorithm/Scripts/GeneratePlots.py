#!/usr/bin/python3
import sys, os, os.path, subprocess, math, csv, random
from Util import intersperse 

def main():
    skip = {
        'Make plots' : False,
    }
        
    basedir = './'

    dirs = {
        'Statistics' : os.path.join(basedir, 'Statistics'),
        'Scripts' : os.path.join(basedir, 'Scripts'),
        'Plots' : os.path.join(basedir, 'Plots'),
        'Bin' : '../../Build',
    }
    
    files = {
        'Scalability-1' : {
            'infile' : os.path.join(dirs['Statistics'], 'Scalability-1', 'runtime_stats.csv'),
            'outfile' : os.path.join(dirs['Plots'], 'Scalability-1_Runtime.ps'),
        },
        'Scalability-2' : {
            'infile' : os.path.join(dirs['Statistics'], 'Scalability-2', 'runtime_stats.csv'),
        'outfile' : os.path.join(dirs['Plots'], 'Scalability-2_Runtime.ps'),
        },
    }
    
    progs = {
        'PlotScalabilityStats' : os.path.join(dirs['Scripts'], 'PlotScalabilityStats.R'),
    }

    params = {
        'Scalability-1' : {
            'title' : "Scalability of k-means. Each measurement is averaged over 100 runs. 56 histograms with 41 bins",
        },
        'Scalability-2' : {
            'title' : "Scalability of k-means. Branching = 16, 100 runs, 56 histograms, 41 bins",
        },
    }

    for ex in ['Scalability-1', 'Scalability-2']:
        print( 'Making', ex, 'plot' )
        args = [
            progs['PlotScalabilityStats'],
            files[ex]['infile'],
            files[ex]['outfile'],
            params[ex]['title'],
        ]
        print(' '.join(args))        
        if subprocess.call( args ) != 0:
            print( 'Error plotting', ex )
            return 1
        
    return 0

if __name__ == '__main__':
    sys.exit( main() )

