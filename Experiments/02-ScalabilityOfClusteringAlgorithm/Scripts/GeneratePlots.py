#!/usr/bin/python3
import sys, os, os.path, subprocess, math, csv, random
from Util import intersperse 

def main():
    skip = {
        'Make histograms' : False,
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
    }
    
    progs = {
        'PlotScalabilityStats' : os.path.join(dirs['Scripts'], 'PlotScalabilityStats.R'),
    }

    params = {
        'Scalability-1' : {
            'title' : "Scalability of k-means. Each measurement is averaged over 100 runs. 56 histograms with 41 bins",
        },
    }


    print( 'Making Scalability-1 plot' )
    args = [
        progs['PlotScalabilityStats'],
        files['Scalability-1']['infile'],
        files['Scalability-1']['outfile'],
        params['Scalability-1']['title'],
    ]
    print(' '.join(args))        
    if subprocess.call( args ) != 0:
        print( 'Error plotting Scalability-1' )
        return 1
    return 0

if __name__ == '__main__':
    sys.exit( main() )

