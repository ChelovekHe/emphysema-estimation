#!/usr/bin/python3
import sys, os, os.path, subprocess, math, csv, random
from Util import intersperse 

def main():       
    basedir = './'

    dirs = {
        'Statistics' : os.path.join(basedir, 'Statistics'),
        'Scripts' : os.path.join(basedir, 'Scripts'),
        'Plots' : os.path.join(basedir, 'Plots'),
        'Bin' : '../../Build',
    }
    
    files = {
        'Scalability-Thesis-1' : {
            'infile' : os.path.join(dirs['Statistics'], 'Scalability-Thesis', 'Scalability-Thesis-1_stats.csv'),
            'outfile' : os.path.join(dirs['Plots'], 'Scalability-Thesis-1.ps'),
        },
        'Scalability-Thesis-2' : {
            'infile' : os.path.join(dirs['Statistics'], 'Scalability-Thesis', 'Scalability-Thesis-2_stats.csv'),
            'outfile' : os.path.join(dirs['Plots'], 'Scalability-Thesis-2.ps'),
        },
        'Scalability-Thesis-3' : {
            'infile' : os.path.join(dirs['Statistics'], 'Scalability-Thesis', 'Scalability-Thesis-3_stats.csv'),
            'outfile' : os.path.join(dirs['Plots'], 'Scalability-Thesis-3.ps'),
        },
        'Scalability-Thesis-4' : {
            'infile' : os.path.join(dirs['Statistics'], 'Scalability-Thesis', 'Scalability-Thesis-4_stats.csv'),
            'outfile' : os.path.join(dirs['Plots'], 'Scalability-Thesis-4.ps'),
        },
    }
    
    progs = {
        'PlotScalabilityStats' : os.path.join(dirs['Scripts'], 'PlotScalabilityStats.R'),
    }

    params = {
        'Scalability-Thesis-1' : {
            'title' : "Scalability of k-means. Branching = 2, k-means iterations = 11, runs = 100.",
        },
        'Scalability-Thesis-2' : {
            'title' : "Scalability of k-means. Branching = k, k-means iterations = 11, runs = 100.",
        },
        'Scalability-Thesis-3' : {
            'title' : "Scalability of k-means. Branching = 2, k-means iterations = -1, runs = 100.",
        },
        'Scalability-Thesis-4' : {
            'title' : "Scalability of k-means. Branching = k, k-means iterations = -1, runs = 100.",
        },
    }

    experiments = [
        'Scalability-Thesis-1',
        'Scalability-Thesis-2',
        'Scalability-Thesis-3',
        'Scalability-Thesis-4',
    ]
    
    for ex in experiments:
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

